#pragma once

#include <JuceHeader.h>

// ============================================================
// OFFOR STEM SPLITTER
// Settings Component
// ============================================================

class SettingsComponent
    : public juce::Component
{
public:

    // ========================================================
    // Constructor / Destructor
    // ========================================================

    SettingsComponent();
    ~SettingsComponent() override;


    // ========================================================
    // JUCE Component
    // ========================================================

    void paint(
        juce::Graphics& g
    ) override;

    void resized() override;
    void setEmbeddedMode(bool embedded);


    // ========================================================
    // SETTINGS
    // ========================================================

    bool getAutoPlayEnabled() const;
    void setAutoPlayEnabled(
        bool enabled
    );

    bool getLoopSelectionEnabled() const;
    void setLoopSelectionEnabled(
        bool enabled
    );


    int getExportFormat() const;
    void setExportFormat(
        int format
    );

    bool getNormalizeEnabled() const;
    void setNormalizeEnabled(
        bool enabled
    );

    void setSixStemModeEnabled(bool enabled);

    // ========================================================
    // LICENSE
    // ========================================================

    void setLicenseActivated(
        bool activated
    );


    // ========================================================
    // CALLBACKS
    // ========================================================

    std::function<void()> onClose;

    std::function<void()> onActivateLicense;

    std::function<void(
        bool autoPlay,
        bool loopSelection,
        int exportFormat,
        bool normalize,
        bool sixStemMode
    )> onSettingsChanged;


private:

    // ========================================================
    // TITLE
    // ========================================================

    juce::Label titleLabel;


    // ========================================================
    // PLAYBACK
    // ========================================================

    juce::Label playbackLabel;

    juce::ToggleButton autoPlayButton {
        "Auto-play"
    };

    juce::ToggleButton loopSelectionButton {
        "Loop Selection"
    };


    // ========================================================
    // EXPORT
    // ========================================================

    juce::Label exportLabel;

    juce::ComboBox exportFormatBox;

    juce::ToggleButton normalizeButton {
        "Normalize exports"
    };

    juce::ToggleButton sixStemModeButton {
        "Split guitar + piano (6-stem)"
    };


    // ========================================================
    // CLOSE
    // ========================================================

    juce::TextButton closeButton {
        "CLOSE"
    };
    bool embeddedMode = false;

    // ========================================================
    // LICENSE
    // ========================================================

    juce::Label licenseLabel;

    juce::TextButton activateButton {
        "ACTIVATE"
    };

    


    // ========================================================
    // NON-COPYABLE
    // ========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        SettingsComponent
    );
};