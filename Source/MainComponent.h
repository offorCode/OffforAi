#pragma once

#include <JuceHeader.h>


class MainComponent : public juce::Component
{
public:

    MainComponent();
    ~MainComponent() override;


    void paint(juce::Graphics&) override;

    void resized() override;


private:

    void selectAudioFile();

    void runSeparator();


    juce::Label titleLabel;

    juce::TextButton selectButton
    {
        "Select Audio File"
    };

    juce::TextButton separateButton
    {
        "SEPARATE"
    };


    juce::Label fileLabel;

    juce::Label statusLabel;


    juce::File selectedFile;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};