#include "MainComponent.h"
#include "AppConfig.h"

#include <cmath>

#include <iostream>


// ============================================================
// CLOSEABLE DIALOG WINDOW
// ============================================================

class CloseableDialogWindow
    : public juce::DialogWindow
{
public:

    CloseableDialogWindow(
        const juce::String& title,
        juce::Colour backgroundColour,
        std::function<void()> closeCallback
    )
        : juce::DialogWindow(
            title,
            backgroundColour,
            true
        ),
          onClose(std::move(closeCallback))
    {
    }

    void closeButtonPressed() override
    {
        if (onClose)
            onClose();
    }

private:

    std::function<void()> onClose;
};



void OfforLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button, const juce::Colour& background,
    bool isHighlighted, bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto fill = button.getToggleState()
        ? juce::Colour(0xffff7a00)
        : background;
    if (isDown) fill = fill.darker(0.12f);
    else if (isHighlighted) fill = fill.brighter(0.12f);
    if (!button.isEnabled()) fill = fill.withAlpha(0.45f);

    g.setColour(fill);
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colour(0xff343b41).withAlpha(button.isEnabled() ? 1.0f : 0.5f));
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
}

void OfforLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button, bool, bool isDown)
{
    const auto textColour = button.getToggleState()
        ? juce::Colours::black
        : button.findColour(juce::TextButton::textColourOffId);
    g.setColour(button.isEnabled() ? textColour : textColour.withAlpha(0.45f));
    g.setFont(juce::Font(juce::FontOptions().withHeight(12.0f).withStyle("Bold")));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(8, isDown ? 2 : 0),
                     juce::Justification::centred, 1);
}

void OfforLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isHighlighted, bool isDown)
{
    auto bounds = button.getLocalBounds();
    const int boxSize = 18;
    auto box = juce::Rectangle<float>(static_cast<float>(bounds.getX()),
                                      static_cast<float>(bounds.getCentreY() - boxSize / 2),
                                      static_cast<float>(boxSize), static_cast<float>(boxSize));
    g.setColour(button.getToggleState() ? juce::Colour(0xffff7a00) : juce::Colour(0xff20262b));
    g.fillRoundedRectangle(box, 4.0f);
    g.setColour(juce::Colour(0xff485057));
    g.drawRoundedRectangle(box, 4.0f, 1.0f);
    if (button.getToggleState())
    {
        juce::Path tick;
        tick.startNewSubPath(box.getX() + 4.0f, box.getCentreY());
        tick.lineTo(box.getX() + 7.0f, box.getBottom() - 4.0f);
        tick.lineTo(box.getRight() - 3.0f, box.getY() + 4.0f);
        g.setColour(juce::Colours::black);
        g.strokePath(tick, juce::PathStrokeType(2.0f));
    }
    if (isHighlighted || isDown)
        g.setColour(juce::Colours::white.withAlpha(0.08f));
    else
        g.setColour(button.findColour(juce::ToggleButton::textColourId));
    g.setFont(juce::Font(juce::FontOptions().withHeight(12.0f)));
    g.drawFittedText(button.getButtonText(), bounds.withTrimmedLeft(boxSize + 9),
                     juce::Justification::centredLeft, 2);
}

void OfforLookAndFeel::drawComboBox(
    juce::Graphics& g, int width, int height, bool isButtonDown,
    int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused(buttonX, buttonY, buttonW, buttonH);
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width),
                                         static_cast<float>(height)).reduced(1.0f);
    g.setColour(isButtonDown ? juce::Colour(0xff292f34) : juce::Colour(0xff191e22));
    g.fillRoundedRectangle(bounds, 7.0f);
    g.setColour(juce::Colour(0xff343b41));
    g.drawRoundedRectangle(bounds, 7.0f, 1.0f);
    juce::Path arrow;
    const float x = static_cast<float>(width - 18);
    const float y = static_cast<float>(height / 2 - 2);
    arrow.startNewSubPath(x - 4.0f, y);
    arrow.lineTo(x, y + 4.0f);
    arrow.lineTo(x + 4.0f, y);
    g.setColour(box.isEnabled() ? juce::Colour(0xffff8a35) : juce::Colours::grey);
    g.strokePath(arrow, juce::PathStrokeType(2.0f));
}

// ==============================================================
// CONSTRUCTOR
// ==============================================================

MainComponent::MainComponent()
{
    setLookAndFeel(&offorLookAndFeel);

    // ======================================================
    // LICENSE / INSTALLATION ID
    // ======================================================

    loadOrCreateInstallationId();

    juce::PropertiesFile::Options userOptions;
    userOptions.applicationName = "OfforStemSplitterPreferences";
    userOptions.filenameSuffix = ".settings";
    userOptions.folderName = "OFFOR";
    userOptions.storageFormat = juce::PropertiesFile::storeAsXML;
    userPreferences.setStorageParameters(userOptions);
    if (auto* prefs = userPreferences.getUserSettings())
    {
        autoPlayEnabled = prefs->getBoolValue("autoPlay", false);
        loopSelectionEnabled = prefs->getBoolValue("loopSelection", false);
        normalizeExportsEnabled = prefs->getBoolValue("normalizeExports", false);
        sixStemModeEnabled = prefs->getBoolValue("sixStemMode", true);
        exportFormatId = prefs->getIntValue("exportFormat", 1);
        masterVolume = juce::jlimit(0.0, 1.0, prefs->getDoubleValue("masterVolume", 1.0));
    }

    juce::Component::SafePointer<MainComponent> safeThis(this);
    juce::Timer::callAfterDelay(
        2000,
        [safeThis]()
        {
            if (safeThis != nullptr)
                safeThis->checkForUpdates();
        }
    );


    // ==========================================================
    // KEYBOARD FOCUS
    // ==========================================================

    setWantsKeyboardFocus(true);
    setMouseClickGrabsKeyboardFocus(true);

    addKeyListener(this);

    // ==========================================================
    // PLAYBACK TIMER
    // ==========================================================

    playbackTimer =
        std::make_unique<PlaybackTimer>(*this);


    // ==========================================================
    // SEPARATION ANIMATION TIMER
    // ==========================================================

    progressAnimationTimer =
        std::make_unique<ProgressAnimationTimer>(*this);

    // ==========================================================
    // Audio format
    // ==========================================================

    formatManager.registerBasicFormats();

    // ==========================================================
    // Audio device
    // ==========================================================

    auto error =
        audioDeviceManager.initialise(
            0,
            2,
            nullptr,
            true
        );

    if (error.isNotEmpty())
    {
        setStatus(
            "Audio device error: " + error
        );
    }

    audioSourcePlayer.setSource(
        &mixerSource
    );

    audioDeviceManager.addAudioCallback(
        &audioSourcePlayer
    );

    // ==========================================================
    // Window
    // ==========================================================

    setSize(1200, 700);

    setWantsKeyboardFocus(true);

    // ==========================================================
    // OFFOR BRANDING
    // ==========================================================

    offorLogo =
        juce::ImageCache::getFromMemory(
            BinaryData::OfforStemSplitter_png,
            BinaryData::OfforStemSplitter_pngSize
        );

    headerTitleLabel.setText(
        "OFFOR STEM SPLITTER",
        juce::dontSendNotification
    );

    headerTitleLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(18.0f)
                .withStyle("Bold")
        )
    );

    headerTitleLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffd7d7d7)
    );

    headerTitleLabel.setJustificationType(
        juce::Justification::centredLeft
    );

    addAndMakeVisible(
        headerTitleLabel
    );

    // ==========================================================
    // CANCEL BUTTON
    // ==========================================================

    addAndMakeVisible(
        cancelButton
    );

    cancelButton.setVisible(
        false
    );

    cancelButton.onClick = [this]
    {
        cancelSeparator();
    };

    cancelButton.setColour(
        juce::TextButton::buttonColourId,
        juce::Colour(0xffff7a00)
    );

    cancelButton.setColour(
        juce::TextButton::textColourOffId,
        juce::Colours::black
    );

    cancelButton.setColour(
        juce::TextButton::buttonOnColourId,
        juce::Colour(0xffff8c1a)
    );

    cancelButton.setColour(
        juce::TextButton::textColourOnId,
        juce::Colours::black
    );

    // ==========================================================
    // TRANSPORT
    // ==========================================================

    addAndMakeVisible(
        playButton
    );

    addAndMakeVisible(
        pauseButton
    );

    addAndMakeVisible(
        stopButton
    );

    playButton.onClick = [this]
    {
        startPlayback();
    };

    pauseButton.onClick = [this]
    {
        pausePlayback();
    };

    stopButton.onClick = [this]
    {
        stopPlayback();
    };

    // ==========================================================
    // TRANSPORT BUTTON STYLE
    // ==========================================================

    auto setupTransportButton =
        [](juce::TextButton& button)
    {
        button.setColour(
            juce::TextButton::buttonColourId,
            juce::Colour(0xff181c20)
        );

        button.setColour(
            juce::TextButton::buttonOnColourId,
            juce::Colour(0xffff7a00)
        );

        button.setColour(
            juce::TextButton::textColourOffId,
            juce::Colours::white
        );

        button.setColour(
            juce::TextButton::textColourOnId,
            juce::Colours::black
        );
    };

    setupTransportButton(
        playButton
    );

    setupTransportButton(
        pauseButton
    );

    setupTransportButton(
        stopButton
    );

    playButton.setTooltip("Play from the playhead; a highlighted region limits playback.");
    pauseButton.setTooltip("Pause and keep the current playhead position.");
    stopButton.setTooltip("Stop and return to the highlighted region start, or the beginning.");

    masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(masterVolume, juce::dontSendNotification);
    masterVolumeSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff343b41));
    masterVolumeSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff7a00));
    masterVolumeSlider.setTooltip("Master playback volume for all stems.");
    masterVolumeSlider.onValueChange = [this]
    {
        masterVolume = masterVolumeSlider.getValue();
        audioSourcePlayer.setGain(static_cast<float>(masterVolume));
        masterVolumeLabel.setText("MASTER  " + juce::String(juce::roundToInt(masterVolume * 100.0)) + "%", juce::dontSendNotification);
        if (auto* prefs = userPreferences.getUserSettings())
        {
            prefs->setValue("masterVolume", masterVolume);
            prefs->saveIfNeeded();
        }
    };
    masterVolumeLabel.setText("MASTER  " + juce::String(juce::roundToInt(masterVolume * 100.0)) + "%", juce::dontSendNotification);
    masterVolumeLabel.setFont(juce::Font(juce::FontOptions().withHeight(10.0f).withStyle("Bold")));
    masterVolumeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc7cbd0));
    masterVolumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterVolumeLabel);
    addAndMakeVisible(masterVolumeSlider);
    audioSourcePlayer.setGain(static_cast<float>(masterVolume));

    // ==========================================================
    // FILE LABEL
    // ==========================================================

    fileLabel.setText(
        "Drag an audio file here or click to browse",
        juce::dontSendNotification
    );

    fileLabel.setJustificationType(
        juce::Justification::centred
    );

    fileLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(10.0f)
        )
    );

    addAndMakeVisible(
        fileLabel
    );

    // ==========================================================
    // STATUS
    // ==========================================================

    statusLabel.setText(
        "Ready for you...",
        juce::dontSendNotification
    );

    statusLabel.setJustificationType(
        juce::Justification::centred
    );

    statusLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(16.0f)
        )
    );

    statusLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff9a9a9a)
    );

    addAndMakeVisible(
        statusLabel
    );

    // ==========================================================
    // PROGRESS
    // ==========================================================

    progressLabel.setText(
        "",
        juce::dontSendNotification
    );

    progressLabel.setJustificationType(
        juce::Justification::centred
    );

    progressLabel.setFont(
        juce::Font(21.0f)
    );

    progressLabel.setVisible(
        false
    );

    addAndMakeVisible(
        progressLabel
    );


    // ==========================================================
    // COMPANY
    // ==========================================================

    addAndMakeVisible(
        companyLabel
    );

    companyLabel.setText(
        "ONNTECH",
        juce::dontSendNotification
    );

    companyLabel.setJustificationType(
        juce::Justification::centredLeft
    );

    companyLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff8f8f8f)
    );

    companyLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(16.0f)
        )
    );

    // ==========================================================
    // VERSION
    // ==========================================================

    addAndMakeVisible(
        versionLabel
    );

    versionLabel.setText(
        "Version " + juce::String(OfforStemSplitter::VERSION),
        juce::dontSendNotification
    );

    versionLabel.setJustificationType(
        juce::Justification::centredRight
    );

    versionLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff8f8f8f)
    );

    versionLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(16.0f)
        )
    );


    // ==========================================================
    // SETTINGS SIDEBAR
    settingsComponent = std::make_unique<SettingsComponent>();
    settingsComponent->setEmbeddedMode(true);
    settingsComponent->setLicenseActivated(licenseManager.isActivated());
    settingsComponent->setAutoPlayEnabled(autoPlayEnabled);
    settingsComponent->setLoopSelectionEnabled(loopSelectionEnabled);
    settingsComponent->setNormalizeEnabled(normalizeExportsEnabled);
    settingsComponent->setExportFormat(exportFormatId);
    settingsComponent->setSixStemModeEnabled(sixStemModeEnabled);
    settingsComponent->onActivateLicense = [this] { showLicenseDialog(); };
    settingsComponent->onSettingsChanged =
        [this](bool autoPlay, bool loopSelection, int exportFormat,
               bool normalize, bool sixStemMode)
        {
            autoPlayEnabled = autoPlay;
            loopSelectionEnabled = loopSelection;
            exportFormatId = exportFormat;
            normalizeExportsEnabled = normalize;
            sixStemModeEnabled = sixStemMode;
            if (auto* prefs = userPreferences.getUserSettings())
            {
                prefs->setValue("autoPlay", autoPlayEnabled);
                prefs->setValue("loopSelection", loopSelectionEnabled);
                prefs->setValue("normalizeExports", normalizeExportsEnabled);
                prefs->setValue("sixStemMode", sixStemModeEnabled);
                prefs->setValue("exportFormat", exportFormatId);
                prefs->saveIfNeeded();
            }
        };
    addAndMakeVisible(*settingsComponent);

    // ==========================================================
    
    // EXPORT BUTTONS
    // ==========================================================

    exportSelectionButton.setButtonText(
        "EXPORT SELECTED"
    );

    exportSelectionButton.onClick = [this]
    {
        exportSelectedAudio();
    };

    addAndMakeVisible(
        exportAllButton
    );

    exportAllButton.setButtonText(
        "EXPORT ALL"
    );

    exportAllButton.onClick = [this]
    {
        exportAllStems();
    };

    addAndMakeVisible(
        openOutputButton
    );

    openOutputButton.setButtonText(
        "OPEN OUTPUT FOLDER"
    );

    openOutputButton.onClick = [this]
    {
        openOutputFolder();
    };

    // ==========================================================
    // STEM LABELS
    // ==========================================================

    auto setupStemLabel =
        [](juce::Label& label,
        const juce::String& text)
    {
        label.setText(
            text,
            juce::dontSendNotification
        );

        label.setJustificationType(
            juce::Justification::centredLeft
        );

        label.setColour(
            juce::Label::textColourId,
            juce::Colour(0xffd7d7d7)
        );
    };

    setupStemLabel(
        vocalsLabel,
        "01  Vocals"
    );

    setupStemLabel(
        drumsLabel,
        "02  Drums"
    );

    setupStemLabel(
        bassLabel,
        "03  Bass"
    );

    setupStemLabel(
        instrumentalLabel,
        "04  Other"
    );
    setupStemLabel(guitarLabel, "05  Guitar");
    setupStemLabel(pianoLabel, "06  Piano");

    // ==========================================================
    // STEM COMPONENTS
    // ==========================================================

    addAndMakeVisible(
        vocalsTrack
    );

    addAndMakeVisible(
        drumsTrack
    );

    addAndMakeVisible(
        bassTrack
    );

    addAndMakeVisible(
        instrumentalTrack
    );
    addAndMakeVisible(guitarTrack);
    addAndMakeVisible(pianoTrack);

    // ==========================================================
    // CALLBACKS
    // ==========================================================

    setupSeekCallbacks();

    setupStemTrackCallbacks();

    // ==========================================================
    // INITIAL STATE
    // ==========================================================

    clearSeparatedStems();

    setTransportVisible(
        false
    );

    resized();

    juce::MessageManager::callAsync(
        [this]
        {
            if (isShowing())
                grabKeyboardFocus();
        }
    );

}

