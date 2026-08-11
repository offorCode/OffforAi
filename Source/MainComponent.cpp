#include "MainComponent.h"
#include "AppConfig.h"

#include <cmath>

#include <iostream>

// ==============================================================
// CONSTRUCTOR
// ==============================================================

MainComponent::MainComponent()
{

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

setSize(
    620,
    400
);

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
// SETTINGS
// ==========================================================

settingsButton.setButtonText(
    "SETTINGS"
);

settingsButton.onClick = [this]
{
    if (settingsWindow != nullptr)
        return;

    settingsComponent =
    std::make_unique<SettingsComponent>();

    // ==========================================================
    // SETTINGS CLOSE CALLBACK
    // ==========================================================

    settingsComponent->onClose =
        [this]
    {
        juce::MessageManager::callAsync(
            [this]
            {
                settingsWindow.reset();
            }
        );
    };

    settingsComponent->setAutoPlayEnabled(
        autoPlayEnabled
    );

    settingsComponent->setLoopSelectionEnabled(
        loopSelectionEnabled
    );

    settingsComponent->setNormalizeEnabled(
        false
    );

    settingsComponent->onSettingsChanged =
    [this](
        bool autoPlay,
        bool loopSelection,
        int exportFormat,
        bool normalize
     )
    {
        autoPlayEnabled =
            autoPlay;

        loopSelectionEnabled =
            loopSelection;

        juce::ignoreUnused(
            exportFormat,
            normalize
        );
    };

    settingsWindow =
        std::make_unique<juce::DialogWindow>(
            "Settings",
            juce::Colour(0xff111417),
            true
        );

    settingsWindow->setContentOwned(
        settingsComponent.release(),
        true
    );

    settingsWindow->centreWithSize(
        420,
        430
    );

    settingsWindow->setResizable(
        false,
        false
    );

    settingsWindow->setUsingNativeTitleBar(
        false
    );

    settingsWindow->setVisible(
        true
    );
};

addAndMakeVisible(
    settingsButton
);

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
// AUTO-PLAY
// ==========================================================

addAndMakeVisible(
    autoPlayButton
);

autoPlayButton.setToggleState(
    autoPlayEnabled,
    juce::dontSendNotification
);

autoPlayButton.setColour(
    juce::ToggleButton::textColourId,
    juce::Colour(0xffd7d7d7)
);

autoPlayButton.setColour(
    juce::ToggleButton::tickColourId,
    juce::Colour(0xffff7a00)
);

autoPlayButton.onClick = [this]
{
    autoPlayEnabled =
        autoPlayButton.getToggleState();
};

// ==========================================================
// LOOP SELECTION
// ==========================================================

addAndMakeVisible(
    loopSelectionButton
);

loopSelectionButton.setToggleState(
    loopSelectionEnabled,
    juce::dontSendNotification
);

loopSelectionButton.setColour(
    juce::ToggleButton::textColourId,
    juce::Colour(0xffd7d7d7)
);

loopSelectionButton.setColour(
    juce::ToggleButton::tickColourId,
    juce::Colour(0xffff7a00)
);

loopSelectionButton.onClick = [this]
{
    loopSelectionEnabled =
        loopSelectionButton.getToggleState();
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
    "04  Instrumental"
);

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


// ==============================================================
// DESTRUCTOR
// ==============================================================

MainComponent::~MainComponent()
{
    removeKeyListener(this);

    if (playbackTimer)
    playbackTimer->stopTimer();

    if (progressAnimationTimer)
    progressAnimationTimer->stopTimer();

    stopTimer();

    stopPlayback();

    unloadStemAudio();

    audioSourcePlayer.setSource(
        nullptr
    );

    audioDeviceManager.removeAudioCallback(
        &audioSourcePlayer
    );

    if (separatorProcess != nullptr)
    {
        if (separatorProcess->isRunning())
            separatorProcess->kill();
    }

    stopSeparatorOutputThread();

    separatorProcess.reset();
}



// ==========================================================
// PAINT
// ==========================================================

void MainComponent::paint(juce::Graphics& g)
{


    const auto bounds = getLocalBounds();
    const bool showLargeDropZone =
    !vocalsTrack.isVisible();

    // ======================================================
    // MAIN BACKGROUND
    // ======================================================

    g.fillAll(
        juce::Colour(0xff080a0c)
    );

    // ======================================================
    // CONTENT AREA
    // ======================================================

    auto content =
        bounds.reduced(24);

    // ======================================================
    // HEADER BACKGROUND
    // ======================================================

    auto header =
        content.removeFromTop(46);

    g.setColour(
        juce::Colour(0xff0e1114)
    );

    g.fillRoundedRectangle(
        header.toFloat(),
        8.0f
    );

    g.setColour(
        juce::Colour(0xff252b30)
    );

    g.drawRoundedRectangle(
        header.toFloat().reduced(0.5f),
        8.0f,
        1.0f
    );

    // ======================================================
    // OFFOR LOGO
    // ======================================================

    if (!offorLogo.isNull())
    {
        auto logoArea =
            header
                .removeFromLeft(42)
                .reduced(9, 8);

        g.drawImageWithin(
            offorLogo,
            logoArea.getX(),
            logoArea.getY(),
            logoArea.getWidth(),
            logoArea.getHeight(),
            juce::RectanglePlacement::centred
                | juce::RectanglePlacement::onlyReduceInSize,
            false
        );
    }

    // ======================================================
    // HEADER DIVIDER
    // ======================================================

    g.setColour(
        juce::Colour(0xff252a2e)
    );

    g.fillRect(
        header.getX(),
        header.getY() + 10,
        1,
        header.getHeight() - 20
    );

    // ======================================================
    // DROP ZONE
    // ======================================================

    auto dropZone =
        dropZoneBounds.toFloat();

    // Shadow / depth
    g.setColour(
        juce::Colours::black.withAlpha(0.25f)
    );

    g.fillRoundedRectangle(
        dropZone.translated(0.0f, 2.0f),
        10.0f
    );

    // Main panel
    g.setColour(
        juce::Colour(0xff101316)
    );

    g.fillRoundedRectangle(
        dropZone,
        10.0f
    );

    // ======================================================
    // DROP ZONE BORDER
    // ======================================================

    const auto orange =
        juce::Colour(0xffff6b22);

    g.setColour(
        isDragOver
            ? orange
            : orange.withAlpha(0.42f)
    );

    g.drawRoundedRectangle(
        dropZone.reduced(0.5f),
        10.0f,
        isDragOver ? 2.0f : 1.0f
    );

    // ======================================================
    // DRAG OVER HIGHLIGHT
    // ======================================================

    if (isDragOver)
    {
        g.setColour(
            orange.withAlpha(0.08f)
        );

        g.fillRoundedRectangle(
            dropZone.reduced(2.0f),
            8.0f
        );
    }

    // ======================================================
    // DROP ZONE CONTENT
    // ======================================================

    if (separationRunning)
    {
        // ==================================================
        // PROCESSING VISUALIZATION
        // ==================================================

        const int centreX =
            dropZoneBounds.getCentreX();

        const int centreY =
            dropZoneBounds.getCentreY();

        // ----------------------------------------------
        // Animated outer circle
        // ----------------------------------------------

        const float pulse =
            0.72f
            + 0.18f
            * std::sin(
                static_cast<float>(progressAnimationFrame)
                * 0.12f
            );

        g.setColour(
            orange.withAlpha(
                0.08f * pulse
            )
        );

        g.fillEllipse(
            static_cast<float>(centreX - 32),
            static_cast<float>(centreY - 32),
            64.0f,
            64.0f
        );

        g.setColour(
            orange.withAlpha(
                0.65f * pulse
            )
        );

        g.drawEllipse(
            static_cast<float>(centreX - 23),
            static_cast<float>(centreY - 23),
            46.0f,
            46.0f,
            1.2f
        );

        // ----------------------------------------------
        // Animated audio bars
        // ----------------------------------------------

        constexpr int numBars = 15;
        constexpr int barWidth = 4;
        constexpr int barGap = 4;
        constexpr int totalWidth =
            (numBars * barWidth)
            + ((numBars - 1) * barGap);

        const int startX =
            centreX
            - (totalWidth / 2);

        const int baseY =
            centreY + 18;

        for (int i = 0; i < numBars; ++i)
        {
            const float phase =
                static_cast<float>(i) * 0.55f
                + static_cast<float>(
                    progressAnimationFrame
                ) * 0.10f;

            const float wave =
                0.5f
                + 0.5f
                * std::sin(phase);

            const float barHeight =
                8.0f
                + wave * 32.0f;

            const int x =
                startX
                + i * (barWidth + barGap);

            g.setColour(
                orange.withAlpha(
                    0.35f
                    + 0.50f * wave
                )
            );

            g.fillRoundedRectangle(
                static_cast<float>(x),
                static_cast<float>(
                    baseY - barHeight
                ),
                static_cast<float>(barWidth),
                barHeight,
                2.0f
            );
        }

        // ==================================================
        // PROCESSING TITLE
        // ==================================================

        auto processingText =
            dropZoneBounds
                .reduced(25);

        processingText.setY(
            centreY - 64
        );

        processingText.setHeight(
            22
        );

        g.setColour(
            orange.withAlpha(0.92f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(14.0f)
                    .withStyle("Bold")
            )
        );

        g.drawText(
            "ANALYZING AUDIO",
            processingText,
            juce::Justification::centred,
            false
        );

        // ==================================================
        // REAL PROGRESS
        // ==================================================

        auto progressText =
            dropZoneBounds
                .reduced(25);

        progressText.setY(
            centreY + 38
        );

        progressText.setHeight(
            22
        );

        g.setColour(
            juce::Colours::white.withAlpha(0.75f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(11.0f)
            )
        );

        // g.drawText(
        //     "Processing... "
        //     + juce::String(
        //         static_cast<int>(
        //             separationProgress * 100.0
        //         )
        //     )
        //     + "%",
        //     progressText,
        //     juce::Justification::centred,
        //     false
        // );

        g.drawText(
            "Processing...",
            progressText,
            juce::Justification::centred,
            false
        );

        // ==================================================
        // SECONDARY MESSAGE
        // ==================================================

        auto messageText =
            dropZoneBounds
                .reduced(25);

        messageText.setY(
            centreY + 58
        );

        messageText.setHeight(
            18
        );

        g.setColour(
            orange.withAlpha(0.42f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(9.0f)
            )
        );

        g.drawText(
            "Extracting vocals • drums • bass • instrumental",
            messageText,
            juce::Justification::centred,
            false
        );
    }
    else if (!selectedFile.existsAsFile())
    {
        const int centreX =
            dropZoneBounds.getCentreX();

        const int centreY =
            dropZoneBounds.getCentreY();

        // ==================================================
        // ORIGINAL ADD ICON
        // ==================================================

        g.setColour(
            orange.withAlpha(0.12f)
        );

        g.fillEllipse(
            static_cast<float>(centreX - 23),
            static_cast<float>(centreY - 23),
            46.0f,
            46.0f
        );

        g.setColour(
            orange.withAlpha(0.75f)
        );

        g.drawEllipse(
            static_cast<float>(centreX - 23),
            static_cast<float>(centreY - 23),
            46.0f,
            46.0f,
            1.0f
        );

        // Small plus
        g.fillRect(
            centreX - 1,
            centreY - 9,
            2,
            18
        );

        g.fillRect(
            centreX - 9,
            centreY - 1,
            18,
            2
        );

        // ==================================================
        // MAIN TEXT
        // ==================================================

        auto textArea =
            dropZoneBounds.reduced(25);

        textArea.removeFromTop(
            showLargeDropZone
                ? 34
                : 8
        );

        g.setColour(
            orange.withAlpha(0.90f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(
                        showLargeDropZone
                            ? 17.0f
                            : 12.0f
                    )
                    .withStyle("Bold")
            )
        );

        g.drawFittedText(
            showLargeDropZone
                ? "DROP AUDIO FILE HERE"
                : "ADD ANOTHER AUDIO FILE",
            textArea.removeFromTop(24),
            juce::Justification::centred,
            1
        );

        // ==================================================
        // SUPPORTED FORMATS
        // ==================================================

        if (showLargeDropZone)
        {
            g.setColour(
                orange.withAlpha(0.42f)
            );

            g.setFont(
                juce::Font(
                    juce::FontOptions()
                        .withHeight(12.0f)
                )
            );

            g.drawText(
                "WAV   |   MP3   |   FLAC   |   AIFF",
                textArea.removeFromTop(22),
                juce::Justification::centred,
                false
            );

            g.setColour(
                orange.withAlpha(0.25f)
            );

            g.setFont(
                juce::Font(
                    juce::FontOptions()
                        .withHeight(9.0f)
                )
            );

            g.drawText(
                "Click to browse files",
                textArea.removeFromTop(18),
                juce::Justification::centred,
                false
            );
        }
    }

    // ======================================================
    // STEMS HEADER
    // ======================================================

    if (vocalsTrack.isVisible())
    {
        auto stemsHeader = getLocalBounds().reduced(24);

        stemsHeader.removeFromTop(
            46 + 12 + dropZoneBounds.getHeight()
        );

        stemsHeader.removeFromTop(8);

        auto headerRow = stemsHeader.removeFromTop(22);

        // --------------------------------------------------
        // LEFT TITLE
        // --------------------------------------------------

        g.setColour(orange.withAlpha(0.75f));

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(10.0f)
                    .withStyle("Bold")
            )
        );

        g.drawText(
            "SEPARATED STEMS",
            headerRow.removeFromLeft(160),
            juce::Justification::centredLeft,
            false
        );

        // --------------------------------------------------
        // RIGHT DURATION
        // --------------------------------------------------

        if (playbackLength > 0.0)
        {
            const int totalSeconds =
                static_cast<int>(playbackLength);

            const int minutes =
                totalSeconds / 60;

            const int seconds =
                totalSeconds % 60;

            const juce::String duration =
                juce::String(minutes)
                + ":"
                + juce::String(seconds).paddedLeft('0', 2);

            g.setColour(orange.withAlpha(0.32f));

            g.setFont(
                juce::Font(
                    juce::FontOptions()
                        .withHeight(9.0f)
                )
            );

            g.drawText(
                duration,
                headerRow,
                juce::Justification::centredRight,
                false
            );
        }
    }
    // ======================================================
    // BOTTOM BAR
    // ======================================================

    auto bottom =
        getLocalBounds()
            .reduced(24)
            .removeFromBottom(34);

    g.setColour(
        juce::Colour(0xff0e1114)
    );

    g.fillRoundedRectangle(
        bottom.toFloat(),
        7.0f
    );

    g.setColour(
        juce::Colour(0xff252a2e)
    );

    g.drawRoundedRectangle(
        bottom.toFloat().reduced(0.5f),
        7.0f,
        1.0f
    );

    // ======================================================
    // TOP ACCENT LINE
    // ======================================================

    g.setColour(
        orange.withAlpha(0.65f)
    );

    g.fillRoundedRectangle(
        static_cast<float>(bottom.getX() + 12),
        static_cast<float>(bottom.getY()),
        42.0f,
        1.0f,
        0.5f
    );
}


