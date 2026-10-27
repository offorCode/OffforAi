#include <JuceHeader.h>
#include "MainComponent.h"
#include "AppConfig.h"


class LoadingSplash final : public juce::Component, private juce::Timer
{
public:
    explicit LoadingSplash(std::function<void()> finished)
        : onFinished(std::move(finished)),
          logo(juce::ImageCache::getFromMemory(
              BinaryData::OfforStemSplitter_png,
              BinaryData::OfforStemSplitter_pngSize))
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff111417));
        const auto centre = getLocalBounds().getCentre().toFloat();
        const float size = 116.0f;
        if (!logo.isNull())
            g.drawImage(logo, centre.x - size * 0.5f, centre.y - size * 0.75f,
                        size, size, 0.0f, 0.0f, (float) logo.getWidth(), (float) logo.getHeight());

        const float angle = (float) frame * 0.12f;
        juce::Path arc;
        arc.addCentredArc(centre.x, centre.y + 22.0f, 12.0f, 12.0f,
                          0.0f, angle, angle + juce::MathConstants<float>::pi * 1.45f, true);
        g.setColour(juce::Colour(0xffff7a00));
        g.strokePath(arc, juce::PathStrokeType(2.5f));
        g.setColour(juce::Colours::white.withAlpha(0.75f));
        g.setFont(juce::Font(14.0f));
        g.drawFittedText("OFFOR STEM SPLITTER", getLocalBounds().removeFromBottom(54),
                         juce::Justification::centred, 1);
    }

private:
    void timerCallback() override
    {
        ++frame;
        repaint();
        if (frame >= 48)
        {
            stopTimer();
            auto callback = std::move(onFinished);
            juce::MessageManager::callAsync([callback = std::move(callback)]() mutable
            {
                if (callback)
                    callback();
            });
        }
    }

    std::function<void()> onFinished;
    juce::Image logo;
    int frame = 0;
};

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
                "OFFOR STEM SPLITTER"
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


            juce::Component::SafePointer<MainWindow> safeWindow(this);
            setContentOwned(
                new LoadingSplash([safeWindow]()
                {
                    if (safeWindow != nullptr)
                        safeWindow->showMainComponent();
                }),
                true
            );


            resized();

            setResizable(
                true,
                true
            );

            setResizeLimits(
                1000,
                660,
                1920,
                1200
            );

            centreWithSize(
                1220,
                740
            );

            setVisible(true);
        }


        void showMainComponent()
        {
            setContentOwned(new MainComponent(), true);
            centreWithSize(1220, 740);
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