// ============================================================
// CHECK LICENSE BEFORE SEPARATION
// ============================================================

void MainComponent::checkLicenseBeforeSeparation()
{
    // --------------------------------------------------------
    // 1. ACTIVATED USERS
    //
    // Paid users can always use the application.
    // No internet connection is required.
    // --------------------------------------------------------

    if (licenseManager.isActivated())
    {
        runSeparator();
        return;
    }

    // --------------------------------------------------------
    // 2. LOCAL FREE USES
    //
    // Allow the user to continue offline while local
    // free uses are still available.
    // --------------------------------------------------------

    if (licenseManager.hasFreeUsesRemaining())
    {
        runSeparator();
        return;
    }

    // --------------------------------------------------------
    // No local free uses remain; display activation options.
    showLicenseDialog();
}

// ============================================================
// SHOW LICENSE DIALOG
// ============================================================

void MainComponent::showLicenseDialog()
{
    if (licenseWindow != nullptr)
        return;

    auto* content = new juce::Component();

    content->setSize(
        420,
        240
    );

    auto* title = new juce::Label(
        {},
        "OFFOR STEM SPLITTER"
    );

    title->setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(22.0f)
                .withStyle("bold")
        )
    );

    title->setJustificationType(
        juce::Justification::centred
    );

    content->addAndMakeVisible(title);

    title->setBounds(
        20,
        20,
        380,
        35
    );


    auto* message = new juce::Label(
        {},
        "Your 60 free separations have been used."
    );

    message->setJustificationType(
        juce::Justification::centred
    );

    content->addAndMakeVisible(message);

    message->setBounds(
        20,
        65,
        380,
        30
    );


    auto* subMessage = new juce::Label(
        {},
        "Purchase a license and enter your license code."
    );

    subMessage->setJustificationType(
        juce::Justification::centred
    );

    content->addAndMakeVisible(subMessage);

    subMessage->setBounds(
        20,
        95,
        380,
        30
    );


    auto* licenseEditor =
        new juce::TextEditor();

    licenseEditor->setTextToShowWhenEmpty(
        "Enter license code...",
        juce::Colours::grey
    );

    licenseEditor->setJustification(
        juce::Justification::centred
    );

    content->addAndMakeVisible(
        licenseEditor
    );

    licenseEditor->setBounds(
        55,
        135,
        310,
        35
    );


    auto* activateButton =
        new juce::TextButton(
            "ACTIVATE"
        );

    content->addAndMakeVisible(
        activateButton
    );

    activateButton->setBounds(
        140,
        185,
        140,
        35
    );


    auto* buyButton =
        new juce::TextButton(
            "BUY LICENSE"
        );

    content->addAndMakeVisible(
        buyButton
    );

    buyButton->setBounds(
        140,
        230,
        140,
        35
    );


    activateButton->onClick =
        [this, licenseEditor]()
    {
        const auto key =
            licenseEditor->getText()
                .trim();

        if (licenseManager.activate(key))
        {
            if (licenseWindow != nullptr)
                licenseWindow->closeButtonPressed();

            if (settingsComponent != nullptr)
            {
                settingsComponent->setLicenseActivated(true);
            }

            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "License Activated",
                "Thank you. Offor Stem Splitter is now activated."
            );

            runSeparator();
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Invalid License",
                "The license code is invalid."
            );
        }
    };


    buyButton->onClick =
        []
    {
        juce::URL(
            "https://ko-fi.com/s/e91086f9f4"
        ).launchInDefaultBrowser();
    };


    licenseWindow =
        std::make_unique<CloseableDialogWindow>(
            "Offor Stem Splitter License",
            juce::Colours::black,

            [this]
            {
                licenseWindow.reset();
            }
        );

    licenseWindow->setContentOwned(
        content,
        true
    );

    licenseWindow->centreWithSize(
        420,
        320
    );

    licenseWindow->setResizable(
        false,
        false
    );

    licenseWindow->setVisible(true);
}


// ==============================================================
// DESTRUCTOR
// ==============================================================

MainComponent::~MainComponent()
{
    removeKeyListener(this);

    // ==========================================================
    // STOP TIMERS
    // ==========================================================

    if (playbackTimer)
        playbackTimer->stopTimer();

    if (progressAnimationTimer)
        progressAnimationTimer->stopTimer();

    stopTimer();

    // ==========================================================
    // STOP PLAYBACK
    // ==========================================================

    stopPlayback();

    unloadStemAudio();

    audioSourcePlayer.setSource(
        nullptr
    );

    audioDeviceManager.removeAudioCallback(
        &audioSourcePlayer
    );

    // ==========================================================
    // STOP SEPARATOR PROCESS
    // ==========================================================

    if (separatorProcess != nullptr)
    {
        if (separatorProcess->isRunning())
            separatorProcess->kill();
    }

    // ==========================================================
    // STOP SEPARATOR OUTPUT THREAD
    // ==========================================================

    stopSeparatorOutputThread();
    setLookAndFeel(nullptr);

    // ==========================================================
    // DESTROY PROCESS
    //
    // Only destroy it after the output thread has stopped.
    // ==========================================================

    if (separatorOutputThread == nullptr)
    {
        separatorProcess.reset();
    }
}



// ==========================================================
// PAINT
// ==========================================================

void MainComponent::paint(juce::Graphics& g)
{
    const bool showLargeDropZone = !vocalsTrack.isVisible();
    g.fillAll(juce::Colour(0xff080a0c));

    auto panel = [](juce::Graphics& graphics, juce::Rectangle<int> bounds)
    {
        graphics.setColour(juce::Colour(0xff111417));
        graphics.fillRoundedRectangle(bounds.toFloat(), 12.0f);
        graphics.setColour(juce::Colour(0xff2a3035));
        graphics.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), 12.0f, 1.0f);
    };
    panel(g, settingsPanelBounds);
    panel(g, workspacePanelBounds);

    g.setColour(juce::Colour(0xff191e22));
    g.fillRoundedRectangle(workspaceHeaderBounds.toFloat(), 9.0f);
    g.setColour(juce::Colour(0xffff7a00).withAlpha(0.75f));
    g.fillRoundedRectangle(workspaceHeaderBounds.getX() + 12.0f,
                           workspaceHeaderBounds.getBottom() - 2.0f,
                           44.0f, 2.0f, 1.0f);

    if (!offorLogo.isNull())
    {
        const auto logoBounds = workspaceHeaderBounds.withX(workspaceHeaderBounds.getX() + 12)
            .withWidth(36).reduced(4, 6);
        g.drawImageWithin(offorLogo, logoBounds.getX(), logoBounds.getY(),
                          logoBounds.getWidth(), logoBounds.getHeight(),
                          juce::RectanglePlacement::centred |
                          juce::RectanglePlacement::onlyReduceInSize);
    }

    auto drop = dropZoneBounds.toFloat();
    g.setColour(juce::Colour(0xff0c0f11));
    g.fillRoundedRectangle(drop, 10.0f);
    g.setColour((isDragOver ? juce::Colour(0xffff7a00) : juce::Colour(0xff3a4147))
                    .withAlpha(isDragOver ? 1.0f : 0.9f));
    g.drawRoundedRectangle(drop.reduced(0.5f), 10.0f, isDragOver ? 2.0f : 1.0f);
    if (separationRunning)
    {
        constexpr int barCount = 43;
        constexpr float barWidth = 5.0f;
        constexpr float gap = 7.0f;
        const float visualizerWidth = barCount * (barWidth + gap) - gap;
        const float startX = drop.getCentreX() - visualizerWidth * 0.5f;
        const float centerY = drop.getY() + drop.getHeight() * 0.48f;
        const float animationPhase = static_cast<float>(progressAnimationFrame) * 0.18f;

        for (int i = 0; i < barCount; ++i)
        {
            const float position = static_cast<float>(i) / static_cast<float>(barCount - 1);
            const float envelope = 0.25f + 0.75f * std::sin(position * juce::MathConstants<float>::pi);
            const float wave = 0.22f + 0.78f * std::abs(std::sin(animationPhase - static_cast<float>(i) * 0.31f));
            const float height = 7.0f + 43.0f * envelope * wave;
            const float x = startX + static_cast<float>(i) * (barWidth + gap);
            const auto bar = juce::Rectangle<float>(x, centerY - height * 0.5f, barWidth, height);
            g.setColour(juce::Colour(0xffff7a00).withAlpha(0.42f + 0.5f * wave));
            g.fillRoundedRectangle(bar, barWidth * 0.5f);
        }

        g.setColour(juce::Colour(0xffe3e5e7).withAlpha(0.9f));
        g.setFont(juce::Font(juce::FontOptions().withHeight(12.0f).withStyle("Bold")));
        g.drawText("OFFOR IS SPLITTING YOUR AUDIO", drop.withTop(centerY + 36.0f),
                   juce::Justification::centredTop);
    }
    else
    {
        g.setColour(juce::Colour(0xffff7a00).withAlpha(0.9f));
        g.setFont(juce::Font(juce::FontOptions().withHeight(22.0f).withStyle("Bold")));
        g.drawText(showLargeDropZone ? "+" : "DROP AUDIO", drop.removeFromTop(showLargeDropZone ? 78.0f : 36.0f),
                   juce::Justification::centred);
        g.setColour(juce::Colours::white.withAlpha(0.86f));
        g.setFont(juce::Font(juce::FontOptions().withHeight(14.0f)));
        g.drawText(showLargeDropZone ? "Drop an audio file here or click to browse"
                                     : "Drop or click to choose another audio file",
                   drop, juce::Justification::centred);
    }

    if (separationRunning || progressLabel.isVisible())
    {
        g.setColour(juce::Colour(0xff252b30));
        g.fillRoundedRectangle(progressBarBounds.toFloat(), 4.0f);
        const float progressWidth = static_cast<float>(progressBarBounds.getWidth())
                                    * static_cast<float>(separationProgress);
        if (progressWidth > 0.0f)
        {
            g.setColour(juce::Colour(0xffff7a00));
            g.fillRoundedRectangle(progressBarBounds.withWidth(
                juce::jmin(progressBarBounds.getWidth(), static_cast<int>(progressWidth)))
                .toFloat(), 4.0f);
        }
    }

    if (vocalsTrack.isVisible())
    {
        g.setColour(juce::Colour(0xffaab1b7));
        g.setFont(juce::Font(juce::FontOptions().withHeight(11.0f).withStyle("Bold")));
        g.drawText(loadedSixStemOutput ? "SIX SEPARATED STEMS" : "SEPARATED STEMS",
                   tracksHeaderBounds, juce::Justification::centredLeft);
    }
}