// ==============================================================
// MOUSE DOWN
// ==============================================================

void MainComponent::mouseDown(
    const juce::MouseEvent& event
)
{
    if (dropZoneBounds.contains(
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
    auto area =
        getLocalBounds().reduced(24);

    // ======================================================
    // HEADER
    // ======================================================

    auto header =
        area.removeFromTop(46);

    settingsButton.setBounds(
        header.removeFromRight(130)
    );

    // Logo
    auto logoArea =
        header
            .removeFromLeft(42)
            .reduced(8);

    juce::ignoreUnused(
        logoArea
    );

    // Header title
    headerTitleLabel.setBounds(
        header.reduced(12, 0)
    );

    // ======================================================
    // SPACE
    // ======================================================

    area.removeFromTop(12);

    // ======================================================
    // DROP ZONE
    // ======================================================

    const bool showLargeDropZone =
        !vocalsTrack.isVisible();

    const int dropZoneHeight =
        showLargeDropZone
            ? 180
            : 48;

    dropZoneBounds =
        area.removeFromTop(
            dropZoneHeight
        );

    // ======================================================
    // FILE LABEL
    // ======================================================

    area.removeFromTop(4);

    fileLabel.setBounds(
        area.removeFromTop(18)
    );

    // ======================================================
    // PROGRESS
    // ======================================================

    progressLabel.setBounds(
        area.removeFromTop(26)
    );

    // ======================================================
    // STATUS
    // ======================================================

    statusLabel.setBounds(
        area.removeFromTop(18)
    );

    // ======================================================
    // STEMS
    // ======================================================

    if (vocalsTrack.isVisible())
    {
        area.removeFromTop(6);

        constexpr int trackHeight = 64;
        constexpr int gap = 4;

        auto vocalsArea =
            area.removeFromTop(
                trackHeight
            );

        vocalsTrack.setBounds(
            vocalsArea
        );

        vocalsLabel.setVisible(false);

        area.removeFromTop(gap);

        auto drumsArea =
            area.removeFromTop(
                trackHeight
            );

        drumsTrack.setBounds(
            drumsArea
        );

        drumsLabel.setVisible(false);

        area.removeFromTop(gap);

        auto bassArea =
            area.removeFromTop(
                trackHeight
            );

        bassTrack.setBounds(
            bassArea
        );

        bassLabel.setVisible(false);

        area.removeFromTop(gap);

        auto instrumentalArea =
            area.removeFromTop(
                trackHeight
            );

        instrumentalTrack.setBounds(
            instrumentalArea
        );

        instrumentalLabel.setVisible(false);
    }

    // ======================================================
    // TRANSPORT
    // ======================================================

    const bool showTransport =
        vocalsTrack.isVisible();

    playButton.setVisible(
        showTransport
    );

    pauseButton.setVisible(
        showTransport
    );

    stopButton.setVisible(
        showTransport
    );

    if (showTransport)
    {
        area.removeFromTop(8);

        auto transportArea =
            area.removeFromTop(38);

        // Keep transport centered
        const int transportWidth =
            300;

        auto transport =
            transportArea
                .withSizeKeepingCentre(
                    transportWidth,
                    38
                );

        playButton.setBounds(
            transport.removeFromLeft(100)
                .reduced(3, 2)
        );

        pauseButton.setBounds(
            transport.removeFromLeft(100)
                .reduced(3, 2)
        );

        stopButton.setBounds(
            transport.reduced(3, 2)
        );
    }

    // ======================================================
    // FOOTER
    // ======================================================

    auto bottom =
        getLocalBounds()
            .reduced(24)
            .removeFromBottom(34);

    // auto footer = bottom.reduced(8, 0);

    // Company - LEFT
    companyLabel.setBounds(
        bottom.removeFromLeft(120)
    );

    // Version - RIGHT
    versionLabel.setBounds(
        bottom.removeFromRight(110)
    );

    // ======================================================
    // OLD EXPORT UI REMOVED
    // ======================================================

}

// ==============================================================
// MOUSE CURSOR
// ==============================================================

juce::MouseCursor MainComponent::getMouseCursor()
{
    if (dropZoneBounds.contains(
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

    repaint();
}

// ==============================================================
// SHOW STEMS
// ==============================================================

void MainComponent::showSeparatedStems()
{

    if (!outputFolder.isDirectory())
    {
        setStatus(
            "ERROR: Output folder not found"
        );

        return;
    }

    // ==========================================================
    // Locate stems
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

    std::cout << "VOCALS: "
          << vocalsFile.getFullPathName()
          << " EXISTS="
          << vocalsFile.existsAsFile()
          << std::endl;

    std::cout << "DRUMS: "
            << drumsFile.getFullPathName()
            << " EXISTS="
            << drumsFile.existsAsFile()
            << std::endl;

    std::cout << "BASS: "
            << bassFile.getFullPathName()
            << " EXISTS="
            << bassFile.existsAsFile()
            << std::endl;

    std::cout << "INSTRUMENTAL: "
            << instrumentalFile.getFullPathName()
            << " EXISTS="
            << instrumentalFile.existsAsFile()
            << std::endl;
    // ==========================================================
    // Verify
    // ==========================================================

    if (!vocalsFile.existsAsFile()
        || !drumsFile.existsAsFile()
        || !bassFile.existsAsFile()
        || !instrumentalFile.existsAsFile())
    {
        setStatus(
            "ERROR: Stem files not found"
        );

        return;
    }

    // ==========================================================
    // Waveforms
    // ==========================================================

    vocalsTrack.setAudioFile(vocalsFile);
    drumsTrack.setAudioFile(drumsFile);
    bassTrack.setAudioFile(bassFile);
    instrumentalTrack.setAudioFile(instrumentalFile);

    // ==========================================================
    // Audio playback
    // ==========================================================

    if (!loadStemAudio())
    {
        std::cout << "=== loadStemAudio() FAILED ===" << std::endl;

        setStatus(
            "ERROR: Could not load stem audio"
        );

        return;
    }

    // ==========================================================
    // Timeline
    // ==========================================================

    playbackLength =
        vocalsTrack.getAudioLength();


    playbackPosition = 0.0;

    vocalsTrack.setPlayheadPosition(0.0);
    drumsTrack.setPlayheadPosition(0.0);
    bassTrack.setPlayheadPosition(0.0);
    instrumentalTrack.setPlayheadPosition(0.0);

    // ==========================================================
    // Show controls
    // ==========================================================

    
    setStemUIVisible(true);
    setTransportVisible(true);

    updateTrackMixing();

    progressLabel.setVisible(true);

    progressLabel.setText(
        "Separation complete",
        juce::dontSendNotification
    );

    // ==========================================================
    // Expand
    // ==========================================================

    expandForTracks();

    resized();

    setStatus(
        "Finished splitting the track."
    );

    // ==========================================================
    // AUTO-PLAY
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

    if (vocalsReader == nullptr
        || drumsReader == nullptr
        || bassReader == nullptr
        || instrumentalReader == nullptr)
    {
        delete vocalsReader;
        delete drumsReader;
        delete bassReader;
        delete instrumentalReader;

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

    mixerSource.removeAllInputs();

    if (vocalsTransport)
        vocalsTransport->stop();

    if (drumsTransport)
        drumsTransport->stop();

    if (bassTransport)
        bassTransport->stop();

    if (instrumentalTransport)
        instrumentalTransport->stop();

    vocalsTransport.reset();
    drumsTransport.reset();
    bassTransport.reset();
    instrumentalTransport.reset();

    vocalsReaderSource.reset();
    drumsReaderSource.reset();
    bassReaderSource.reset();
    instrumentalReaderSource.reset();

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
        || !instrumentalTransport)
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

    const bool anySolo =
        vocalsSolo
        || drumsSolo
        || bassSolo
        || instrumentalSolo;

    vocalsTransport->setGain(
        anySolo
            ? (vocalsSolo ? 1.0f : 0.0f)
            : (vocalsTrack.isMuted() ? 0.0f : 1.0f)
    );

    drumsTransport->setGain(
        anySolo
            ? (drumsSolo ? 1.0f : 0.0f)
            : (drumsTrack.isMuted() ? 0.0f : 1.0f)
    );

    bassTransport->setGain(
        anySolo
            ? (bassSolo ? 1.0f : 0.0f)
            : (bassTrack.isMuted() ? 0.0f : 1.0f)
    );

    instrumentalTransport->setGain(
        anySolo
            ? (instrumentalSolo ? 1.0f : 0.0f)
            : (instrumentalTrack.isMuted() ? 0.0f : 1.0f)
    );
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

    runSeparator();
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
    if (separatorOutputThread)
    {
        separatorOutputThread->stopThread(
            2000
        );

        separatorOutputThread.reset();
    }
}

// ==============================================================
// RUN SEPARATOR
// ==============================================================

void MainComponent::runSeparator()
{
    
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
    // OFFOR AI PYTHON
    // ==========================================================

    const juce::File pythonExecutable(
        "C:\\AudioDevelopment\\OfforAudioDev"
        "\\OfforAI\\demucs-env\\Scripts\\python.exe"
    );

    // ==========================================================
    // OFFOR SEPARATOR SCRIPT
    // ==========================================================

    const juce::File separatorScript(
        "C:\\AudioDevelopment\\OfforAudioDev"
        "\\OfforAI\\demucs-engine\\separator.py"
    );

    if (!pythonExecutable.existsAsFile())
    {
        setStatus(
            "ERROR: Python environment not found."
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
    // RESET SEPARATOR STATE
    // ==========================================================

    separationProgress = 0.0;

    separationRunning = true;

    progressAnimationFrame = 0;

    if (progressAnimationTimer != nullptr)
        progressAnimationTimer->startTimer(40);

    cancelButton.setVisible(
        true
    );

    separatorOutputBuffer.clear();

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

    if (!started)
    {
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
    auto lines =
        juce::StringArray::fromLines(output);

    for (const auto& line : lines)
    {
        auto trimmed =
            line.trim();

        // ==================================================
        // PROGRESS
        // ==================================================

        if (trimmed.startsWith(
                "OFFOR_PROGRESS:"))
        {
            auto progressText =
                trimmed.fromFirstOccurrenceOf(
                    "OFFOR_PROGRESS:",
                    false,
                    false
                ).trim();

            const int progress =
                progressText.getIntValue();

            separationProgress =
                juce::jlimit(
                    0.0,
                    1.0,
                    progress / 100.0
                );

            

            juce::MessageManager::callAsync(
                [this, progress]()
                {
                    if (separationRunning)
                    {
                        progressLabel.setVisible(true);

                        progressLabel.setText(
                            juce::String(progress) + "%",
                            juce::dontSendNotification
                        );

                        progressLabel.repaint();

                        repaint();
                    }
                }
            );

            continue;
        }

        // ==================================================
        // OUTPUT FOLDER
        // ==================================================

        if (trimmed.startsWith(
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
                    << "OFFOR OUTPUT FOLDER = "
                    << outputFolder.getFullPathName()
                    << std::endl;
            }

            continue;
        }

        // ==================================================
        // STEMS READY
        // ==================================================

        if (trimmed == "OFFOR_STEMS_READY")
        {
            std::cout
                << "=== OFFOR STEMS READY ==="
                << std::endl;

            continue;
        }
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

    // Stop output reader first so it no longer reads the process.
    stopSeparatorOutputThread();

    // ==========================================================
    // CHECK WHETHER SEPARATION WAS CANCELLED
    // ==========================================================

    if (!separationRunning)
    {
        separatorProcess.reset();
        return;
    }

    // ==========================================================
    // SEPARATION COMPLETE
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
    // CLEAN UP PROCESS
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

    if (vocalsTrack.isVisible())
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

    if (vocalsTrack.isVisible())
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
        if (separatorProcess->isRunning())
            separatorProcess->kill();
    }

    stopSeparatorOutputThread();

    separatorProcess.reset();

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

    if (playbackPosition >= playbackLength)
        playbackPosition = 0.0;

    vocalsTransport->setPosition(playbackPosition);
    drumsTransport->setPosition(playbackPosition);
    bassTransport->setPosition(playbackPosition);
    instrumentalTransport->setPosition(playbackPosition);

    vocalsTransport->start();
    drumsTransport->start();
    bassTransport->start();
    instrumentalTransport->start();

    isPlaying = true;

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

    // ==========================================================
    // Keep waveform playheads synchronized
    // ==========================================================

    vocalsTrack.setPlayheadPosition(playbackPosition);
    drumsTrack.setPlayheadPosition(playbackPosition);
    bassTrack.setPlayheadPosition(playbackPosition);
    instrumentalTrack.setPlayheadPosition(playbackPosition);

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
    // KEEP PLAYHEAD AT CURRENT POSITION
    // ==========================================================

    isPlaying = false;

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
}


// ==============================================================
// EXPAND WINDOW FOR TRACKS
// ==============================================================

void MainComponent::expandForTracks()
{
    setSize(
        840,
        600
    );

    resized();
}


// ==============================================================
// SHRINK WINDOW FOR INPUT
// ==============================================================

void MainComponent::shrinkToInputWindow()
{
    setSize(
        620,
        400
    );

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

    // ==========================================================
    // READ SELECTED AUDIO
    // ==========================================================

    constexpr int bufferSize = 8192;

    juce::AudioBuffer<float> buffer(
        numChannels,
        bufferSize
    );

    reader->read(
        &buffer,
        0,
        static_cast<int>(
            juce::jmin<juce::int64>(
                samplesToWrite,
                bufferSize
            )
        ),
        startSample,
        true,
        true
    );

    

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

        if (normalize)
        {
            float peak = 0.0f;

            for (int channel = 0;
                 channel < numChannels;
                 ++channel)
            {
                peak =
                    juce::jmax(
                        peak,
                        buffer.getMagnitude(
                            channel,
                            0,
                            samplesThisBlock
                        )
                    );
            }

            if (peak > 0.000001f)
            {
                const float gain =
                    0.999f / peak;

                buffer.applyGain(
                    0,
                    samplesThisBlock,
                    gain
                );
            }
        }

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
    return ExportFormat::wav;
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

            // Normalization is currently disabled.
            const bool normalize = false;

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

    bool success = true;

    success =
        vocalsFile.copyFileTo(
            exportFolder.getChildFile(
                vocalsFile.getFileName()
            )
        )
        && success;

    success =
        drumsFile.copyFileTo(
            exportFolder.getChildFile(
                drumsFile.getFileName()
            )
        )
        && success;

    success =
        bassFile.copyFileTo(
            exportFolder.getChildFile(
                bassFile.getFileName()
            )
        )
        && success;

    success =
        instrumentalFile.copyFileTo(
            exportFolder.getChildFile(
                instrumentalFile.getFileName()
            )
        )
        && success;

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
                << "OUTPUT THREAD RECEIVED "
                << bytesRead
                << " BYTES: ["
                << output
                << "]"
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
            break;

        wait(10);
    }
}