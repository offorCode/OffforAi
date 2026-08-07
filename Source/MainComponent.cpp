#include "MainComponent.h"


MainComponent::MainComponent()
{
    setSize(
        700,
        450
    );


    titleLabel.setText(
        "OFFOR STEM SPLITTER",
        juce::dontSendNotification
    );

    titleLabel.setFont(
        juce::Font(28.0f)
    );

    titleLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(titleLabel);



    addAndMakeVisible(selectButton);

    selectButton.onClick = [this]
    {
        selectAudioFile();
    };



    addAndMakeVisible(separateButton);

    separateButton.onClick = [this]
    {
        runSeparator();
    };



    fileLabel.setText(
        "No file selected",
        juce::dontSendNotification
    );

    fileLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(fileLabel);



    statusLabel.setText(
        "Status: Waiting...",
        juce::dontSendNotification
    );

    statusLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(statusLabel);
}



MainComponent::~MainComponent()
{
}



void MainComponent::paint(
    juce::Graphics& g
)
{
    g.fillAll(
        juce::Colours::black
    );


    g.setColour(
        juce::Colours::white
    );
}



void MainComponent::resized()
{

    auto area = getLocalBounds()
        .reduced(40);


    titleLabel.setBounds(
        area.removeFromTop(60)
    );


    area.removeFromTop(30);


    selectButton.setBounds(
        area.removeFromTop(40)
        .withSizeKeepingCentre(
            220,
            40
        )
    );


    area.removeFromTop(20);


    fileLabel.setBounds(
        area.removeFromTop(40)
    );


    area.removeFromTop(20);


    separateButton.setBounds(
        area.removeFromTop(50)
        .withSizeKeepingCentre(
            180,
            45
        )
    );


    area.removeFromTop(30);


    statusLabel.setBounds(
        area.removeFromTop(40)
    );
}



void MainComponent::selectAudioFile()
{

    auto chooser = std::make_shared
    <juce::FileChooser>(
        "Select audio file...",
        juce::File{},
        "*.mp3;*.wav;*.flac"
    );


    chooser->launchAsync(
        juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles,

        [this, chooser]
        (const juce::FileChooser&)
        {

            auto file = chooser->getResult();


            if (file.existsAsFile())
            {
                selectedFile = file;


                fileLabel.setText(
                    file.getFileName(),
                    juce::dontSendNotification
                );


                statusLabel.setText(
                    "Status: Ready",
                    juce::dontSendNotification
                );
            }

        }
    );
}



void MainComponent::runSeparator()
{

    if (!selectedFile.existsAsFile())
    {
        statusLabel.setText(
            "Please select an audio file first",
            juce::dontSendNotification
        );

        return;
    }


    statusLabel.setText(
        "Status: Separating...",
        juce::dontSendNotification
    );


    // Python bridge will be added here next

}