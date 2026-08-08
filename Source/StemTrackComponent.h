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
    // Mixing Callback
    // ==========================================================

    std::function<void()> onMixingChanged;


    // ==========================================================
    // Track State Callback
    // ==========================================================

    std::function<void()> onTrackStateChanged;


    // ==========================================================
    // Selection Callback
    // ==========================================================

    std::function<void()> onSelectionChanged;


    // ==========================================================
    // Seek Callback
    // ==========================================================

    std::function<void(double)> onSeek;


    // ==========================================================
    // Selection
    // ==========================================================

    bool hasSelection() const
    {
        return selectionEnd > selectionStart;
    }

    double getSelectionStart() const
    {
        return selectionStart;
    }

    double getSelectionEnd() const
    {
        return selectionEnd;
    }

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
    // Track State
    // ==========================================================

    bool muted = false;

    bool soloed = false;

    float volume = 1.0f;


    // ==========================================================
    // Colours
    // ==========================================================

    static constexpr juce::uint32 accentColour =
        0xffba430d;


    static constexpr juce::uint32 playheadColour =
        0xffba430d;


    // ==========================================================
    // Internal Functions
    // ==========================================================

    void updateButtonStates();


    juce::Rectangle<int> getWaveformArea() const;


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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        StemTrackComponent
    )
};