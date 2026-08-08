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

    //==============================================================
    // Separation Thread
    //==============================================================

    class SeparationThread : public juce::Thread
    {
    public:

        SeparationThread(
            MainComponent& owner,
            const juce::File& audioFile
        );

        ~SeparationThread() override;


        void run() override;


    private:

        MainComponent& owner;
        juce::File audioFile;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            SeparationThread
        );
    };


    //==============================================================
    // Functions
    //==============================================================

    void selectAudioFile();

    void runSeparator();

    void separationFinished(
        bool success,
        const juce::String& message
    );


    juce::File getProjectRoot() const;

    juce::File getPythonExecutable() const;

    juce::File getSeparatorScript() const;


    //==============================================================
    // UI
    //==============================================================

    juce::Label titleLabel;

    juce::Label versionLabel;

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


    //==============================================================
    // State
    //==============================================================

    juce::File selectedFile;

    std::unique_ptr<SeparationThread> separationThread;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MainComponent
    );
};