// ==============================================================
// MOUSE DOWN
// ==============================================================

void MainComponent::mouseDown(
    const juce::MouseEvent& event
)
{
    if (!separationRunning && dropZoneBounds.contains(
            event.getPosition()))
    {
        selectAudioFile();
        return;
    }
}


// ==========================================================
// RESIZED
// ==========================================================

void MainComponent::resized()
{
    auto outer = getLocalBounds().reduced(14);
    settingsPanelBounds = outer.removeFromLeft(286);
    outer.removeFromLeft(12);
    workspacePanelBounds = outer;

    if (settingsComponent)
        settingsComponent->setBounds(settingsPanelBounds.reduced(1));

    auto area = workspacePanelBounds.reduced(16);
    workspaceHeaderBounds = area.removeFromTop(46);
    auto header = workspaceHeaderBounds.reduced(8, 0);
    header.removeFromLeft(42);
    versionLabel.setBounds(header.removeFromRight(108));
    companyLabel.setBounds(header.removeFromRight(84));
    headerTitleLabel.setBounds(header.reduced(4, 0));

    area.removeFromTop(10);
    const bool hasStems = vocalsTrack.isVisible();
    const int dropHeight = hasStems ? 68 : 176;
    dropZoneBounds = area.removeFromTop(dropHeight);
    area.removeFromTop(4);
    fileLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(2);
    progressLabel.setBounds(area.removeFromTop(20));
    progressBarBounds = area.removeFromTop(7);
    area.removeFromTop(3);
    statusLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(7);

    const bool showTransport = hasStems;
    playButton.setVisible(showTransport);
    pauseButton.setVisible(showTransport);
    stopButton.setVisible(showTransport);
    masterVolumeLabel.setVisible(showTransport);
    masterVolumeSlider.setVisible(showTransport);
    exportSelectionButton.setVisible(showTransport);
    exportAllButton.setVisible(showTransport);
    openOutputButton.setVisible(showTransport);

    if (showTransport)
    {
        auto bottom = area.removeFromBottom(80);
        transportBounds = bottom.removeFromTop(38);
        bottom.removeFromTop(6);
        exportActionsBounds = bottom.removeFromTop(34);

        auto controlsArea = transportBounds.reduced(3, 1);
        auto masterArea = controlsArea.removeFromRight(220);
        masterVolumeLabel.setBounds(masterArea.removeFromLeft(86));
        masterVolumeSlider.setBounds(masterArea.reduced(5, 5));
        auto transport = controlsArea.withSizeKeepingCentre(
            juce::jmin(390, controlsArea.getWidth()), controlsArea.getHeight());
        const int eachTransport = transport.getWidth() / 3;
        playButton.setBounds(transport.removeFromLeft(eachTransport).reduced(3, 1));
        pauseButton.setBounds(transport.removeFromLeft(eachTransport).reduced(3, 1));
        stopButton.setBounds(transport.reduced(3, 1));

        auto actions = exportActionsBounds;
        const int actionWidth = actions.getWidth() / 3;
        exportSelectionButton.setBounds(actions.removeFromLeft(actionWidth).reduced(3, 1));
        exportAllButton.setBounds(actions.removeFromLeft(actionWidth).reduced(3, 1));
        openOutputButton.setBounds(actions.reduced(3, 1));

        tracksHeaderBounds = area.removeFromTop(20);
        area.removeFromTop(5);
        const int count = loadedSixStemOutput ? 6 : 4;
        const int rowHeight = juce::jlimit(52, 64, area.getHeight() / count - 4);
        auto placeTrack = [&area, rowHeight](StemTrackComponent& track)
        {
            track.setBounds(area.removeFromTop(rowHeight));
            area.removeFromTop(4);
        };
        placeTrack(vocalsTrack);
        placeTrack(drumsTrack);
        placeTrack(bassTrack);
        if (loadedSixStemOutput)
        {
            placeTrack(guitarTrack);
            placeTrack(pianoTrack);
        }
        placeTrack(instrumentalTrack);
    }
    else
    {
        transportBounds = {};
        exportActionsBounds = {};
        tracksHeaderBounds = {};
    }
}

// ==============================================================
// MOUSE CURSOR
// ==============================================================

juce::MouseCursor MainComponent::getMouseCursor()
{
    if (!separationRunning && dropZoneBounds.contains(
            getMouseXYRelative()))
    {
        return juce::MouseCursor::PointingHandCursor;
    }

    return juce::MouseCursor::NormalCursor;
}

// ==============================================================
// STATUS
// ==============================================================

void MainComponent::setStatus(
    const juce::String& message
)
{
    statusLabel.setText(
        message,
        juce::dontSendNotification
    );
}

// ==============================================================
// TRANSPORT VISIBILITY
// ==============================================================

void MainComponent::setTransportVisible(
    bool shouldBeVisible
)
{
    playButton.setVisible(
        shouldBeVisible
    );

    pauseButton.setVisible(
        shouldBeVisible
    );

    stopButton.setVisible(
        shouldBeVisible
    );
}

// ==============================================================
// STEM UI VISIBILITY
// ==============================================================

void MainComponent::setStemUIVisible(
    bool shouldBeVisible
)
{
    vocalsLabel.setVisible(
        shouldBeVisible
    );

    drumsLabel.setVisible(
        shouldBeVisible
    );

    bassLabel.setVisible(
        shouldBeVisible
    );

    instrumentalLabel.setVisible(
        shouldBeVisible
    );

    vocalsTrack.setVisible(
        shouldBeVisible
    );

    drumsTrack.setVisible(
        shouldBeVisible
    );

    bassTrack.setVisible(
        shouldBeVisible
    );

    instrumentalTrack.setVisible(
        shouldBeVisible
    );
    guitarTrack.setVisible(shouldBeVisible && loadedSixStemOutput);
    pianoTrack.setVisible(shouldBeVisible && loadedSixStemOutput);
}

// ==============================================================
// CLEAR STEMS
// ==============================================================

void MainComponent::clearSeparatedStems()
{
    setStemUIVisible(
        false
    );

    setTransportVisible(
        false
    );

    progressLabel.setText(
        "",
        juce::dontSendNotification
    );

    progressLabel.setVisible(
        false
    );

    unloadStemAudio();
    loadedSixStemOutput = false;

    repaint();
}

// ==============================================================
// SHOW STEMS
// ==============================================================

void MainComponent::showSeparatedStems()
{
    std::cout
        << "\n\n========================================\n"
        << "        SHOW SEPARATED STEMS\n"
        << "========================================\n"
        << std::endl;

    // ==========================================================
    // BUILD THE EXPECTED OUTPUT FOLDER
    // ==========================================================

    const auto documentsDirectory =
        juce::File::getSpecialLocation(
            juce::File::userDocumentsDirectory
        );

    const auto exportsRoot =
        documentsDirectory
            .getChildFile("Offor Stem Splitter")
            .getChildFile("Exports");

    const auto expectedFolder =
        exportsRoot.getChildFile(
            selectedFile.getFileNameWithoutExtension()
        );

    // ==========================================================
    // DEBUG
    // ==========================================================

    std::cout
        << "Expected output folder:\n"
        << expectedFolder.getFullPathName().toStdString()
        << "\nExists: "
        << (expectedFolder.exists() ? "YES" : "NO")
        << "\nDirectory: "
        << (expectedFolder.isDirectory() ? "YES" : "NO")
        << "\n"
        << std::endl;

    // ==========================================================
    // CHECK STEM FILES
    // ==========================================================

    const auto testVocals =
        expectedFolder.getChildFile(
            "01_Vocals.wav"
        );

    const auto testDrums =
        expectedFolder.getChildFile(
            "02_Drums.wav"
        );

    const auto testBass =
        expectedFolder.getChildFile(
            "03_Bass.wav"
        );

    const auto testInstrumental =
        expectedFolder.getChildFile(
            "04_Instrumental.wav"
        );

    std::cout
        << "VOCALS: "
        << (testVocals.existsAsFile() ? "YES" : "NO")
        << "\nDRUMS: "
        << (testDrums.existsAsFile() ? "YES" : "NO")
        << "\nBASS: "
        << (testBass.existsAsFile() ? "YES" : "NO")
        << "\nINSTRUMENTAL: "
        << (testInstrumental.existsAsFile() ? "YES" : "NO")
        << "\n========================================\n"
        << std::endl;

    // ==========================================================
    // OUTPUT FOLDER MUST EXIST
    // ==========================================================

    if (!expectedFolder.isDirectory())
    {
        progressLabel.setText(
            "",
            juce::dontSendNotification
        );

        setStatus(
            "ERROR: Output folder was not created."
        );

        return;
    }

    // ==========================================================
    // USE THE VERIFIED FOLDER
    // ==========================================================

    outputFolder = expectedFolder;

    // ==========================================================
    // VERIFY ALL STEM FILES
    // ==========================================================

    vocalsFile =
        outputFolder.getChildFile(
            "01_Vocals.wav"
        );

    drumsFile =
        outputFolder.getChildFile(
            "02_Drums.wav"
        );

    bassFile =
        outputFolder.getChildFile(
            "03_Bass.wav"
        );

    instrumentalFile =
        outputFolder.getChildFile(
            "04_Instrumental.wav"
        );
    guitarFile = outputFolder.getChildFile("05_Guitar.wav");
    pianoFile = outputFolder.getChildFile("06_Piano.wav");
    loadedSixStemOutput = guitarFile.existsAsFile() && pianoFile.existsAsFile();

    if (!vocalsFile.existsAsFile()
        || !drumsFile.existsAsFile()
        || !bassFile.existsAsFile()
        || !instrumentalFile.existsAsFile()
        || (sixStemModeEnabled && !loadedSixStemOutput))
    {
        progressLabel.setText(
            "",
            juce::dontSendNotification
        );

        setStatus(
            "ERROR: Stem files were not created correctly."
        );

        return;
    }

    // ==========================================================
    // LOAD STEM FILES
    // ==========================================================

    vocalsTrack.setAudioFile(vocalsFile);
    drumsTrack.setAudioFile(drumsFile);
    bassTrack.setAudioFile(bassFile);
    instrumentalTrack.setAudioFile(instrumentalFile);
    if (loadedSixStemOutput)
    {
        guitarTrack.setAudioFile(guitarFile);
        pianoTrack.setAudioFile(pianoFile);
    }

    if (!loadStemAudio())
    {
        progressLabel.setText(
            "",
            juce::dontSendNotification
        );

        setStatus(
            "ERROR: Could not load stem audio."
        );

        return;
    }

    // ==========================================================
    if (!licenseManager.isActivated())
        licenseManager.incrementUsage();

    // INITIAL PLAYBACK STATE
    // ==========================================================

    playbackLength =
        vocalsTrack.getAudioLength();

    playbackPosition = 0.0;

    vocalsTrack.setPlayheadPosition(0.0);
    drumsTrack.setPlayheadPosition(0.0);
    bassTrack.setPlayheadPosition(0.0);
    instrumentalTrack.setPlayheadPosition(0.0);
    guitarTrack.setPlayheadPosition(0.0);
    pianoTrack.setPlayheadPosition(0.0);

    // ==========================================================
    // SHOW STEM UI
    // ==========================================================

    setStemUIVisible(true);
    setTransportVisible(true);

    updateTrackMixing();

    // ==========================================================
    // COMPLETION MESSAGE
    // ==========================================================

    progressLabel.setVisible(true);

    progressLabel.setText(
        "Separation complete",
        juce::dontSendNotification
    );

    setStatus(loadedSixStemOutput
        ? "Finished: vocals, drums, bass, other, guitar, piano (piano is experimental)."
        : "Finished splitting the track.");

    // ==========================================================
    // EXPAND UI
    // ==========================================================

    expandForTracks();

    resized();

    // ==========================================================
    // AUTO PLAY
    // ==========================================================

    if (autoPlayEnabled)
    {
        juce::MessageManager::callAsync(
            [this]
            {
                if (playbackLength > 0.0
                    && !isPlaying)
                {
                    startPlayback();
                }
            }
        );
    }

    repaint();
}


