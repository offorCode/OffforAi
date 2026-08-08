#include "StemTrackComponent.h"

// ==========================================================
// Constructor
// ==========================================================

StemTrackComponent::StemTrackComponent(
    const juce::String& name
)
    : stemName(name)
{
    // ==========================================================
    // Audio
    // ==========================================================

    formatManager.registerBasicFormats();


    // ==========================================================
    // Name
    // ==========================================================

    nameLabel.setText(
        stemName,
        juce::dontSendNotification
    );

    nameLabel.setFont(
        juce::Font(15.0f)
    );

    nameLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    nameLabel.setJustificationType(
        juce::Justification::centredLeft
    );

    addAndMakeVisible(
        nameLabel
    );


    // ==========================================================
    // Mute
    // ==========================================================

    muteButton.setClickingTogglesState(
        true
    );

    muteButton.onClick = [this]
    {
        muted =
            muteButton.getToggleState();

        updateButtonStates();

        if (onTrackStateChanged)
            onTrackStateChanged();
    };

    addAndMakeVisible(
        muteButton
    );


    // ==========================================================
    // Solo
    // ==========================================================

    soloButton.setClickingTogglesState(
        true
    );

    soloButton.onClick = [this]
    {
        soloed =
            soloButton.getToggleState();

        updateButtonStates();

        if (onTrackStateChanged)
            onTrackStateChanged();
    };

    addAndMakeVisible(
        soloButton
    );

    // ==========================================================
    // Volume Slider
    // ==========================================================

    volumeSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    volumeSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        55,
        22
    );

    volumeSlider.setRange(
        0.0,
        1.0,
        0.01
    );

    volumeSlider.setValue(
        1.0,
        juce::dontSendNotification
    );

    volumeSlider.setDoubleClickReturnValue(
        true,
        1.0
    );

    volumeSlider.onValueChange = [this]
    {
        volume =
            static_cast<float>(
                volumeSlider.getValue()
            );

        if (onTrackStateChanged)
            onTrackStateChanged();
    };

    addAndMakeVisible(
        volumeSlider
    );
}


// ==========================================================
// Paint
// ==========================================================

void StemTrackComponent::paint(
    juce::Graphics& g
)
{
    // ==========================================================
    // Track background
    // ==========================================================

    g.fillAll(
        juce::Colour(0xff111111)
    );


    // ==========================================================
    // Track border
    // ==========================================================

    g.setColour(
        juce::Colour(accentColour)
    );


    g.drawRect(
        getLocalBounds(),
        1
    );


    // ==========================================================
    // Waveform area
    // ==========================================================

    auto waveformArea =
        getWaveformArea();


    g.setColour(
        juce::Colour(0xff181818)
    );


    g.fillRect(
        waveformArea
    );


    // ==========================================================
    // Waveform
    // ==========================================================

    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(
            juce::Colour(accentColour)
        );


        thumbnail.drawChannels(
            g,
            waveformArea,
            0.0,
            thumbnail.getTotalLength(),
            1.0f
        );
    }
    else
    {
        g.setColour(
            juce::Colours::grey
        );


        g.drawText(
            "No audio loaded",
            waveformArea,
            juce::Justification::centred
        );
    }


    // ==========================================================
    // Center line
    // ==========================================================

    g.setColour(
        juce::Colour(0xff333333)
    );

    auto centerY =
        waveformArea.getCentreY();

    g.drawHorizontalLine(
        centerY,
        static_cast<float>(
            waveformArea.getX()
        ),
        static_cast<float>(
            waveformArea.getRight()
        )
    );


   

    // ==========================================================
    // Playhead
    // ==========================================================

    if (thumbnail.getTotalLength() > 0.0)
    {
        auto totalLength =
            thumbnail.getTotalLength();


        auto normalizedPosition =
            playheadPosition / totalLength;


        normalizedPosition =
            juce::jlimit(
                0.0,
                1.0,
                normalizedPosition
            );


        auto playheadX =
            waveformArea.getX()
            + normalizedPosition
              * waveformArea.getWidth();


        g.setColour(
            juce::Colour(playheadColour)
        );


        g.drawVerticalLine(
            static_cast<int>(playheadX),
            static_cast<float>(
                waveformArea.getY()
            ),
            static_cast<float>(
                waveformArea.getBottom()
            )
        );
    }
}


// ==========================================================
// Resized
// ==========================================================

void StemTrackComponent::resized()
{
    auto area =
        getLocalBounds()
        .reduced(10);


    // ==========================================================
    // Controls
    // ==========================================================

    auto controls =
        area.removeFromLeft(245);


    // ==========================================================
    // Track name
    // ==========================================================

    nameLabel.setBounds(
        controls.removeFromTop(24)
    );


    // ==========================================================
    // M / S buttons
    // ==========================================================

    auto buttonArea =
        controls.removeFromTop(30);


    muteButton.setBounds(
        buttonArea
            .removeFromLeft(45)
            .reduced(2)
    );


    soloButton.setBounds(
        buttonArea
            .removeFromLeft(45)
            .reduced(2)
    );


    // ==========================================================
    // Volume slider
    // ==========================================================

    volumeSlider.setBounds(
        controls.removeFromTop(28)
            .reduced(2)
    );
}


// ==========================================================
// Waveform area
// ==========================================================

juce::Rectangle<int>
StemTrackComponent::getWaveformArea() const
{
    return getLocalBounds()
        .reduced(10)
        .withTrimmedLeft(255)
        .withTrimmedRight(10);
}


    // ==========================================================
    // Set audio file
    // ==========================================================

    void StemTrackComponent::setAudioFile(
    const juce::File& file
)
{
    audioFile = file;

    playheadPosition = 0.0;

    thumbnail.clear();


    if (!audioFile.existsAsFile())
    {
        repaint();
        return;
    }


    // ==========================================================
    // Load audio into thumbnail
    // ==========================================================

    thumbnail.setSource(
        new juce::FileInputSource(
            audioFile
        )
    );


    // ==========================================================
    // Reset playhead
    // ==========================================================

    playheadPosition = 0.0;


    repaint();
}


// ==========================================================
// Clear audio
// ==========================================================

void StemTrackComponent::clearAudio()
{
    audioFile = juce::File();

    playheadPosition = 0.0;

    thumbnail.clear();

    repaint();
}


// ==========================================================
// Playback
// ==========================================================

void StemTrackComponent::setPlayheadPosition(
    double positionInSeconds
)
{
    playheadPosition =
        juce::jmax(
            0.0,
            positionInSeconds
        );

    repaint();
}


// ==========================================================
// Volume
// ==========================================================


void StemTrackComponent::setVolume(
    float newVolume
)
{
    volume =
        juce::jlimit(
            0.0f,
            1.0f,
            newVolume
        );


    volumeSlider.setValue(
        volume,
        juce::dontSendNotification
    );
}


// ==========================================================
// Button states
// ==========================================================

void StemTrackComponent::updateButtonStates()
{
    muteButton.setToggleState(
        muted,
        juce::dontSendNotification
    );


    soloButton.setToggleState(
        soloed,
        juce::dontSendNotification
    );


    repaint();
}
