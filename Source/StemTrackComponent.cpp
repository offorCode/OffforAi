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


            // --------------------------------------------------
            // Selection background
            // --------------------------------------------------

            g.setColour(
                juce::Colour(0x66ba430d)
            );

            g.fillRect(
                selectionArea
            );


            // --------------------------------------------------
            // Selection border
            // --------------------------------------------------

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


            // --------------------------------------------------
            // Selection handles
            // --------------------------------------------------

            g.fillRect(
                startX - 2,
                waveformArea.getY(),
                4,
                waveformArea.getHeight()
            );

            g.fillRect(
                endX - 2,
                waveformArea.getY(),
                4,
                waveformArea.getHeight()
            );


            // --------------------------------------------------
            // Selection duration
            // --------------------------------------------------

            const double length =
                selectionEnd - selectionStart;

            g.setColour(
                juce::Colours::white
            );

            g.setFont(
                juce::Font(11.0f)
            );

            g.drawText(
                juce::String(
                    selectionStart,
                    2
                )
                + "s - "
                + juce::String(
                    selectionEnd,
                    2
                )
                + "s  ("
                + juce::String(
                    length,
                    2
                )
                + "s)",

                selectionArea.reduced(4),

                juce::Justification::centredTop,

                false
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


    if (!waveformArea.contains(
            event.getPosition()))
    {
        return;
    }


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    externalDragStarted = false;


    // ==========================================================
    // If we already have a selection and click INSIDE it,
    // prepare to drag the selected region to the DAW.
    // ==========================================================

    if (hasSelection())
    {
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

        const double clickTime =
            normalizedPosition
            * totalLength;


        if (clickTime >= selectionStart
            && clickTime <= selectionEnd)
        {
            selecting = false;
            selectionMouseStartX =
                event.getPosition().x;

            return;
        }
    }


    // ==========================================================
    // Otherwise begin a NEW selection.
    // ==========================================================

    selecting = true;

    selectionMouseStartX =
        event.getPosition().x;


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
    auto waveformArea =
        getWaveformArea();


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    // ==========================================================
    // EXISTING SELECTION
    //
    // If the user clicks inside an existing selection and drags,
    // export that selected region and start the DAW drag.
    // ==========================================================

    if (!selecting
        && hasSelection()
        && !externalDragStarted)
    {
        if (event.getDistanceFromDragStart() >= 8)
        {
            if (startExternalFileDrag())
            {
                externalDragStarted = true;
            }

            return;
        }
    }


    // ==========================================================
    // NEW SELECTION
    // ==========================================================

    if (!selecting)
        return;


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
    const juce::MouseEvent&
)
{
    if (externalDragStarted)
    {
        externalDragStarted = false;
        return;
    }


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
// Double Click = Select Entire Track
// ==========================================================

void StemTrackComponent::mouseDoubleClick(
    const juce::MouseEvent& event
)
{
    auto waveformArea =
        getWaveformArea();


    if (!waveformArea.contains(
            event.getPosition()))
    {
        return;
    }


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    selectEntireTrack();
}


// ==========================================================
// Select Entire Track
// ==========================================================

void StemTrackComponent::selectEntireTrack()
{
    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return;


    selectionStart = 0.0;

    selectionEnd = totalLength;

    selecting = false;


    repaint();


    if (onSelectionChanged)
        onSelectionChanged();
}


// ==========================================================
// External File Drag
// ==========================================================

bool StemTrackComponent::startExternalFileDrag()
{
    if (!audioFile.existsAsFile())
        return false;


    if (audioFile.getFullPathName().isEmpty())
        return false;


    // ==========================================================
    // Determine what to drag
    //
    // Selection exists:
    //     create a temporary WAV containing ONLY selection.
    //
    // No selection:
    //     drag original full WAV.
    // ==========================================================

    if (hasSelection())
    {
        if (!createTemporarySelectionFile())
            return false;
    }
    else
    {
        externalDragFile =
            audioFile;
    }


    if (!externalDragFile.existsAsFile())
        return false;


    juce::StringArray files;

    files.add(
        externalDragFile.getFullPathName()
    );


    // ==========================================================
    // Native OS drag
    // ==========================================================

    return juce::DragAndDropContainer::
        performExternalDragDropOfFiles(
            files,
            false,
            this,
            [this]
            {
                externalDragStarted = false;
            }
        );
}


// ==========================================================
// Create Temporary Selection WAV
// ==========================================================

bool StemTrackComponent::createTemporarySelectionFile()
{
    if (!audioFile.existsAsFile())
        return false;


    const double totalLength =
        thumbnail.getTotalLength();


    if (totalLength <= 0.0)
        return false;


    const double start =
        juce::jlimit(
            0.0,
            totalLength,
            selectionStart
        );


    const double end =
        juce::jlimit(
            0.0,
            totalLength,
            selectionEnd
        );


    if (end <= start)
        return false;


    // ==========================================================
    // Create temporary directory
    // ==========================================================

    auto tempDirectory =
        juce::File::getSpecialLocation(
            juce::File::tempDirectory
        )
        .getChildFile(
            "OfforStemSplitter"
        );


    if (!tempDirectory.exists())
    {
        if (!tempDirectory.createDirectory())
            return false;
    }


    // ==========================================================
    // Create unique filename
    // ==========================================================

    const auto stemNameSafe =
        stemName
            .replace(
                " ",
                "_"
            )
            .replace(
                "/",
                "_"
            )
            .replace(
                "\\",
                "_"
            );


    const auto timestamp =
        juce::Time::getCurrentTime()
            .toMilliseconds();


    externalDragFile =
        tempDirectory.getChildFile(
            stemNameSafe
            + "_"
            + juce::String(timestamp)
            + ".wav"
        );


    // ==========================================================
    // Remove existing file
    // ==========================================================

    externalDragFile.deleteFile();


    // ==========================================================
    // Open source reader
    // ==========================================================

    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(
            audioFile
        )
    );


    if (reader == nullptr)
        return false;


    const int64 sourceStartSample =
        static_cast<int64>(
            start
            * reader->sampleRate
        );


    const int64 sourceEndSample =
        static_cast<int64>(
            end
            * reader->sampleRate
        );


    const int64 numberOfSamples =
        sourceEndSample
        - sourceStartSample;


    if (numberOfSamples <= 0)
        return false;


    // ==========================================================
    // Create WAV writer
    // ==========================================================

    std::unique_ptr<juce::FileOutputStream> outputStream(
        externalDragFile.createOutputStream()
    );


    if (outputStream == nullptr)
        return false;


    juce::WavAudioFormat wavFormat;


    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(
            outputStream.get(),
            reader->sampleRate,
            static_cast<unsigned int>(
                reader->numChannels
            ),
            24,
            {},
            0
        )
    );


    if (writer == nullptr)
        return false;


    // ==========================================================
    // Writer owns the stream now
    // ==========================================================

    outputStream.release();


    // ==========================================================
    // Render selection
    // ==========================================================

    constexpr int blockSize = 8192;


    juce::AudioBuffer<float> buffer(
        static_cast<int>(
            reader->numChannels
        ),
        blockSize
    );


    int64 samplesRemaining =
        numberOfSamples;


    int64 currentSample =
        sourceStartSample;


    while (samplesRemaining > 0)
    {
        const int numSamplesThisBlock =
            static_cast<int>(
                juce::jmin<int64>(
                    samplesRemaining,
                    blockSize
                )
            );


        buffer.clear();


        if (!reader->read(
                &buffer,
                0,
                numSamplesThisBlock,
                currentSample,
                true,
                true))
        {
            writer.reset();
            externalDragFile.deleteFile();

            return false;
        }


        writer->writeFromAudioSampleBuffer(
            buffer,
            0,
            numSamplesThisBlock
        );


        currentSample +=
            numSamplesThisBlock;

        samplesRemaining -=
            numSamplesThisBlock;
    }


    writer.reset();


    // ==========================================================
    // Verify result
    // ==========================================================

    if (!externalDragFile.existsAsFile())
        return false;


    if (externalDragFile.getSize() <= 44)
        return false;


    return true;
}


// ==========================================================
// Clear Selection
// ==========================================================

void StemTrackComponent::clearSelection()
{
    selectionStart = 0.0;

    selectionEnd = 0.0;

    selecting = false;

    externalDragStarted = false;

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
    audioFile =
        file;

    playheadPosition =
        0.0;

    selectionStart =
        0.0;

    selectionEnd =
        0.0;

    selecting =
        false;

    externalDragStarted =
        false;

    externalDragFile =
        juce::File();


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


    playheadPosition =
        0.0;


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

    externalDragStarted =
        false;

    externalDragFile =
        juce::File();


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
// Selection Start
// ==========================================================

double StemTrackComponent::getSelectionStart() const
{
    return selectionStart;
}


// ==========================================================
// Selection End
// ==========================================================

double StemTrackComponent::getSelectionEnd() const
{
    return selectionEnd;
}


// ==========================================================
// Has Selection
// ==========================================================

bool StemTrackComponent::hasSelection() const
{
    return selectionEnd > selectionStart;
}


// ==========================================================
// Audio File
// ==========================================================

juce::File StemTrackComponent::getAudioFile() const
{
    return audioFile;
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