// ==============================================================
// LOAD STEM AUDIO
// ==============================================================

bool MainComponent::loadStemAudio()
{
    unloadStemAudio();

    auto* vocalsReader =
        formatManager.createReaderFor(
            vocalsFile
        );

    auto* drumsReader =
        formatManager.createReaderFor(
            drumsFile
        );

    auto* bassReader =
        formatManager.createReaderFor(
            bassFile
        );

    auto* instrumentalReader =
        formatManager.createReaderFor(
            instrumentalFile
        );
    auto* guitarReader = loadedSixStemOutput ? formatManager.createReaderFor(guitarFile) : nullptr;
    auto* pianoReader = loadedSixStemOutput ? formatManager.createReaderFor(pianoFile) : nullptr;

    if (vocalsReader == nullptr
        || drumsReader == nullptr
        || bassReader == nullptr
        || instrumentalReader == nullptr
        || (loadedSixStemOutput && (guitarReader == nullptr || pianoReader == nullptr)))
    {
        delete vocalsReader;
        delete drumsReader;
        delete bassReader;
        delete instrumentalReader;
        delete guitarReader;
        delete pianoReader;

        return false;
    }

    vocalsReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            vocalsReader,
            true
        );

    drumsReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            drumsReader,
            true
        );

    bassReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            bassReader,
            true
        );

    instrumentalReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            instrumentalReader,
            true
        );
    if (loadedSixStemOutput)
    {
        guitarReaderSource = std::make_unique<juce::AudioFormatReaderSource>(guitarReader, true);
        pianoReaderSource = std::make_unique<juce::AudioFormatReaderSource>(pianoReader, true);
    }

    vocalsTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();

    drumsTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();

    bassTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();

    instrumentalTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();
    if (loadedSixStemOutput)
    {
        guitarTransport = std::make_unique<juce::AudioTransportSource>();
        pianoTransport = std::make_unique<juce::AudioTransportSource>();
    }

    vocalsTransport->setSource(
        vocalsReaderSource.get()
    );

    drumsTransport->setSource(
        drumsReaderSource.get()
    );

    bassTransport->setSource(
        bassReaderSource.get()
    );

    instrumentalTransport->setSource(
        instrumentalReaderSource.get()
    );
    if (loadedSixStemOutput)
    {
        guitarTransport->setSource(guitarReaderSource.get());
        pianoTransport->setSource(pianoReaderSource.get());
    }

    mixerSource.addInputSource(
        vocalsTransport.get(),
        false
    );

    mixerSource.addInputSource(
        drumsTransport.get(),
        false
    );

    mixerSource.addInputSource(
        bassTransport.get(),
        false
    );

    mixerSource.addInputSource(
        instrumentalTransport.get(),
        false
    );
    if (loadedSixStemOutput)
    {
        mixerSource.addInputSource(guitarTransport.get(), false);
        mixerSource.addInputSource(pianoTransport.get(), false);
    }

    return true;
}

// ==============================================================
// UNLOAD STEM AUDIO
// ==============================================================

void MainComponent::unloadStemAudio()
{
    if (playbackTimer)
    playbackTimer->stopTimer();

    isPlaying = false;
    updateTransportButtons(false, false);
    syncOptionalTrackPlayback(false);

    mixerSource.removeAllInputs();

    if (vocalsTransport)
        vocalsTransport->stop();

    if (drumsTransport)
        drumsTransport->stop();

    if (bassTransport)
        bassTransport->stop();

    if (instrumentalTransport)
        instrumentalTransport->stop();
    if (guitarTransport) guitarTransport->stop();
    if (pianoTransport) pianoTransport->stop();

    vocalsTransport.reset();
    drumsTransport.reset();
    bassTransport.reset();
    instrumentalTransport.reset();
    guitarTransport.reset();
    pianoTransport.reset();

    vocalsReaderSource.reset();
    drumsReaderSource.reset();
    bassReaderSource.reset();
    instrumentalReaderSource.reset();
    guitarReaderSource.reset();
    pianoReaderSource.reset();

    playbackPosition = 0.0;
    playbackLength = 0.0;
}

// ==============================================================
// TRACK MIXING
// ==============================================================

void MainComponent::updateTrackMixing()
{
    if (!vocalsTransport
        || !drumsTransport
        || !bassTransport
        || !instrumentalTransport
        || (loadedSixStemOutput && (!guitarTransport || !pianoTransport)))
    {
        return;
    }

    const bool vocalsSolo =
        vocalsTrack.isSoloed();

    const bool drumsSolo =
        drumsTrack.isSoloed();

    const bool bassSolo =
        bassTrack.isSoloed();

    const bool instrumentalSolo =
        instrumentalTrack.isSoloed();
    const bool guitarSolo = loadedSixStemOutput && guitarTrack.isSoloed();
    const bool pianoSolo = loadedSixStemOutput && pianoTrack.isSoloed();

    const bool anySolo =
        vocalsSolo
        || drumsSolo
        || bassSolo
        || instrumentalSolo || guitarSolo || pianoSolo;

    vocalsTransport->setGain((anySolo && !vocalsSolo) || (!anySolo && vocalsTrack.isMuted())
        ? 0.0f : vocalsTrack.getVolume());

    drumsTransport->setGain((anySolo && !drumsSolo) || (!anySolo && drumsTrack.isMuted())
        ? 0.0f : drumsTrack.getVolume());

    bassTransport->setGain((anySolo && !bassSolo) || (!anySolo && bassTrack.isMuted())
        ? 0.0f : bassTrack.getVolume());

    instrumentalTransport->setGain((anySolo && !instrumentalSolo)
        || (!anySolo && instrumentalTrack.isMuted())
        ? 0.0f : instrumentalTrack.getVolume());
    if (loadedSixStemOutput)
    {
        guitarTransport->setGain((anySolo && !guitarSolo) || (!anySolo && guitarTrack.isMuted())
            ? 0.0f : guitarTrack.getVolume());
        pianoTransport->setGain((anySolo && !pianoSolo) || (!anySolo && pianoTrack.isMuted())
            ? 0.0f : pianoTrack.getVolume());
    }
}

// ==============================================================
void MainComponent::syncOptionalTrackPlayback(bool shouldPlay)
{
    if (!loadedSixStemOutput) return;
    if (guitarTransport)
    {
        guitarTransport->setPosition(playbackPosition);
        if (shouldPlay) guitarTransport->start(); else guitarTransport->stop();
    }
    if (pianoTransport)
    {
        pianoTransport->setPosition(playbackPosition);
        if (shouldPlay) pianoTransport->start(); else pianoTransport->stop();
    }
    guitarTrack.setPlayheadPosition(playbackPosition);
    pianoTrack.setPlayheadPosition(playbackPosition);
}

// ==============================================================
// SEEK CALLBACKS
// ==============================================================

void MainComponent::setupSeekCallbacks()
{
    auto setupSeek =
        [this](StemTrackComponent& track)
    {
        track.onSeek =
            [this](double position)
        {
            playbackPosition =
                juce::jlimit(
                    0.0,
                    playbackLength,
                    position
                );

            if (vocalsTransport)
                vocalsTransport->setPosition(
                    playbackPosition
                );

            if (drumsTransport)
                drumsTransport->setPosition(
                    playbackPosition
                );

            if (bassTransport)
                bassTransport->setPosition(
                    playbackPosition
                );

            if (instrumentalTransport)
                instrumentalTransport->setPosition(
                    playbackPosition
                );

            vocalsTrack.setPlayheadPosition(
                playbackPosition
            );

            drumsTrack.setPlayheadPosition(
                playbackPosition
            );

            bassTrack.setPlayheadPosition(
                playbackPosition
            );

            instrumentalTrack.setPlayheadPosition(
                playbackPosition
            );

            syncOptionalTrackPlayback(isPlaying);
            if (isPlaying)
            {
                if (vocalsTransport)
                    vocalsTransport->start();

                if (drumsTransport)
                    drumsTransport->start();

                if (bassTransport)
                    bassTransport->start();

                if (instrumentalTransport)
                    instrumentalTransport->start();
            }

            setStatus(
                "Position: "
                + juce::String(
                    playbackPosition,
                    2
                )
                + " seconds"
            );
        };
    };

    setupSeek(
        vocalsTrack
    );

    setupSeek(
        drumsTrack
    );

    setupSeek(
        bassTrack
    );

    setupSeek(
        instrumentalTrack
    );
    setupSeek(guitarTrack);
    setupSeek(pianoTrack);
}

// ==============================================================
// TRACK CALLBACKS
// ==============================================================

