#pragma once

#include <JuceHeader.h>

class StemTrackComponent
    : public juce::Component
{
public:

    explicit StemTrackComponent(
        const juce::String& stemName
    );

    ~StemTrackComponent() override = default;


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
    // Track controls
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
    // MainComponent notification
    // ==========================================================

    std::function<void()> onTrackStateChanged;

private:

    // ==========================================================
    // Track name
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
    // Volume
    // ==========================================================

    juce::Slider volumeSlider;

    juce::Label volumeLabel;

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
    // State
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
    // Internal functions
    // ==========================================================

    void updateButtonStates();


    juce::Rectangle<int> getWaveformArea() const;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        StemTrackComponent
    )
};