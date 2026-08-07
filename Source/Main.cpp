#include <JuceHeader.h>
#include "MainComponent.h"


class OfforStemSplitterApplication 
    : public juce::JUCEApplication
{
public:

    const juce::String getApplicationName() override
    {
        return "Offor Stem Splitter";
    }


    const juce::String getApplicationVersion() override
    {
        return "0.1.0";
    }


    void initialise(
        const juce::String&) override
    {
        mainWindow.reset(
            new MainWindow(
                getApplicationName()
            )
        );
    }


    void shutdown() override
    {
        mainWindow = nullptr;
    }


    class MainWindow 
        : public juce::DocumentWindow
    {

    public:

        MainWindow(
            juce::String name
        )
        :
        DocumentWindow(
            name,
            juce::Colours::black,
            DocumentWindow::allButtons
        )
        {

            setUsingNativeTitleBar(true);

            setContentOwned(
                new MainComponent(),
                true
            );

            centreWithSize(
                700,
                450
            );

            setVisible(true);
        }


        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()
            ->systemRequestedQuit();
        }

    };


private:

    std::unique_ptr<MainWindow> mainWindow;
};


START_JUCE_APPLICATION(
    OfforStemSplitterApplication
)