void MainComponent::setupStemTrackCallbacks()
{
    // ==========================================================
    // SOLO SELECTION CALLBACKS
    // ==========================================================

    auto setupSoloSelection =
        [this](StemTrackComponent& track)
    {
        track.onSoloSelectionRequested =
            [this](double start, double end)
        {
            // ==================================================
            // CLEAR SELECTED REGION
            // ==================================================

            if (end <= start)
            {
                playbackSelectionActive = false;

                playbackSelectionStart = 0.0;
                playbackSelectionEnd = 0.0;

                return;
            }

            // ==================================================
            // STORE SELECTED PLAYBACK REGION
            // ==================================================

            playbackSelectionStart =
                juce::jmax(0.0, start);

            playbackSelectionEnd =
                juce::jmax(
                    playbackSelectionStart,
                    end
                );

            playbackSelectionActive =
                playbackSelectionEnd
                > playbackSelectionStart;

            // ==================================================
            // START FROM SELECTION START
            // ==================================================

            playbackPosition =
                playbackSelectionStart;

            // ==================================================
            // MOVE ALL TRANSPORTS
            // ==================================================

            if (vocalsTransport)
                vocalsTransport->setPosition(
                    playbackPosition
                );

            if (drumsTransport)
                drumsTransport->setPosition(
                    playbackPosition
                );

            if (bassTransport)
                bassTransport->setPosition(
                    playbackPosition
                );

            if (instrumentalTransport)
                instrumentalTransport->setPosition(
                    playbackPosition
                );

            // ==================================================
            // UPDATE ALL PLAYHEADS
            // ==================================================

            vocalsTrack.setPlayheadPosition(
                playbackPosition
            );

            drumsTrack.setPlayheadPosition(
                playbackPosition
            );

            bassTrack.setPlayheadPosition(
                playbackPosition
            );

            instrumentalTrack.setPlayheadPosition(
                playbackPosition
            );

            // ==================================================
            // START PLAYBACK
            // ==================================================

            startPlayback();

            setStatus(
                "Playing selected region: "
                + juce::String(
                    playbackSelectionStart,
                    2
                )
                + " - "
                + juce::String(
                    playbackSelectionEnd,
                    2
                )
                + " seconds"
            );
        };
    };

    setupSoloSelection(vocalsTrack);
    setupSoloSelection(drumsTrack);
    setupSoloSelection(bassTrack);
    setupSoloSelection(instrumentalTrack);
    setupSoloSelection(guitarTrack);
    setupSoloSelection(pianoTrack);

    auto bindPlaybackRegion = [this](StemTrackComponent& track)
    {
        auto* selectedTrack = &track;
        track.onSelectionChanged = [this, selectedTrack]
        {
            auto& track = *selectedTrack;
            if (!track.hasSelection())
            {
                playbackSelectionActive = false;
                playbackSelectionStart = playbackSelectionEnd = 0.0;
                return;
            }

            if (isPlaying)
                pausePlayback();
            playbackSelectionStart = juce::jlimit(0.0, playbackLength, track.getSelectionStart());
            playbackSelectionEnd = juce::jlimit(playbackSelectionStart, playbackLength,
                                                 track.getSelectionEnd());
            playbackSelectionActive = playbackSelectionEnd > playbackSelectionStart;
            if (playbackSelectionActive)
            {
                playbackPosition = playbackSelectionStart;
                if (vocalsTransport) vocalsTransport->setPosition(playbackPosition);
                if (drumsTransport) drumsTransport->setPosition(playbackPosition);
                if (bassTransport) bassTransport->setPosition(playbackPosition);
                if (instrumentalTransport) instrumentalTransport->setPosition(playbackPosition);
                syncOptionalTrackPlayback(false);
                vocalsTrack.setPlayheadPosition(playbackPosition);
                drumsTrack.setPlayheadPosition(playbackPosition);
                bassTrack.setPlayheadPosition(playbackPosition);
                instrumentalTrack.setPlayheadPosition(playbackPosition);
                guitarTrack.setPlayheadPosition(playbackPosition);
                pianoTrack.setPlayheadPosition(playbackPosition);
            }
        };
    };
    bindPlaybackRegion(vocalsTrack);
    bindPlaybackRegion(drumsTrack);
    bindPlaybackRegion(bassTrack);
    bindPlaybackRegion(guitarTrack);
    bindPlaybackRegion(pianoTrack);
    bindPlaybackRegion(instrumentalTrack);


    // ==========================================================
    // MIXING CALLBACKS
    // ==========================================================

    vocalsTrack.onMixingChanged =
        [this]
    {
        updateTrackMixing();
    };

    drumsTrack.onMixingChanged =
        [this]
    {
        updateTrackMixing();
    };

    bassTrack.onMixingChanged =
        [this]
    {
        updateTrackMixing();
    };

    instrumentalTrack.onMixingChanged =
        [this]
    {
        updateTrackMixing();
    };
    guitarTrack.onMixingChanged = [this] { updateTrackMixing(); };
    pianoTrack.onMixingChanged = [this] { updateTrackMixing(); };
    guitarTrack.onTrackStateChanged = [this] { updateTrackMixing(); };
    pianoTrack.onTrackStateChanged = [this] { updateTrackMixing(); };


    // ==========================================================
    // TRACK STATE CALLBACKS
    // ==========================================================

    vocalsTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };

    drumsTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };

    bassTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };

    instrumentalTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };

    // ==========================================================
    // EXPORT SELECTION CALLBACKS
    // ==========================================================

    vocalsTrack.onExportSelectionRequested =
        [this](double start, double end)
    {
        playbackLength = vocalsTrack.getAudioLength();

        playbackSelectionStart =
            juce::jlimit(0.0, playbackLength, start);

        playbackSelectionEnd =
            juce::jlimit(0.0, playbackLength, end);

        playbackSelectionActive = true;

        exportSelectedAudio();
    };

    drumsTrack.onExportSelectionRequested =
        [this](double start, double end)
    {
        playbackLength = drumsTrack.getAudioLength();

        playbackSelectionStart =
            juce::jlimit(0.0, playbackLength, start);

        playbackSelectionEnd =
            juce::jlimit(0.0, playbackLength, end);

        playbackSelectionActive = true;

        exportSelectedAudio();
    };

    bassTrack.onExportSelectionRequested =
        [this](double start, double end)
    {
        playbackLength = bassTrack.getAudioLength();

        playbackSelectionStart =
            juce::jlimit(0.0, playbackLength, start);

        playbackSelectionEnd =
            juce::jlimit(0.0, playbackLength, end);

        playbackSelectionActive = true;

        exportSelectedAudio();
    };

    instrumentalTrack.onExportSelectionRequested =
        [this](double start, double end)
    {
        playbackLength = instrumentalTrack.getAudioLength();

        playbackSelectionStart =
            juce::jlimit(0.0, playbackLength, start);

        playbackSelectionEnd =
            juce::jlimit(0.0, playbackLength, end);

        playbackSelectionActive = true;

        exportSelectedAudio();
    };

    auto setExportRegion = [this](double start, double end, StemTrackComponent& track)
    {
        playbackLength = track.getAudioLength();
        playbackSelectionStart = juce::jlimit(0.0, playbackLength, start);
        playbackSelectionEnd = juce::jlimit(0.0, playbackLength, end);
        playbackSelectionActive = true;
        exportSelectedAudio();
    };
    guitarTrack.onExportSelectionRequested = [setExportRegion, this](double a, double b) { setExportRegion(a, b, guitarTrack); };
    pianoTrack.onExportSelectionRequested = [setExportRegion, this](double a, double b) { setExportRegion(a, b, pianoTrack); };
}

// ==============================================================
// CHECK FOR UPDATES
// ==============================================================

// ============================================================
// CHECK FOR UPDATES
// ============================================================

void MainComponent::checkForUpdates()
{
    const juce::String currentVersion =
        juce::String(OfforStemSplitter::VERSION);

    updateChecker.checkForUpdate(
        currentVersion,
        [this](const UpdateChecker::UpdateInfo& info)
        {
            handleUpdateResult(info);
        }
    );
}


// ==============================================================
// HANDLE UPDATE RESULT
// ==============================================================

void MainComponent::handleUpdateResult(
    const UpdateChecker::UpdateInfo& info
)
{
    // ==========================================================
    // NO UPDATE
    // ==========================================================

    if (!info.updateAvailable)
    {
        // setStatus(
        //     "Offor Stem Splitter is up to date."
        // );

        return;
    }

    // ==========================================================
    // BUILD MESSAGE
    // ==========================================================

    juce::String message;

    message << "A new version of Offor Stem Splitter is available.\n\n";

    message << "Current version: "
            << info.currentVersion
            << "\n";

    message << "Latest version: "
            << info.latestVersion
            << "\n\n";

    // ==========================================================
    // RELEASE NOTES
    // ==========================================================

    if (!info.releaseNotes.isEmpty())
    {
        message << "What's new:\n";

        for (const auto& note : info.releaseNotes)
        {
            message << "• "
                    << note
                    << "\n";
        }

        message << "\n";
    }

    // ==========================================================
    // DOWNLOAD URL
    // ==========================================================

    const auto downloadUrl =
        info.downloadUrl;

    // ==========================================================
    // CREATE ALERT
    // ==========================================================

    auto* alert =
        new juce::AlertWindow(
            "Update Available",
            message,
            juce::MessageBoxIconType::InfoIcon
        );

  // ==========================================================
    // BUTTONS
    // ==========================================================

    alert->addButton(
        "Download",
        1,
        juce::KeyPress(
            juce::KeyPress::returnKey
        ),
        juce::KeyPress()
    );

    alert->addButton(
        "Later",
        0,
        juce::KeyPress(
            juce::KeyPress::escapeKey
        ),
        juce::KeyPress()
    );

    // ==========================================================
    // CALLBACK
    // ==========================================================

    alert->enterModalState(
        true,
        juce::ModalCallbackFunction::create(
            [downloadUrl](int result)
            {
                if (result == 1
                    && downloadUrl.isNotEmpty())
                {
                    juce::URL(
                        downloadUrl
                    ).launchInDefaultBrowser();
                }
            }
        ),
        true
    );
}

// ==============================================================
// FILE PICKER
// ==============================================================

void MainComponent::selectAudioFile()
{
    auto chooser =
        std::make_shared<
            juce::FileChooser
        >(
            "Select audio file...",
            juce::File{},
            "*.mp3;*.wav;*.flac;*.aiff;*.aif"
        );

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode
        |
        juce::FileBrowserComponent::canSelectFiles,

        [this, chooser]
        (const juce::FileChooser&)
        {
            auto file =
                chooser->getResult();

            if (!file.existsAsFile())
                return;

            loadSelectedFile(
                file
            );
        }
    );
}

// ==============================================================
// LOAD SELECTED FILE
// ==============================================================

void MainComponent::loadSelectedFile(
    const juce::File& file
)
{
    stopPlayback();

    if (separatorProcess != nullptr)
    {
        if (separatorProcess->isRunning())
        {
            separatorProcess->kill();
        }

        separatorProcess.reset();
    }

    selectedFile = file;

    fileLabel.setText(
        file.getFileName(),
        juce::dontSendNotification
    );

    progressLabel.setVisible(
        true
    );

    progressLabel.setText(
        "",
        juce::dontSendNotification
    );

    setStatus(
        "Audio selected. Starting separation..."
    );

    clearSeparatedStems();

    shrinkToInputWindow();

    resized();

    grabKeyboardFocus();

    // ==========================================================
    // Automatically start separation
    // ==========================================================

    checkLicenseBeforeSeparation();
}


// ==========================================================
// INSTALLATION ID
// ==========================================================

juce::String MainComponent::getInstallationId()
{
    return installationId;
}


// ==========================================================
// LOAD OR CREATE INSTALLATION ID
// ==========================================================

void MainComponent::loadOrCreateInstallationId()
{
    // ------------------------------------------------------
    // Application data folder
    // ------------------------------------------------------

    auto appDataFolder =
        juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory
        )
        .getChildFile("Offor")
        .getChildFile("StemSplitter");


    // ------------------------------------------------------
    // Create folder if it doesn't exist
    // ------------------------------------------------------

    if (!appDataFolder.exists())
    {
        appDataFolder.createDirectory();
    }


    // ------------------------------------------------------
    // Installation ID file
    // ------------------------------------------------------

    auto installationFile =
        appDataFolder.getChildFile(
            "installation.id"
        );


    // ------------------------------------------------------
    // Load existing Installation ID
    // ------------------------------------------------------

    if (installationFile.existsAsFile())
    {
        installationId =
            installationFile.loadFileAsString().trim();

        if (installationId.isNotEmpty())
            return;
    }


    // ------------------------------------------------------
    // Create a new Installation ID
    // ------------------------------------------------------

    juce::Uuid uuid;

    installationId = uuid.toString();


    // ------------------------------------------------------
    // Save Installation ID
    // ------------------------------------------------------

    installationFile.replaceWithText(
        installationId
    );
}

// ==============================================================
// START DEMUCS OUTPUT THREAD
// ==============================================================

void MainComponent::startSeparatorOutputThread()
{
    stopSeparatorOutputThread();

    separatorOutputThread =
        std::make_unique<
            SeparatorOutputThread
        >(
            *this
        );

    separatorOutputThread->startThread();
}

// ==============================================================
// STOP DEMUCS OUTPUT THREAD
// ==============================================================

void MainComponent::stopSeparatorOutputThread()
{
    if (separatorOutputThread != nullptr)
    {
        const bool stopped =
            separatorOutputThread->stopThread(5000);

        if (!stopped)
        {
            // The thread is still running.
            // Do NOT destroy anything it may still be using.
            return;
        }

        separatorOutputThread.reset();
    }
}

// ==============================================================
// RUN SEPARATOR
// ==============================================================

