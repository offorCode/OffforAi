#include <JuceHeader.h>
#include "MainComponent.h"
#include "AppConfig.h"


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
        return OfforStemSplitter::VERSION;
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

            setUsingNativeTitleBar(false);

            setContentOwned(
                new MainComponent(),
                true
            );

            setResizable(
                true,
                true
            );

            setResizeLimits(
                600,
                280,
                1000,
                900
            );

            centreWithSize(
                620,
                400
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