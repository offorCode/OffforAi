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


    // ========================================================
    // CALLBACKS
    // ========================================================

    std::function<void()> onClose;

    std::function<void(
        bool autoPlay,
        bool loopSelection,
        int exportFormat,
        bool normalize
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
        "Normalize"
    };


    // ========================================================
    // CLOSE
    // ========================================================

    juce::TextButton closeButton {
        "CLOSE"
    };


    // ========================================================
    // NON-COPYABLE
    // ========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        SettingsComponent
    );
};