void MainComponent::runSeparator()
{

    std::cout
    << "\n========================================\n"
    << "        RUN SEPARATOR DEBUG\n"
    << "========================================\n"
    << "Selected file:\n"
    << selectedFile.getFullPathName().toStdString()
    << "\n"
    << "Selected filename:\n"
    << selectedFile.getFileName().toStdString()
    << "\n"
    << "Filename without extension:\n"
    << selectedFile.getFileNameWithoutExtension().toStdString()
    << "\n"
    << "========================================\n"
    << std::endl;
    
    if (!selectedFile.existsAsFile())
    {
        setStatus(
            "Please select an audio file first."
        );

        return;
    }

    if (separatorProcess != nullptr)
    {
        if (separatorProcess->isRunning())
        {
            setStatus(
                "Offor is already splitting..."
            );

            return;
        }

        separatorProcess.reset();
    }

    // ==========================================================
    // OFFOR STEM SPLITTER APPLICATION DIRECTORY
    // ==========================================================

    const juce::File appDirectory =
        juce::File::getSpecialLocation(
            juce::File::currentExecutableFile
        ).getParentDirectory();


    // ==========================================================
    // OFFOR AI PYTHON
    // ==========================================================

    const juce::File pythonExecutable =
        appDirectory
            .getChildFile("demucs-runtime")
            .getChildFile("python.exe");


    // ==========================================================
    // OFFOR SEPARATOR SCRIPT
    // ==========================================================

    const juce::File separatorScript =
        appDirectory
            .getChildFile("demucs-engine")
            .getChildFile("separator.py");


    if (!pythonExecutable.existsAsFile())
    {
        setStatus(
            "ERROR: Bundled Python runtime not found."
        );

        return;
    }

    if (!separatorScript.existsAsFile())
    {
        setStatus(
            "ERROR: separator.py not found."
        );

        return;
    }


    // ==========================================================
    // EXPECTED OUTPUT FOLDER
    // ==========================================================

    const juce::File documentsDirectory =
        juce::File::getSpecialLocation(
            juce::File::userDocumentsDirectory
        );

    outputFolder =
        documentsDirectory
            .getChildFile("Offor Stem Splitter")
            .getChildFile("Exports")
            .getChildFile(
                selectedFile.getFileNameWithoutExtension()
            );

    // ==========================================================
    // RESET SEPARATOR STATE
    // ==========================================================

    separationProgress = 0.0;
    progressLabel.setText("0%", juce::dontSendNotification);
    progressLabel.setVisible(true);

    separationRunning = true;

    progressAnimationFrame = 0;

    if (progressAnimationTimer != nullptr)
        progressAnimationTimer->startTimer(40);

    cancelButton.setVisible(
        true
    );

    separatorOutputBuffer.clear();
    pendingOutputLine.clear();

    // ==========================================================
    // BUILD COMMAND
    // ==========================================================

    juce::StringArray command;

    command.add(
        pythonExecutable.getFullPathName()
    );

    command.add(
        "-u"
    );

    command.add(
        separatorScript.getFullPathName()
    );

    command.add(
        selectedFile.getFullPathName()
    );

    command.add(sixStemModeEnabled ? "htdemucs_6s" : "htdemucs");

    separatorProgressFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
        .getChildFile("offor_stem_progress_" + juce::String(juce::Random::getSystemRandom().nextInt64()) + ".txt");
    separatorProgressFile.deleteFile();
    command.add(separatorProgressFile.getFullPathName());

    // ==========================================================
    // CREATE PROCESS
    // ==========================================================

    separatorProcess =
        std::make_unique<
            juce::ChildProcess
        >();

    {
        const juce::ScopedLock lock(
            separatorOutputLock
        );

        separatorOutputBuffer.clear();
        pendingOutputLine.clear();
    }

    // ==========================================================
    // INITIAL UI
    // ==========================================================

    progressLabel.setVisible(
        true
    );

    progressLabel.setText(
        "Relax, Offor is splitting it...",
        juce::dontSendNotification
    );

    setStatus(
        // "Working...",
        "This may take a few minutes depending on the length of the track."
    );

    // ==========================================================
    // START PROCESS
    // ==========================================================

    const bool started =
        separatorProcess->start(
            command,
            juce::ChildProcess::wantStdOut
            |
            juce::ChildProcess::wantStdErr
        );

    std::cout
    << "\n========================================\n"
    << "        PYTHON PROCESS STARTED\n"
    << "========================================\n"
    << "Python:\n"
    << pythonExecutable.getFullPathName().toStdString()
    << "\n"
    << "Separator:\n"
    << separatorScript.getFullPathName().toStdString()
    << "\n"
    << "Expected output folder:\n"
    << outputFolder.getFullPathName().toStdString()
    << "\n"
    << "Process running: "
    << (separatorProcess->isRunning() ? "YES" : "NO")
    << "\n"
    << "========================================\n"
    << std::endl;

    std::cout
    << "Waiting for separator output...\n"
    << std::endl;

    if (!started)
    {

        std::cout
    << "\n========== PYTHON PROCESS STARTED ==========\n"
    << "Python:\n"
    << pythonExecutable.getFullPathName().toStdString()
    << "\n"
    << "Separator:\n"
    << separatorScript.getFullPathName().toStdString()
    << "\n"
    << "Expected output folder:\n"
    << outputFolder.getFullPathName().toStdString()
    << "\n"
    << "============================================\n"
    << std::endl;
    
        separatorProcess.reset();

        separationRunning = false;

        if (progressAnimationTimer != nullptr)
            progressAnimationTimer->stopTimer();

        cancelButton.setVisible(
            false
        );

        setStatus(
            "ERROR: Could not start Python."
        );

        progressLabel.setText(
            "",
            juce::dontSendNotification
        );

        return;
    }

    

    startSeparatorOutputThread();

    
    startTimer(100);

   
    grabKeyboardFocus();
}


// ==========================================================
// PARSE SEPARATOR PROGRESS
// ==========================================================

void MainComponent::parseSeparatorProgress(
    const juce::String& output
)
{
    // ======================================================
    // COMBINE PREVIOUS INCOMPLETE LINE WITH NEW OUTPUT
    // ======================================================

    juce::String combined =
        pendingOutputLine + output;

    pendingOutputLine.clear();

    int searchStart = 0;

    const int len =
        combined.length();

    // ======================================================
    // PROCESS COMPLETE LINES
    // ======================================================

    while (searchStart < len)
    {
        const int newlineIndex =
            combined.indexOfAnyOf(
                "\r\n",
                searchStart,
                false
            );

        // ==================================================
        // INCOMPLETE LINE
        // ==================================================

        if (newlineIndex < 0)
        {
            pendingOutputLine =
                combined.substring(
                    searchStart
                );

            break;
        }

        // ==================================================
        // EXTRACT COMPLETE LINE
        // ==================================================

        auto trimmed =
            combined.substring(
                searchStart,
                newlineIndex
            ).trim();

        // ==================================================
        // PROGRESS
        // ==================================================

        int progress = -1;
        if (trimmed.startsWith("OFFOR_PROGRESS:"))
        {
            progress = trimmed.fromFirstOccurrenceOf("OFFOR_PROGRESS:", false, false).trim().getIntValue();
        }
        else
        {
            const int percentPosition = trimmed.lastIndexOfChar('%');
            if (percentPosition > 0)
            {
                int numberStart = percentPosition - 1;
                while (numberStart >= 0
                       && ((trimmed[numberStart] >= '0' && trimmed[numberStart] <= '9')
                           || trimmed[numberStart] == '.'))
                    --numberStart;

                ++numberStart;
                if (numberStart < percentPosition)
                {
                    const auto numberText = trimmed.substring(numberStart, percentPosition);
                    progress = juce::roundToInt(numberText.getDoubleValue());
                }
            }
        }

        if (progress >= 0)
        {
            progress = juce::jlimit(0, 100, progress);
            separationProgress = progress / 100.0;

            juce::Component::SafePointer<MainComponent> safeThis(this);
            juce::MessageManager::callAsync(
                [safeThis, progress]()
                {
                    if (safeThis != nullptr && safeThis->separationRunning)
                    {
                        safeThis->progressLabel.setVisible(true);
                        safeThis->progressLabel.setText(juce::String(progress) + "%",
                                                        juce::dontSendNotification);
                        safeThis->progressLabel.repaint();
                        safeThis->repaint();
                    }
                }
            );
        }

        // ==================================================
        // OUTPUT FOLDER
        // ==================================================

        else if (trimmed.startsWith(
                     "OFFOR_OUTPUT:"))
        {
            const auto path =
                trimmed.fromFirstOccurrenceOf(
                    "OFFOR_OUTPUT:",
                    false,
                    false
                ).trim();

            if (path.isNotEmpty())
            {
                outputFolder =
                    juce::File(path);

                std::cout
                    << "\n========== OFFOR OUTPUT ==========\n"
                    << "Path: "
                    << outputFolder
                           .getFullPathName()
                           .toStdString()
                    << "\nIs directory: "
                    << (
                        outputFolder.isDirectory()
                            ? "YES"
                            : "NO"
                    )
                    << "\n==================================\n"
                    << std::endl;
            }
        }

        // ==================================================
        // STEMS READY
        // ==================================================

        else if (trimmed == "OFFOR_STEMS_READY")
        {
            std::cout
                << "\n========================================\n"
                << "       OFFOR_STEMS_READY RECEIVED\n"
                << "========================================\n"
                << "Demucs completed successfully.\n"
                << "========================================\n"
                << std::endl;
        }

        // ==================================================
        // MOVE TO NEXT LINE
        // ==================================================

        int next =
            newlineIndex + 1;

        // Handle CRLF as one terminator
        if (combined[newlineIndex] == '\r'
            && next < len
            && combined[next] == '\n')
        {
            ++next;
        }

        searchStart =
            next;
    }
}

// ==============================================================
// SEPARATION ANIMATION
// ==============================================================

void MainComponent::updateSeparationAnimation()
{
    if (!separationRunning)
        return;

    ++progressAnimationFrame;

    if (separatorProgressFile.existsAsFile())
    {
        const auto progressText = separatorProgressFile.loadFileAsString().trim();
        if (progressText.isNotEmpty())
        {
            const int reportedProgress = juce::jlimit(0, 100, progressText.getIntValue());
            separationProgress.store(reportedProgress / 100.0);
        }
    }

    const int progress = juce::jlimit(0, 100,
        juce::roundToInt(separationProgress.load() * 100.0));
    progressLabel.setText(juce::String(progress) + "%", juce::dontSendNotification);
    repaint();
}

// ==============================================================
// TIMER CALLBACK
// ==============================================================

void MainComponent::timerCallback()
{
    // ==========================================================
    // CHECK SEPARATOR PROCESS
    // ==========================================================

    if (separatorProcess == nullptr)
        return;

    if (separatorProcess->isRunning())
        return;

    // ==========================================================
    // PROCESS HAS FINISHED
    // ==========================================================

    stopTimer();

    // Stop the output reader first.
    stopSeparatorOutputThread();
    separatorProgressFile.deleteFile();

    // ==========================================================
    // CHECK WHETHER SEPARATION WAS CANCELLED
    // ==========================================================

    if (!separationRunning)
    {
        separatorProcess.reset();
        return;
    }

    // ==========================================================
    // GET EXIT CODE
    // ==========================================================

    const int exitCode =
        separatorProcess->getExitCode();

    std::cout
        << "\n========================================\n"
        << "     SEPARATOR PROCESS FINISHED\n"
        << "========================================\n"
        << "Exit code: "
        << exitCode
        << "\n"
        << "========================================\n"
        << std::endl;

    // ==========================================================
    // GET DOCUMENTS FOLDER
    // ==========================================================

    const juce::File documentsDirectory =
        juce::File::getSpecialLocation(
            juce::File::userDocumentsDirectory
        );

    // ==========================================================
    // CREATE OFFOR LOG FOLDER
    // ==========================================================

    const juce::File logFolder =
        documentsDirectory
            .getChildFile("Offor Stem Splitter");

    logFolder.createDirectory();

    // ==========================================================
    // DEBUG LOG FILE
    // ==========================================================

    const juce::File logFile =
        logFolder.getChildFile(
            "Offor-Debug-Log.txt"
        );

    // ==========================================================
    // COPY COMPLETE PYTHON OUTPUT
    // ==========================================================

    juce::String completeOutput;

    {
        const juce::ScopedLock lock(
            separatorOutputLock
        );

        completeOutput =
            separatorOutputBuffer;
    }

    // ==========================================================
    // WRITE DEBUG LOG
    // ==========================================================

    juce::String logText;

    logText +=
        "\n\n========================================\n";
    logText +=
        "       OFFOR STEM SPLITTER DEBUG\n";
    logText +=
        "========================================\n\n";

    logText +=
        "Input file:\n";
    logText +=
        selectedFile.getFullPathName();
    logText +=
        "\n\n";

    logText +=
        "Output folder:\n";
    logText +=
        outputFolder.getFullPathName();
    logText +=
        "\n\n";

    logText +=
        "----------------------------------------\n";
    logText +=
        "PYTHON / DEMUCS OUTPUT\n";
    logText +=
        "----------------------------------------\n";

    logText += completeOutput;

    logText +=
        "\n\n----------------------------------------\n";
    logText +=
        "PROCESS RESULT\n";
    logText +=
        "----------------------------------------\n";

    logText +=
        "Exit code: ";
    logText +=
        juce::String(exitCode);

    logText +=
        "\n========================================\n";

    logFile.appendText(
        logText,
        false,
        false,
        "\n"
    );

    // ==========================================================
    // SEPARATION FAILED
    // ==========================================================

    if (exitCode != 0)
    {
        separationRunning = false;

        if (progressAnimationTimer != nullptr)
            progressAnimationTimer->stopTimer();

        cancelButton.setVisible(false);

        progressLabel.setVisible(true);

        progressLabel.setText(
            "Separation failed",
            juce::dontSendNotification
        );

        setStatus(
            "Offor could not separate this track. "
            "Please check the debug log."
        );

        repaint();

        separatorProcess.reset();

        return;
    }

    // ==========================================================
    // SEPARATION SUCCEEDED
    // ==========================================================

    separationRunning = false;

    if (progressAnimationTimer != nullptr)
        progressAnimationTimer->stopTimer();

    cancelButton.setVisible(false);

    separationProgress = 1.0;

    progressLabel.setVisible(true);

    progressLabel.setText(
        "100%",
        juce::dontSendNotification
    );

    repaint();

    // ==========================================================
    // SHOW SEPARATED STEMS
    // ==========================================================

    showSeparatedStems();

    // ==========================================================
    // CLEAN UP
    // ==========================================================

    separatorProcess.reset();
}

