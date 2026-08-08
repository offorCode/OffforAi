#pragma once

#include <JuceHeader.h>

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

    juce::Label fileLabel;
    juce::Label statusLabel;


    // ==========================================================
    // Stem labels
    // ==========================================================

    juce::Label vocalsLabel;
    juce::Label drumsLabel;
    juce::Label bassLabel;
    juce::Label instrumentalLabel;


    // ==========================================================
    // Audio
    // ==========================================================

    juce::File selectedFile;
    juce::File outputFolder;


    // ==========================================================
    // Python / Demucs
    // ==========================================================

    std::unique_ptr<juce::ChildProcess> separatorProcess;


    // ==========================================================
    // Functions
    // ==========================================================

    void selectAudioFile();

    void runSeparator();

    void checkSeparatorProcess();

    void showSeparatedStems();

    void clearSeparatedStems();

    void openOutputFolder();

    void setStatus(
        const juce::String& message
    );


    // ==========================================================
    // Timer
    // ==========================================================

    void timerCallback() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MainComponent
    )
};
