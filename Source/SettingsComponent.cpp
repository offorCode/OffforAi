#include "SettingsComponent.h"

// ==========================================================
// Constructor
// ==========================================================

SettingsComponent::SettingsComponent()
{
    // ======================================================
    // TITLE
    // ======================================================

    titleLabel.setText(
        "SETTINGS",
        juce::dontSendNotification
    );

    titleLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(22.0f)
                .withStyle("Bold")
        )
    );

    titleLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    addAndMakeVisible(
        titleLabel
    );


    // ======================================================
    // PLAYBACK
    // ======================================================

    playbackLabel.setText(
        "PLAYBACK",
        juce::dontSendNotification
    );

    playbackLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(12.0f)
                .withStyle("Bold")
        )
    );

    playbackLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white.withAlpha(0.55f)
    );

    addAndMakeVisible(
        playbackLabel
    );


    addAndMakeVisible(
        autoPlayButton
    );

    addAndMakeVisible(
        loopSelectionButton
    );


    // ======================================================
    // EXPORT
    // ======================================================

    exportLabel.setText(
        "EXPORT",
        juce::dontSendNotification
    );

    exportLabel.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(12.0f)
                .withStyle("Bold")
        )
    );

    exportLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white.withAlpha(0.55f)
    );

    addAndMakeVisible(
        exportLabel
    );


    exportFormatBox.addItem(
        "WAV",
        1
    );

    exportFormatBox.addItem(
        "FLAC",
        2
    );

    exportFormatBox.addItem(
        "AIFF",
        3
    );

    exportFormatBox.setSelectedId(
        1,
        juce::dontSendNotification
    );

    addAndMakeVisible(
        exportFormatBox
    );


    addAndMakeVisible(
        normalizeButton
    );


    sixStemModeButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(sixStemModeButton);


    // ======================================================
    // CLOSE
    // ======================================================

    closeButton.onClick = [this]
    {
        if (onClose)
            onClose();
    };

    addAndMakeVisible(
        closeButton
    );

    // ========================================================
    // LICENSE
    // ========================================================

    licenseLabel.setText(
        "LICENSE",
        juce::dontSendNotification
    );

    licenseLabel.setFont(
        juce::Font(15.0f)
    );

    licenseLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    addAndMakeVisible(
        licenseLabel
    );


    activateButton.setButtonText(
        "ACTIVATE"
    );

    activateButton.onClick = [this]
    {
        if (onActivateLicense)
        {
            onActivateLicense();
        }
    };

    addAndMakeVisible(
        activateButton
    );

    // ======================================================
    // SETTINGS CALLBACKS
    // ======================================================

    auto notifySettingsChanged = [this]
    {
        if (onSettingsChanged)
        {
            onSettingsChanged(
                autoPlayButton.getToggleState(),
                loopSelectionButton.getToggleState(),
                exportFormatBox.getSelectedId(),
                normalizeButton.getToggleState(),
                sixStemModeButton.getToggleState()
            );
        }
    };


    autoPlayButton.onClick =
        notifySettingsChanged;

    loopSelectionButton.onClick =
        notifySettingsChanged;

    exportFormatBox.onChange =
        notifySettingsChanged;

    normalizeButton.onClick =
        notifySettingsChanged;

    sixStemModeButton.onClick =
        notifySettingsChanged;
}


// ==========================================================
// Destructor
// ==========================================================

SettingsComponent::~SettingsComponent()
{
}


// ==========================================================
// Paint
// ==========================================================

void SettingsComponent::paint(
    juce::Graphics& g
)
{
    g.fillAll(
        juce::Colour(0xff111417)
    );

    auto bounds =
        getLocalBounds()
            .toFloat()
            .reduced(1.0f);

    g.setColour(
        juce::Colour(0xff2a3035)
    );

    g.drawRoundedRectangle(
        bounds,
        10.0f,
        1.0f
    );
}


// ==========================================================
// Resized
// ==========================================================

void SettingsComponent::resized()
{
    auto area = getLocalBounds().reduced(16, 14);

    titleLabel.setBounds(area.removeFromTop(34));
    area.removeFromTop(8);

    playbackLabel.setBounds(area.removeFromTop(20));
    autoPlayButton.setBounds(area.removeFromTop(30));
    loopSelectionButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(12);

    exportLabel.setBounds(area.removeFromTop(20));
    exportFormatBox.setBounds(area.removeFromTop(32));
    area.removeFromTop(4);
    normalizeButton.setBounds(area.removeFromTop(30));
    sixStemModeButton.setBounds(area.removeFromTop(36));
    area.removeFromTop(12);

    licenseLabel.setBounds(area.removeFromTop(22));
    activateButton.setBounds(area.removeFromTop(38));

    if (embeddedMode)
    {
        closeButton.setVisible(false);
    }
    else
    {
        closeButton.setVisible(true);
        closeButton.setBounds(area.removeFromBottom(40));
    }
}

void SettingsComponent::setEmbeddedMode(bool embedded)
{
    embeddedMode = embedded;
    closeButton.setVisible(!embeddedMode);
    resized();
}


// ==========================================================
// AUTO PLAY
// ==========================================================

bool SettingsComponent::getAutoPlayEnabled() const
{
    return autoPlayButton.getToggleState();
}

void SettingsComponent::setAutoPlayEnabled(
    bool enabled
)
{
    autoPlayButton.setToggleState(
        enabled,
        juce::dontSendNotification
    );
}


// ==========================================================
// LOOP
// ==========================================================

bool SettingsComponent::getLoopSelectionEnabled() const
{
    return loopSelectionButton.getToggleState();
}

void SettingsComponent::setLoopSelectionEnabled(
    bool enabled
)
{
    loopSelectionButton.setToggleState(
        enabled,
        juce::dontSendNotification
    );
}


// ==========================================================
// EXPORT FORMAT
// ==========================================================

int SettingsComponent::getExportFormat() const
{
    return exportFormatBox.getSelectedId();
}

void SettingsComponent::setExportFormat(
    int format
)
{
    exportFormatBox.setSelectedId(
        format,
        juce::dontSendNotification
    );
}


// ==========================================================
// NORMALIZE
// ==========================================================

bool SettingsComponent::getNormalizeEnabled() const
{
    return normalizeButton.getToggleState();
}

void SettingsComponent::setNormalizeEnabled(
    bool enabled
)
{
    normalizeButton.setToggleState(
        enabled,
        juce::dontSendNotification
    );
}

void SettingsComponent::setSixStemModeEnabled(bool enabled)
{
    sixStemModeButton.setToggleState(enabled, juce::dontSendNotification);
}

// ============================================================
// LICENSE STATE
// ============================================================

void SettingsComponent::setLicenseActivated(
    bool activated
)
{
    if (activated)
    {
        activateButton.setButtonText(
            "Activated"
        );

        activateButton.setEnabled(
            false
        );
    }
    else
    {
        activateButton.setButtonText(
            "ACTIVATE"
        );

        activateButton.setEnabled(
            true
        );
    }

    activateButton.repaint();
}