// ==============================================================
// KEYBOARD
// ==============================================================

bool MainComponent::keyPressed(
    const juce::KeyPress& key,
    juce::Component*
)
{
    // ==========================================================
    // SPACE = PLAY / STOP
    // ==========================================================

    if (key.getKeyCode() == juce::KeyPress::spaceKey)
    {
        if (isPlaying)
            stopPlayback();
        else
            startPlayback();

        return true;
    }

    // ==========================================================
    // P = PAUSE
    // ==========================================================

    if (key.getKeyCode() == 'p'
        || key.getKeyCode() == 'P')
    {
        pausePlayback();

        return true;
    }

    // ==========================================================
    // ESCAPE = CANCEL / STOP
    // ==========================================================

    if (key.getKeyCode() == juce::KeyPress::escapeKey)
    {
        if (separationRunning)
            cancelSeparator();
        else
            stopPlayback();

        return true;
    }

    return false;
}

// ==============================================================
// FILE DRAG
// ==============================================================

bool MainComponent::isInterestedInFileDrag(
    const juce::StringArray& files
)
{
    // ==========================================================
    // Once stems are already loaded, the main drop box should
    // no longer accept dragged audio files.
    //
    // Clicking the box will still open the file browser.
    // Dragging stems to a DAW remains unaffected.
    // ==========================================================

    if (separationRunning || vocalsTrack.isVisible())
        return false;

    for (const auto& path : files)
    {
        juce::File file(path);

        if (file.hasFileExtension(
                ".wav;.mp3;.flac;.aiff;.aif"))
        {
            return true;
        }
    }

    return false;
}



void MainComponent::fileDragEnter(
    const juce::StringArray& files,
    int,
    int
)
{
    isDragOver =
        isInterestedInFileDrag(files);

    repaint();
}


void MainComponent::fileDragExit(
    const juce::StringArray&
)
{
    isDragOver = false;
    repaint();
}


void MainComponent::filesDropped(
    const juce::StringArray& files,
    int,
    int
)
{
    isDragOver = false;
    repaint();

    // ==========================================================
    // Do not accept another dragged audio file once stems
    // are already displayed.
    //
    // The user must CLICK the drop zone to choose another file.
    // ==========================================================

    if (separationRunning || vocalsTrack.isVisible())
        return;

    if (files.isEmpty())
        return;

    juce::File file(files[0]);

    if (!file.existsAsFile())
        return;

    loadSelectedFile(file);
}


// ==============================================================
// CANCEL SEPARATOR
// ==============================================================

void MainComponent::cancelSeparator()
{
    if (!separationRunning)
        return;

    separationRunning = false;

    // ==========================================================
    // STOP SEPARATION ANIMATION
    // ==========================================================

    if (progressAnimationTimer != nullptr)
        progressAnimationTimer->stopTimer();

    progressAnimationFrame = 0;

    // ==========================================================
    // STOP SEPARATOR PROCESS
    // ==========================================================

    if (separatorProcess != nullptr)
    {
        separatorProcess->kill();
    }

    // Stop the reader thread FIRST.
    stopSeparatorOutputThread();

    // Only destroy the process after the reader thread
    // has completely stopped.
    if (separatorOutputThread == nullptr)
    {
        separatorProcess.reset();
    }

    // ==========================================================
    // RESET UI
    // ==========================================================

    cancelButton.setVisible(false);

    progressLabel.setVisible(false);

    progressLabel.setText(
        "",
        juce::dontSendNotification
    );

    setStatus(
        "Separation cancelled."
    );

    stopTimer();

    resized();
    repaint();
}


// ==============================================================
// START PLAYBACK
// ==============================================================

void MainComponent::startPlayback()
{
    if (!vocalsTransport
        || !drumsTransport
        || !bassTransport
        || !instrumentalTransport)
    {
        return;
    }

    if (playbackLength <= 0.0)
        return;

    if (playbackSelectionActive)
    {
        if (playbackPosition < playbackSelectionStart || playbackPosition >= playbackSelectionEnd)
            playbackPosition = playbackSelectionStart;
    }
    else if (playbackPosition >= playbackLength)
    {
        playbackPosition = 0.0;
    }

    vocalsTransport->setPosition(playbackPosition);
    drumsTransport->setPosition(playbackPosition);
    bassTransport->setPosition(playbackPosition);
    instrumentalTransport->setPosition(playbackPosition);

    vocalsTransport->start();
    drumsTransport->start();
    bassTransport->start();
    instrumentalTransport->start();

    isPlaying = true;
    updateTransportButtons(true, false);
    syncOptionalTrackPlayback(true);

    playbackTimer->startTimer(30);

    setStatus("Playing...");
}

// ==============================================================
// PAUSE PLAYBACK
// ==============================================================

void MainComponent::pausePlayback()
{
    // ==========================================================
    // Capture the current playback position
    // ==========================================================

    if (vocalsTransport)
        playbackPosition = vocalsTransport->getCurrentPosition();

    // ==========================================================
    // Stop all transports
    // ==========================================================

    if (vocalsTransport)
        vocalsTransport->stop();

    if (drumsTransport)
        drumsTransport->stop();

    if (bassTransport)
        bassTransport->stop();

    if (instrumentalTransport)
        instrumentalTransport->stop();

    // ==========================================================
    // Playback state
    // ==========================================================

    isPlaying = false;
    updateTransportButtons(false, true);
    syncOptionalTrackPlayback(false);

    // ==========================================================
    // Keep waveform playheads synchronized
    // ==========================================================

    vocalsTrack.setPlayheadPosition(playbackPosition);
    drumsTrack.setPlayheadPosition(playbackPosition);
    bassTrack.setPlayheadPosition(playbackPosition);
    instrumentalTrack.setPlayheadPosition(playbackPosition);
    guitarTrack.setPlayheadPosition(playbackPosition);
    pianoTrack.setPlayheadPosition(playbackPosition);

    setStatus("Paused.");
}


// ==============================================================
// STOP PLAYBACK
// ==============================================================

void MainComponent::stopPlayback()
{
    if (playbackTimer)
        playbackTimer->stopTimer();

    if (vocalsTransport)
        vocalsTransport->stop();

    if (drumsTransport)
        drumsTransport->stop();

    if (bassTransport)
        bassTransport->stop();

    if (instrumentalTransport)
        instrumentalTransport->stop();

    // ==========================================================
    // Rewind to the active highlighted range, or the song start.
    playbackPosition = playbackSelectionActive ? playbackSelectionStart : 0.0;
    if (vocalsTransport) vocalsTransport->setPosition(playbackPosition);
    if (drumsTransport) drumsTransport->setPosition(playbackPosition);
    if (bassTransport) bassTransport->setPosition(playbackPosition);
    if (instrumentalTransport) instrumentalTransport->setPosition(playbackPosition);

    isPlaying = false;
    updateTransportButtons(false, false);
    syncOptionalTrackPlayback(false);

    vocalsTrack.setPlayheadPosition(playbackPosition);
    drumsTrack.setPlayheadPosition(playbackPosition);
    bassTrack.setPlayheadPosition(playbackPosition);
    instrumentalTrack.setPlayheadPosition(playbackPosition);

    setStatus("Stopped.");

    repaint();
}



// ==============================================================
// UPDATE PLAYBACK
// ==============================================================

void MainComponent::updateTransportButtons(bool playing, bool paused)
{
    playButton.setToggleState(playing, juce::dontSendNotification);
    pauseButton.setToggleState(paused, juce::dontSendNotification);
    stopButton.setToggleState(!playing && !paused, juce::dontSendNotification);
}
void MainComponent::updatePlayback()
{
    if (!isPlaying)
        return;

    if (!vocalsTransport)
        return;

    // ==========================================================
    // GET CURRENT POSITION
    // ==========================================================

    playbackPosition =
        vocalsTransport->getCurrentPosition();

    // ==========================================================
    // SELECTION PLAYBACK
    // ==========================================================

    if (playbackSelectionActive)
    {
        if (playbackPosition >= playbackSelectionEnd)
        {
            // ==================================================
            // LOOP SELECTION
            // ==================================================

            if (loopSelectionEnabled)
            {
                playbackPosition =
                    playbackSelectionStart;

                if (vocalsTransport)
                    vocalsTransport->setPosition(
                        playbackPosition
                    );

                if (drumsTransport)
                    drumsTransport->setPosition(
                        playbackPosition
                    );

                if (bassTransport)
                    bassTransport->setPosition(
                        playbackPosition
                    );

                if (instrumentalTransport)
                    instrumentalTransport->setPosition(
                        playbackPosition
                    );

                syncOptionalTrackPlayback(true);

                vocalsTrack.setPlayheadPosition(
                    playbackPosition
                );

                drumsTrack.setPlayheadPosition(
                    playbackPosition
                );

                bassTrack.setPlayheadPosition(
                    playbackPosition
                );

                instrumentalTrack.setPlayheadPosition(
                    playbackPosition
                );
            guitarTrack.setPlayheadPosition(playbackPosition);
            pianoTrack.setPlayheadPosition(playbackPosition);

                return;
            }

            // ==================================================
            // NORMAL SELECTION PLAYBACK
            // ==================================================

            stopPlayback();

            playbackPosition =
                playbackSelectionStart;

            vocalsTrack.setPlayheadPosition(
                playbackPosition
            );

            drumsTrack.setPlayheadPosition(
                playbackPosition
            );

            bassTrack.setPlayheadPosition(
                playbackPosition
            );

            instrumentalTrack.setPlayheadPosition(
                playbackPosition
            );
            guitarTrack.setPlayheadPosition(playbackPosition);
            pianoTrack.setPlayheadPosition(playbackPosition);

            return;
        }
    }

    // ==========================================================
    // NORMAL FULL-TRACK PLAYBACK
    // ==========================================================

    if (playbackPosition >= playbackLength
        || !vocalsTransport->isPlaying())
    {
        stopPlayback();

        playbackPosition = 0.0;

        vocalsTrack.setPlayheadPosition(
            playbackPosition
        );

        drumsTrack.setPlayheadPosition(
            playbackPosition
        );

        bassTrack.setPlayheadPosition(
            playbackPosition
        );

        instrumentalTrack.setPlayheadPosition(
            playbackPosition
        );
            guitarTrack.setPlayheadPosition(playbackPosition);
            pianoTrack.setPlayheadPosition(playbackPosition);

        return;
    }

    // ==========================================================
    // UPDATE PLAYHEADS
    // ==========================================================

    vocalsTrack.setPlayheadPosition(
        playbackPosition
    );

    drumsTrack.setPlayheadPosition(
        playbackPosition
    );

    bassTrack.setPlayheadPosition(
        playbackPosition
    );

    instrumentalTrack.setPlayheadPosition(
        playbackPosition
    );
            guitarTrack.setPlayheadPosition(playbackPosition);
            pianoTrack.setPlayheadPosition(playbackPosition);
}


