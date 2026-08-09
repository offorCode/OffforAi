#pragma once

#include <JuceHeader.h>

class StemTrackComponent
    : public juce::Component
{
public:

    // ==========================================================
    // Constructor
    // ==========================================================

    explicit StemTrackComponent(
        const juce::String& stemName
    );

    ~StemTrackComponent() override = default;


    // ==========================================================
    // Component
    // ==========================================================

    void paint(
        juce::Graphics& g
    ) override;

    void resized() override;


    // ==========================================================
    // Solo Selection
    // ==========================================================

    void requestSoloSelectionPlayback()
    {
        if (!isMuted() && isSoloed() && hasSelection())
        {
            if (onSoloSelectionRequested)
            {
                onSoloSelectionRequested(
                    selectionStart,
                    selectionEnd
                );
            }
        }
    }


    // ==========================================================
    // Waveform
    // ==========================================================

    void setAudioFile(
        const juce::File& file
    );

    void clearAudio();


    // ==========================================================
    // Playback / Timeline
    // ==========================================================

    void setPlayheadPosition(
        double positionInSeconds
    );

    double getSelectionStart() const;

    double getSelectionEnd() const;

    bool hasSelection() const;

    juce::File getAudioFile() const;

    double getPlayheadPosition() const
    {
        return playheadPosition;
    }

    double getAudioLength() const
    {
        return thumbnail.getTotalLength();
    }


    // ==========================================================
    // Track Controls
    // ==========================================================

    void setVolume(
        float newVolume
    );

    float getVolume() const
    {
        return volume;
    }

    bool isMuted() const
    {
        return muted;
    }

    bool isSoloed() const
    {
        return soloed;
    }


    // ==========================================================
    // Whole Track Export Selection
    // ==========================================================

    void setTrackSelected(
        bool shouldBeSelected
    );

    bool isTrackSelected() const
    {
        return trackSelected;
    }


    // ==========================================================
    // Callbacks
    // ==========================================================

    std::function<void(double, double)>
        onSoloSelectionRequested;

    std::function<void()>
        onMixingChanged;

    std::function<void()>
        onTrackStateChanged;

    std::function<void()>
        onSelectionChanged;

    std::function<void(double)>
        onSeek;

    std::function<void(bool)>
        onTrackSelectionChanged;


    // ==========================================================
    // Selection
    // ==========================================================

    double getSelectionLength() const
    {
        return selectionEnd - selectionStart;
    }

    void clearSelection();


private:

    // ==========================================================
    // Track Name
    // ==========================================================

    juce::String stemName;


    // ==========================================================
    // UI
    // ==========================================================

    juce::Label nameLabel;


    // Whole-track selection button
    juce::TextButton trackSelectButton
    {
        "□"
    };


    // Mute / Solo
    juce::TextButton muteButton
    {
        "M"
    };

    juce::TextButton soloButton
    {
        "S"
    };


    // ==========================================================
    // Volume Slider
    // ==========================================================

    juce::Slider volumeSlider;


    // ==========================================================
    // Waveform
    // ==========================================================

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache
    {
        5
    };

    juce::AudioThumbnail thumbnail
    {
        512,
        formatManager,
        thumbnailCache
    };

    juce::File audioFile;


    // ==========================================================
    // Timeline
    // ==========================================================

    double playheadPosition = 0.0;


    // ==========================================================
    // Selection
    // ==========================================================

    double selectionStart = 0.0;

    double selectionEnd = 0.0;

    bool selecting = false;

    int selectionMouseStartX = 0;


    // ==========================================================
    // External Drag
    // ==========================================================

    bool externalDragStarted = false;

    bool draggingFromControlArea = false;

    bool draggingExistingSelection = false;


    // Temporary WAV used for partial selection export
    juce::File temporaryDragFile;


    // ==========================================================
    // Track State
    // ==========================================================

    bool muted = false;

    bool soloed = false;

    float volume = 1.0f;

    bool trackSelected = false;


    // ==========================================================
    // Colours
    // ==========================================================

    static constexpr juce::uint32 backgroundColour =
        0xff0d0f11;

    static constexpr juce::uint32 controlColour =
        0xff15181b;

    static constexpr juce::uint32 waveformColour =
        0xffba430d;

    static constexpr juce::uint32 waveformBackgroundColour =
        0xff181b1e;

    static constexpr juce::uint32 borderColour =
        0xff292d31;

    static constexpr juce::uint32 textColour =
        0xffeeeeee;

    static constexpr juce::uint32 secondaryTextColour =
        0xff8c9298;

    static constexpr juce::uint32 buttonColour =
        0xff202428;

    static constexpr juce::uint32 buttonHoverColour =
        0xff292e33;

    static constexpr juce::uint32 buttonActiveColour =
        0xffba430d;

    static constexpr juce::uint32 playheadColour =
        0xffff7a35;


    // ==========================================================
    // Internal Functions
    // ==========================================================

    void updateButtonStates();

    void updateTrackSelectionButton();

    juce::Rectangle<int>
        getWaveformArea() const;


    // ==========================================================
    // Control Area Drag
    // ==========================================================

    void beginControlAreaDrag();


    // ==========================================================
    // Selection Rendering
    // ==========================================================

    bool createSelectionDragFile();

    bool startExternalFileDrag();


    // ==========================================================
    // Mouse
    // ==========================================================

    void mouseDown(
        const juce::MouseEvent& event
    ) override;

    void mouseDrag(
        const juce::MouseEvent& event
    ) override;

    void mouseUp(
        const juce::MouseEvent& event
    ) override;


    // ==========================================================
    // JUCE
    // ==========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        StemTrackComponent
    )
};
