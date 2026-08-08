#pragma once

#include <JuceHeader.h>
#include "StemTrackComponent.h"

class MainComponent
    : public juce::Component,
      private juce::Timer
{
public:

    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    void expandForTracks();

    // ==========================================================
    // Transport
    // ==========================================================

    void startPlayback();

    void pausePlayback();

    void stopPlayback();

    void updatePlayback();



    juce::TextButton playButton
    {
        "PLAY"
    };


    juce::TextButton pauseButton
    {
        "PAUSE"
    };


    juce::TextButton stopButton
    {
        "STOP"
    };


    double playbackPosition = 0.0;

    double playbackLength = 0.0;

    bool isPlaying = false;


    // ==========================================================
    // Audio Playback Engine
    // ==========================================================

    juce::AudioDeviceManager audioDeviceManager;

    juce::AudioSourcePlayer audioSourcePlayer;

    juce::MixerAudioSource mixerSource;


    std::unique_ptr<juce::AudioFormatReaderSource>
        vocalsReaderSource;

    std::unique_ptr<juce::AudioFormatReaderSource>
        drumsReaderSource;

    std::unique_ptr<juce::AudioFormatReaderSource>
        bassReaderSource;

    std::unique_ptr<juce::AudioFormatReaderSource>
        instrumentalReaderSource;


    std::unique_ptr<juce::AudioTransportSource>
        vocalsTransport;

    std::unique_ptr<juce::AudioTransportSource>
        drumsTransport;

    std::unique_ptr<juce::AudioTransportSource>
        bassTransport;

    std::unique_ptr<juce::AudioTransportSource>
        instrumentalTransport;


    // ==========================================================
    // Playback Timer
    // ==========================================================

    class PlaybackTimer
        : public juce::Timer
    {
    public:

        explicit PlaybackTimer(
            MainComponent& owner
        )
            : owner(owner)
        {
        }


        void timerCallback() override
        {
            owner.updatePlayback();
        }


    private:

        MainComponent& owner;
    };


    PlaybackTimer playbackTimer
    {
        *this
    };


    // ==========================================================
    // UI
    // ==========================================================

    juce::Label titleLabel;


    juce::TextButton selectButton
    {
        "Select Audio File"
    };


    juce::TextButton separateButton
    {
        "SEPARATE"
    };


    juce::TextButton openOutputButton
    {
        "OPEN OUTPUT FOLDER"
    };

    juce::TextButton exportSelectionButton
    {
        "EXPORT SELECTION"
    };


    juce::Label fileLabel;

    juce::Label statusLabel;


    // ==========================================================
    // Stem Labels
    // ==========================================================

    juce::Label vocalsLabel;

    juce::Label drumsLabel;

    juce::Label bassLabel;

    juce::Label instrumentalLabel;


    // ==========================================================
    // Stem Tracks
    // ==========================================================

    StemTrackComponent vocalsTrack
    {
        "VOCALS"
    };


    StemTrackComponent drumsTrack
    {
        "DRUMS"
    };


    StemTrackComponent bassTrack
    {
        "BASS"
    };


    StemTrackComponent instrumentalTrack
    {
        "INSTRUMENTAL"
    };


    // ==========================================================
    // Audio Files
    // ==========================================================

    juce::File selectedFile;

    juce::File outputFolder;


    juce::File vocalsFile;

    juce::File drumsFile;

    juce::File bassFile;

    juce::File instrumentalFile;


    // ==========================================================
    // Audio Format Manager
    // ==========================================================

    juce::AudioFormatManager formatManager;


    // ==========================================================
    // Python / Demucs
    // ==========================================================

    std::unique_ptr<juce::ChildProcess>
        separatorProcess;


    // ==========================================================
    // Functions
    // ==========================================================

    void selectAudioFile();

    void runSeparator();

    void checkSeparatorProcess();

    void showSeparatedStems();

    void clearSeparatedStems();

    void openOutputFolder();

    void exportSelectedAudio();

    void setStatus(
        const juce::String& message
    );


    // ==========================================================
    // Audio Functions
    // ==========================================================

    bool loadStemAudio();

    void unloadStemAudio();

    
    // ==========================================================
    // Track Mixing
    // ==========================================================

    void updateTrackMixing();



    // ==========================================================
    // Timer
    // ==========================================================

    void timerCallback() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MainComponent
    )
};