// ==============================================================
// EXPAND WINDOW FOR TRACKS
// ==============================================================

void MainComponent::expandForTracks()
{
    setSize(1200, 700);

    resized();
}


// ==============================================================
// SHRINK WINDOW FOR INPUT
// ==============================================================

void MainComponent::shrinkToInputWindow()
{
    setSize(1200, 700);

    resized();
}



// ==============================================================
// WRITE AUDIO FILE
// ==============================================================

bool MainComponent::writeAudioFile(
    const juce::File& sourceFile,
    const juce::File& destination,
    double startTime,
    double endTime,
    bool normalize
)
{
    // ==========================================================
    // VALIDATE SOURCE
    // ==========================================================

    if (!sourceFile.existsAsFile())
        return false;

    // ==========================================================
    // OPEN SOURCE FILE
    // ==========================================================

    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(sourceFile)
    );

    if (reader == nullptr)
        return false;

    const double sampleRate =
        reader->sampleRate;

    const int numChannels =
        static_cast<int>(reader->numChannels);

    const juce::int64 totalSamples =
        reader->lengthInSamples;

    if (sampleRate <= 0.0
        || numChannels <= 0
        || totalSamples <= 0)
    {
        return false;
    }

    // ==========================================================
    // CONVERT TIME TO SAMPLES
    // ==========================================================

    juce::int64 startSample =
        static_cast<juce::int64>(
            std::floor(
                startTime * sampleRate
            )
        );

    juce::int64 endSample =
        static_cast<juce::int64>(
            std::ceil(
                endTime * sampleRate
            )
        );

    // ==========================================================
    // CLAMP SAMPLE RANGE
    // ==========================================================

    startSample =
        juce::jlimit<juce::int64>(
            0,
            totalSamples,
            startSample
        );

    endSample =
        juce::jlimit<juce::int64>(
            0,
            totalSamples,
            endSample
        );

    if (endSample <= startSample)
        return false;

    const juce::int64 samplesToWrite =
        endSample - startSample;

    // ==========================================================
    // CREATE OUTPUT FORMAT
    // ==========================================================

    std::unique_ptr<juce::AudioFormat> outputFormat;

    const auto extension =
        destination.getFileExtension()
            .toLowerCase();

    if (extension == ".flac")
    {
        outputFormat =
            std::make_unique<juce::FlacAudioFormat>();
    }
    else if (extension == ".aiff"
             || extension == ".aif")
    {
        outputFormat =
            std::make_unique<juce::AiffAudioFormat>();
    }
    else
    {
        outputFormat =
            std::make_unique<juce::WavAudioFormat>();
    }

    if (outputFormat == nullptr)
        return false;

    // ==========================================================
    // CREATE OUTPUT FILE
    // ==========================================================

    if (destination.existsAsFile())
    {
        if (!destination.deleteFile())
            return false;
    }

    std::unique_ptr<juce::OutputStream> outputStream(
        destination.createOutputStream()
    );

    if (outputStream == nullptr)
        return false;

    // ==========================================================
    // OUTPUT OPTIONS
    // ==========================================================

    juce::AudioFormatWriter* rawWriter =
        outputFormat->createWriterFor(
            outputStream.release(),
            sampleRate,
            static_cast<unsigned int>(numChannels),
            24,
            {},
            0
        );

    if (rawWriter == nullptr)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer(
        rawWriter
    );

    constexpr int bufferSize = 8192;
    juce::AudioBuffer<float> buffer(numChannels, bufferSize);

    // Measure one peak across the full export range so normalization does
    // not change gain from block to block.
    float normalizationGain = 1.0f;
    if (normalize)
    {
        float peak = 0.0f;
        juce::int64 samplesScanned = 0;
        while (samplesScanned < samplesToWrite)
        {
            const int samplesThisBlock = static_cast<int>(
                juce::jmin<juce::int64>(bufferSize, samplesToWrite - samplesScanned));
            if (!reader->read(&buffer, 0, samplesThisBlock,
                              startSample + samplesScanned, true, true))
                return false;

            for (int channel = 0; channel < numChannels; ++channel)
                peak = juce::jmax(peak, buffer.getMagnitude(channel, 0, samplesThisBlock));
            samplesScanned += samplesThisBlock;
        }

        if (peak > 0.000001f)
            normalizationGain = 0.999f / peak;
    }

    // ==========================================================
    // WRITE SELECTED AUDIO
    // ==========================================================

    juce::int64 samplesWritten = 0;

    while (samplesWritten < samplesToWrite)
    {
        const int samplesThisBlock =
            static_cast<int>(
                juce::jmin<juce::int64>(
                    bufferSize,
                    samplesToWrite - samplesWritten
                )
            );

        if (!reader->read(
                &buffer,
                0,
                samplesThisBlock,
                startSample + samplesWritten,
                true,
                true))
        {
            return false;
        }

        if (normalize && normalizationGain != 1.0f)
            buffer.applyGain(0, samplesThisBlock, normalizationGain);

        writer->writeFromAudioSampleBuffer(
            buffer,
            0,
            samplesThisBlock
        );

        samplesWritten +=
            samplesThisBlock;
    }

    return true;
}


// ==============================================================
// PLAYBACK SELECTION POSITIONS
// ==============================================================

double MainComponent::getPlaybackStartPosition() const
{
    if (!playbackSelectionActive)
        return 0.0;

    return juce::jlimit(
        0.0,
        playbackLength,
        playbackSelectionStart
    );
}

// ==============================================================

double MainComponent::getPlaybackEndPosition() const
{
    if (!playbackSelectionActive)
        return playbackLength;

    return juce::jlimit(
        0.0,
        playbackLength,
        playbackSelectionEnd
    );
}


// ==============================================================
// GET EXPORT EXTENSION
// ==============================================================

juce::String MainComponent::getExportExtension() const
{
    switch (getSelectedExportFormat())
    {
        case ExportFormat::wav:
            return ".wav";

        case ExportFormat::flac:
            return ".flac";

        case ExportFormat::aiff:
            return ".aiff";
    }

    return ".wav";
}


// ==============================================================
// GET SELECTED EXPORT FORMAT
// ==============================================================

MainComponent::ExportFormat
MainComponent::getSelectedExportFormat() const
{
    switch (exportFormatId)
    {
        case 2: return ExportFormat::flac;
        case 3: return ExportFormat::aiff;
        default: return ExportFormat::wav;
    }
}


// ==============================================================
// EXPORT SELECTED AUDIO
// ==============================================================

void MainComponent::exportSelectedAudio()
{
    if (!vocalsTrack.isVisible())
    {
        setStatus(
            "No separated stems available."
        );

        return;
    }

    if (!playbackSelectionActive)
    {
        setStatus(
            "Select a region on the waveform first."
        );

        return;
    }

    // ==========================================================
    // FIND SELECTED STEM
    // ==========================================================

    juce::File source;

    if (vocalsTrack.isTrackSelected())
        source = vocalsFile;
    else if (drumsTrack.isTrackSelected())
        source = drumsFile;
    else if (bassTrack.isTrackSelected())
        source = bassFile;
    else if (instrumentalTrack.isTrackSelected())
        source = instrumentalFile;
    else if (guitarTrack.isTrackSelected())
        source = guitarFile;
    else if (pianoTrack.isTrackSelected())
        source = pianoFile;

    if (!source.existsAsFile())
    {
        setStatus(
            "Select a stem first."
        );

        return;
    }

    // ==========================================================
    // GET EXPORT RANGE
    // ==========================================================

    const double startTime =
        getPlaybackStartPosition();

    const double endTime =
        getPlaybackEndPosition();

    if (endTime <= startTime)
    {
        setStatus(
            "Invalid export selection."
        );

        return;
    }

    // ==========================================================
    // EXPORT EXTENSION
    // ==========================================================

    const juce::String extension =
        getExportExtension();

    // ==========================================================
    // DEFAULT FILE NAME
    // ==========================================================

    juce::String fileName =
        source.getFileNameWithoutExtension()
        + "_selected"
        + extension;

    // ==========================================================
    // SAVE DIALOG
    // ==========================================================

    auto chooser =
        std::make_shared<juce::FileChooser>(
            "Export selected audio...",
            outputFolder.getChildFile(
                fileName
            ),
            "*" + extension
        );

    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode
        | juce::FileBrowserComponent::canSelectFiles
        | juce::FileBrowserComponent::warnAboutOverwriting,

        [this,
         chooser,
         source,
         startTime,
         endTime](
            const juce::FileChooser&)
        {
            auto destination =
                chooser->getResult();

            if (destination.getFullPathName().isEmpty())
                return;

            // ==================================================
            // WRITE SELECTED AUDIO
            // ==================================================

            const bool normalize = normalizeExportsEnabled;

            const bool success =
                writeAudioFile(
                    source,
                    destination,
                    startTime,
                    endTime,
                    normalize
                );

            if (success)
            {
                setStatus(
                    "Export complete: "
                    + destination.getFileName()
                );
            }
            else
            {
                setStatus(
                    "ERROR: Export failed."
                );
            }
        }
    );
}




// ==============================================================
// EXPORT ALL STEMS
// ==============================================================

void MainComponent::exportAllStems()
{
    if (!vocalsTrack.isVisible())
    {
        setStatus(
            "No separated stems available."
        );

        return;
    }

    if (!outputFolder.isDirectory())
    {
        setStatus(
            "ERROR: Output folder not found."
        );

        return;
    }

    juce::File exportFolder =
        outputFolder.getChildFile(
            "Exported"
        );

    if (!exportFolder.exists())
        exportFolder.createDirectory();

    juce::Array<juce::File> stems;
    stems.add(vocalsFile);
    stems.add(drumsFile);
    stems.add(bassFile);
    stems.add(instrumentalFile);
    if (guitarFile.existsAsFile() && pianoFile.existsAsFile())
    {
        stems.add(guitarFile);
        stems.add(pianoFile);
    }

    bool success = true;
    const auto extension = getExportExtension();
    for (const auto& stem : stems)
    {
        const auto destination = exportFolder.getChildFile(
            stem.getFileNameWithoutExtension() + extension);
        success = writeAudioFile(stem, destination, 0.0, playbackLength,
                                 normalizeExportsEnabled) && success;
    }

    if (success)
    {
        setStatus(
            "All stems exported."
        );

        exportFolder.revealToUser();
    }
    else
    {
        setStatus(
            "ERROR: Could not export all stems."
        );
    }
}


// ==============================================================
// OPEN OUTPUT FOLDER
// ==============================================================

void MainComponent::openOutputFolder()
{
    if (!outputFolder.isDirectory())
    {
        setStatus(
            "Output folder not found."
        );

        return;
    }

    outputFolder.revealToUser();
}


// ==============================================================
// SEPARATOR OUTPUT THREAD
// ==============================================================
void MainComponent::SeparatorOutputThread::run()
{
    while (!threadShouldExit())
    {
        if (owner.separatorProcess == nullptr)
            break;

        char buffer[4096] = {};

        const int bytesRead =
            owner.separatorProcess->readProcessOutput(
                buffer,
                static_cast<int>(sizeof(buffer) - 1)
            );

        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';

            const juce::String output(buffer);

            std::cout
    << "\n========== RAW PYTHON OUTPUT ==========\n"
    << "Bytes received: "
    << bytesRead
    << "\n"
    << output.toStdString()
    << "\n========== END PYTHON OUTPUT ==========\n"
    << std::endl;

            {
                const juce::ScopedLock lock(
                    owner.separatorOutputLock
                );

                owner.separatorOutputBuffer += output;

                owner.parseSeparatorProgress(output);
            }
        }

        if (!owner.separatorProcess->isRunning())
        {
            std::cout
                << "\n========================================\n"
                << "     SEPARATOR PROCESS STOPPED\n"
                << "========================================\n"
                << "Process is no longer running.\n"
                << "========================================\n"
                << std::endl;

            break;
        }

        wait(10);
    }
}