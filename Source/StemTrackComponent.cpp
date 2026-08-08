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

        if (onMixingChanged)
            onMixingChanged();
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

        if (onMixingChanged)
            onMixingChanged();
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
        juce::Slider::NoTextBox,
        false,
        0,
        0
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

        if (onMixingChanged)
            onMixingChanged();
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
    // Track Background
    // ==========================================================

    g.fillAll(
        juce::Colour(0xff111111)
    );


    // ==========================================================
    // Track Border
    // ==========================================================

    g.setColour(
        juce::Colour(accentColour)
    );

    g.drawRect(
        getLocalBounds(),
        1
    );


    // ==========================================================
    // Waveform Area
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


        // ======================================================
        // Selection
        // ======================================================

        if (hasSelection())
        {
            const double totalLength =
                thumbnail.getTotalLength();


            const double startRatio =
                selectionStart / totalLength;


            const double endRatio =
                selectionEnd / totalLength;


            const int startX =
                waveformArea.getX()
                + static_cast<int>(
                    startRatio
                    * waveformArea.getWidth()
                );


            const int endX =
                waveformArea.getX()
                + static_cast<int>(
                    endRatio
                    * waveformArea.getWidth()
                );


            auto selectionArea =
                waveformArea
                    .withLeft(startX)
                    .withRight(endX);


            // Selection background
            g.setColour(
                juce::Colour(0x55ba430d)
            );

            g.fillRect(
                selectionArea
            );


            // Selection borders
            g.setColour(
                juce::Colour(accentColour)
            );

            g.drawVerticalLine(
                startX,
                static_cast<float>(
                    waveformArea.getY()
                ),
                static_cast<float>(
                    waveformArea.getBottom()
                )
            );

            g.drawVerticalLine(
                endX,
                static_cast<float>(
                    waveformArea.getY()
                ),
                static_cast<float>(
                    waveformArea.getBottom()
                )
            );
        }
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
    // Center Line
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
        const double totalLength =
            thumbnail.getTotalLength();


        double normalizedPosition =
            playheadPosition / totalLength;


        normalizedPosition =
            juce::jlimit(
                0.0,
                1.0,
                normalizedPosition
            );


        const auto playheadX =
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
            .reduced(6);


    // ==========================================================
    // Controls
    // ==========================================================

    auto controls =
        area.removeFromLeft(155);


    // ==========================================================
    // Track Name
    // ==========================================================

    nameLabel.setBounds(
        controls.removeFromTop(22)
    );


    // ==========================================================
    // M / S
    // ==========================================================

    auto buttonArea =
        controls.removeFromTop(28);


    muteButton.setBounds(
        buttonArea
            .removeFromLeft(48)
            .reduced(2)
    );


    soloButton.setBounds(
        buttonArea
            .removeFromLeft(48)
            .reduced(2)
    );


    // ==========================================================
    // Volume
    // ==========================================================

    controls.removeFromTop(2);


    volumeSlider.setBounds(
        controls.removeFromTop(24)
    );
}


// ==========================================================
// Mouse Down
// ==========================================================

void StemTrackComponent::mouseDown(
    const juce::MouseEvent& event
)
{
    auto waveformArea =
        getWaveformArea();


    // ==========================================================
    // Only interact inside waveform
    // ==========================================================

    if (!waveformArea.contains(
            event.getPosition()))
    {
        return;
    }


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    // ==========================================================
    // Start selection
    // ==========================================================

    selectionMouseStartX =
        event.getPosition().x;


    selecting = true;


    // ==========================================================
    // Calculate start position
    // ==========================================================

    const double normalizedPosition =
        juce::jlimit(
            0.0,
            1.0,
            static_cast<double>(
                event.position.x
                - waveformArea.getX()
            )
            / static_cast<double>(
                waveformArea.getWidth()
            )
        );


    selectionStart =
        normalizedPosition
        * totalLength;


    selectionEnd =
        selectionStart;


    // ==========================================================
    // Move playhead immediately
    // ==========================================================

    setPlayheadPosition(
        selectionStart
    );


    repaint();
}


// ==========================================================
// Mouse Drag
// ==========================================================

void StemTrackComponent::mouseDrag(
    const juce::MouseEvent& event
)
{
    if (!selecting)
        return;


    auto waveformArea =
        getWaveformArea();


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    // ==========================================================
    // Clamp mouse position
    // ==========================================================

    const double startX =
        juce::jlimit(
            static_cast<double>(
                waveformArea.getX()
            ),
            static_cast<double>(
                waveformArea.getRight()
            ),
            static_cast<double>(
                selectionMouseStartX
            )
        );


    const double currentX =
        juce::jlimit(
            static_cast<double>(
                waveformArea.getX()
            ),
            static_cast<double>(
                waveformArea.getRight()
            ),
            static_cast<double>(
                event.position.x
            )
        );


    // ==========================================================
    // Convert X -> time
    // ==========================================================

    const double startRatio =
        (startX - waveformArea.getX())
        / static_cast<double>(
            waveformArea.getWidth()
        );


    const double currentRatio =
        (currentX - waveformArea.getX())
        / static_cast<double>(
            waveformArea.getWidth()
        );


    const double startTime =
        startRatio * totalLength;


    const double currentTime =
        currentRatio * totalLength;


    // ==========================================================
    // Selection direction
    // ==========================================================

    if (currentTime >= startTime)
    {
        selectionStart =
            startTime;

        selectionEnd =
            currentTime;
    }
    else
    {
        selectionStart =
            currentTime;

        selectionEnd =
            startTime;
    }


    // ==========================================================
    // Move playhead
    // ==========================================================

    setPlayheadPosition(
        currentTime
    );


    repaint();
}


// ==========================================================
// Mouse Up
// ==========================================================

void StemTrackComponent::mouseUp(
    const juce::MouseEvent&)
{
    if (!selecting)
        return;


    selecting = false;


    // ==========================================================
    // Very small selection = normal seek
    // ==========================================================

    if (selectionEnd - selectionStart < 0.01)
    {
        const double seekPosition =
            selectionStart;


        selectionStart = 0.0;
        selectionEnd = 0.0;


        setPlayheadPosition(
            seekPosition
        );


        if (onSeek)
            onSeek(
                seekPosition
            );
    }
    else
    {
        // ======================================================
        // Real selection
        // ======================================================

        if (onSelectionChanged)
            onSelectionChanged();
    }


    repaint();
}


// ==========================================================
// Clear Selection
// ==========================================================

void StemTrackComponent::clearSelection()
{
    selectionStart = 0.0;

    selectionEnd = 0.0;

    selecting = false;

    repaint();


    if (onSelectionChanged)
        onSelectionChanged();
}


// ==========================================================
// Waveform Area
// ==========================================================

juce::Rectangle<int>
StemTrackComponent::getWaveformArea() const
{
    return getLocalBounds()
        .reduced(6)
        .withTrimmedLeft(165)
        .withTrimmedRight(6);
}


// ==========================================================
// Set Audio File
// ==========================================================

void StemTrackComponent::setAudioFile(
    const juce::File& file
)
{
    audioFile = file;


    playheadPosition = 0.0;


    selectionStart = 0.0;

    selectionEnd = 0.0;

    selecting = false;


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
// Clear Audio
// ==========================================================

void StemTrackComponent::clearAudio()
{
    audioFile =
        juce::File();


    playheadPosition =
        0.0;


    selectionStart =
        0.0;


    selectionEnd =
        0.0;


    selecting =
        false;


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


    if (onMixingChanged)
        onMixingChanged();
}


// ==========================================================
// Button States
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