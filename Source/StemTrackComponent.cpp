#include "StemTrackComponent.h"

// ==========================================================
// Constructor
// ==========================================================

StemTrackComponent::StemTrackComponent(
    const juce::String& name
)
    : stemName(name)
{
    // ======================================================
    // Audio
    // ======================================================

    formatManager.registerBasicFormats();

    // ======================================================
    // Track Name
    // ======================================================

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

    // ======================================================
    // Whole Track Selection
    // ======================================================

    trackSelectButton.setClickingTogglesState(
        true
    );

    trackSelectButton.setTooltip(
        "Select entire track for DAW export"
    );

    trackSelectButton.onClick = [this]
    {
        trackSelected =
            trackSelectButton.getToggleState();

        updateTrackSelectionButton();

        if (trackSelected)
        {
            const double totalLength =
                thumbnail.getTotalLength();

            if (totalLength > 0.0)
            {
                selectionStart = 0.0;
                selectionEnd = totalLength;
            }
        }
        else
        {
            selectionStart = 0.0;
            selectionEnd = 0.0;
        }

        repaint();

        if (onTrackSelectionChanged)
        {
            onTrackSelectionChanged(
                trackSelected
            );
        }

        if (onSelectionChanged)
        {
            onSelectionChanged();
        }
    };

    addAndMakeVisible(
        trackSelectButton
    );

    // ======================================================
    // Mute
    // ======================================================

    muteButton.setClickingTogglesState(
        true
    );

    muteButton.setButtonText(
        "M"
    );

    muteButton.setTooltip(
        "Mute track"
    );

    muteButton.onClick = [this]
    {
        muted =
            muteButton.getToggleState();

        updateButtonStates();

        if (onTrackStateChanged)
        {
            onTrackStateChanged();
        }

        if (onMixingChanged)
        {
            onMixingChanged();
        }
    };

    addAndMakeVisible(
        muteButton
    );

    // ======================================================
    // Solo
    // ======================================================

    soloButton.setClickingTogglesState(
        true
    );

    soloButton.setButtonText(
        "S"
    );

    soloButton.setTooltip(
        "Solo track"
    );

    soloButton.onClick = [this]
    {
        soloed =
            soloButton.getToggleState();

        updateButtonStates();

        if (onTrackStateChanged)
        {
            onTrackStateChanged();
        }

        if (onMixingChanged)
        {
            onMixingChanged();
        }

        // ==================================================
        // Solo selected region
        // ==================================================

        if (soloed
            && !muted
            && hasSelection())
        {
            if (onSoloSelectionRequested)
            {
                onSoloSelectionRequested(
                    selectionStart,
                    selectionEnd
                );
            }
        }
    };

    addAndMakeVisible(
        soloButton
    );

    // ======================================================
    // Volume Slider
    // ======================================================

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

    volumeSlider.setTooltip(
        "Track volume"
    );

    volumeSlider.onValueChange = [this]
    {
        volume =
            static_cast<float>(
                volumeSlider.getValue()
            );

        if (onTrackStateChanged)
        {
            onTrackStateChanged();
        }

        if (onMixingChanged)
        {
            onMixingChanged();
        }
    };

    // Keep hidden for now.
    volumeSlider.setVisible(false);

    addAndMakeVisible(
        volumeSlider
    );

    // ======================================================
    // Initial UI State
    // ======================================================

    updateButtonStates();
    updateTrackSelectionButton();
}


// ==========================================================
// Paint
// ==========================================================

void StemTrackComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // ======================================================
    // TRACK COLORS
    // ======================================================

    const auto panel      = juce::Colour(0xff111417);
    const auto panelDark  = juce::Colour(0xff0c0f11);
    const auto border     = juce::Colour(0xff2a3035);
    const auto divider    = juce::Colour(0xff252a2f);
    const auto orange     = juce::Colour(0xffff6b22);
    const auto orangeSoft = juce::Colour(0xffff6b22).withAlpha(0.18f);

    // ======================================================
    // OUTER TRACK
    // ======================================================

    g.setColour(panel);

    g.fillRoundedRectangle(
        bounds.toFloat(),
        7.0f
    );

    // ======================================================
    // BORDER
    // ======================================================

    g.setColour(
        trackSelected
            ? orange.withAlpha(0.85f)
            : border
    );

    g.drawRoundedRectangle(
        bounds.toFloat().reduced(0.5f),
        7.0f,
        trackSelected ? 1.5f : 1.0f
    );

    // ======================================================
    // CHANNEL AREA
    // ======================================================

    auto channelArea =
        bounds
            .reduced(1)
            .removeFromLeft(150);

    g.setColour(panel);

    g.fillRoundedRectangle(
        channelArea.toFloat(),
        6.0f
    );

    // ======================================================
    // CHANNEL / WAVEFORM DIVIDER
    // ======================================================

    g.setColour(divider);

    g.fillRect(
        channelArea.getRight(),
        bounds.getY() + 7,
        1,
        bounds.getHeight() - 14
    );

    // ======================================================
    // SELECTED INDICATOR
    // ======================================================

    if (trackSelected)
    {
        g.setColour(orange);

        g.fillRoundedRectangle(
            static_cast<float>(bounds.getX()),
            static_cast<float>(bounds.getY()),
            4.0f,
            static_cast<float>(bounds.getHeight()),
            3.0f
        );
    }

    // ======================================================
    // STEM NUMBER
    // ======================================================

    juce::String stemNumber = "01";

    if (stemName.equalsIgnoreCase("Drums"))
        stemNumber = "02";
    else if (stemName.equalsIgnoreCase("Bass"))
        stemNumber = "03";
    else if (stemName.equalsIgnoreCase("Instrumental"))
        stemNumber = "04";

    auto numberArea =
        channelArea
            .removeFromTop(16)
            .reduced(12, 0);

    g.setColour(
        juce::Colours::white.withAlpha(0.35f)
    );

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(9.0f)
        )
    );

    g.drawText(
        stemNumber,
        numberArea,
        juce::Justification::left,
        false
    );

    // ======================================================
    // STEM NAME
    // ======================================================

    auto nameArea =
        channelArea
            .removeFromTop(20)
            .reduced(12, 0);

    g.setColour(
        juce::Colours::white
    );

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(13.0f)
                .withStyle("Bold")
        )
    );

    g.drawText(
        stemName.toUpperCase(),
        nameArea,
        juce::Justification::left,
        false
    );

    // ======================================================
    // VOLUME LABEL
    // ======================================================

    auto volumeArea =
        channelArea
            .removeFromBottom(16)
            .reduced(12, 0);

    g.setColour(
        juce::Colours::white.withAlpha(0.38f)
    );

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(8.5f)
        )
    );

    g.drawText(
        "VOLUME",
        volumeArea,
        juce::Justification::left,
        false
    );

    // ======================================================
    // VOLUME VALUE
    // ======================================================

    g.setColour(
        juce::Colours::white.withAlpha(0.65f)
    );

    g.drawText(
        juce::String(
            juce::Decibels::gainToDecibels(
                juce::jmax(0.0001f, volume)
            ),
            1
        ) + " dB",
        volumeArea,
        juce::Justification::right,
        false
    );

    // ======================================================
    // VOLUME TRACK
    // ======================================================

    auto volumeTrack =
        channelArea
            .reduced(12, 0);

    volumeTrack.setY(
        volumeTrack.getCentreY() - 2
    );

    volumeTrack.setHeight(4);

    g.setColour(
        juce::Colour(0xff252a2f)
    );

    g.fillRoundedRectangle(
        volumeTrack.toFloat(),
        2.0f
    );

    const float volumeRatio =
        juce::jlimit(
            0.0f,
            1.0f,
            volume
        );

    auto volumeFill =
        volumeTrack.withWidth(
            static_cast<int>(
                volumeTrack.getWidth()
                * volumeRatio
            )
        );

    g.setColour(orange);

    g.fillRoundedRectangle(
        volumeFill.toFloat(),
        2.0f
    );

    // ======================================================
    // WAVEFORM AREA
    // ======================================================

    const auto waveformArea =
        getWaveformArea();

    // ======================================================
    // WAVEFORM BACKGROUND
    // ======================================================

    g.setColour(panelDark);

    g.fillRect(
        waveformArea
    );

    // ======================================================
    // SUBTLE TIMELINE GRID
    // ======================================================

    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(
            juce::Colour(0xff20252a)
                .withAlpha(0.45f)
        );

        constexpr int gridLines = 8;

        for (int i = 1; i < gridLines; ++i)
        {
            const float ratio =
                static_cast<float>(i)
                / static_cast<float>(gridLines);

            const float x =
                static_cast<float>(
                    waveformArea.getX()
                )
                + ratio
                * static_cast<float>(
                    waveformArea.getWidth()
                );

            g.drawVerticalLine(
                static_cast<int>(x),
                static_cast<float>(
                    waveformArea.getY()
                ),
                static_cast<float>(
                    waveformArea.getBottom()
                )
            );
        }
    }

    // ======================================================
    // WAVEFORM
    // ======================================================

    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(
            trackSelected
                ? orange
                : juce::Colour(0xffc4511c)
        );

        thumbnail.drawChannels(
            g,
            waveformArea.reduced(0, 4),
            0.0,
            thumbnail.getTotalLength(),
            1.0f
        );

        // ==================================================
        // SELECTION
        // ==================================================

        if (hasSelection())
        {
            const double totalLength =
                thumbnail.getTotalLength();

            const double startRatio =
                juce::jlimit(
                    0.0,
                    1.0,
                    selectionStart / totalLength
                );

            const double endRatio =
                juce::jlimit(
                    0.0,
                    1.0,
                    selectionEnd / totalLength
                );

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
                waveformArea.withLeft(startX)
                            .withRight(
                                juce::jmax(
                                    startX + 1,
                                    endX
                                )
                            );

            // Selection background
            g.setColour(
                orangeSoft
            );

            g.fillRect(
                selectionArea
            );

            // Selected waveform
            g.setColour(
                juce::Colour(0xffff8a45)
            );

            thumbnail.drawChannels(
                g,
                selectionArea,
                selectionStart,
                selectionEnd,
                1.0f
            );

            // Selection boundaries
            g.setColour(
                juce::Colour(0xffff9b5d)
            );

            g.fillRect(
                startX,
                waveformArea.getY(),
                1,
                waveformArea.getHeight()
            );

            g.fillRect(
                endX,
                waveformArea.getY(),
                1,
                waveformArea.getHeight()
            );
        }
    }
    else
    {
        g.setColour(
            juce::Colours::white.withAlpha(0.25f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(10.0f)
            )
        );

        g.drawText(
            "NO AUDIO",
            waveformArea,
            juce::Justification::centred,
            false
        );
    }

    // ======================================================
    // WAVEFORM CENTER LINE
    // ======================================================

    g.setColour(
        juce::Colour(0xff30363b)
    );

    g.drawHorizontalLine(
        waveformArea.getCentreY(),
        static_cast<float>(
            waveformArea.getX()
        ),
        static_cast<float>(
            waveformArea.getRight()
        )
    );

    // ======================================================
    // PLAYHEAD
    // ======================================================

    if (thumbnail.getTotalLength() > 0.0)
    {
        const double totalLength =
            thumbnail.getTotalLength();

        const double normalizedPosition =
            juce::jlimit(
                0.0,
                1.0,
                playheadPosition / totalLength
            );

        const int playheadX =
            waveformArea.getX()
            + static_cast<int>(
                normalizedPosition
                * waveformArea.getWidth()
            );

        // glow
        g.setColour(
            orange.withAlpha(0.12f)
        );

        g.fillRect(
            playheadX - 2,
            waveformArea.getY(),
            5,
            waveformArea.getHeight()
        );

        // main line
        g.setColour(
            juce::Colour(0xffff8b45)
        );

        g.fillRect(
            playheadX,
            waveformArea.getY(),
            2,
            waveformArea.getHeight()
        );
    }

    // ======================================================
    // MUTED OVERLAY
    // ======================================================

    if (muted)
    {
        g.setColour(
            juce::Colours::black.withAlpha(0.42f)
        );

        g.fillRoundedRectangle(
            waveformArea.toFloat(),
            3.0f
        );

        g.setColour(
            juce::Colours::white.withAlpha(0.4f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(9.0f)
                    .withStyle("Bold")
            )
        );

        g.drawText(
            "MUTED",
            waveformArea.reduced(10),
            juce::Justification::topRight,
            false
        );
    }

    // ======================================================
    // SOLO OVERLAY
    // ======================================================

    if (soloed)
    {
        g.setColour(
            juce::Colour(0xffffb347)
                .withAlpha(0.10f)
        );

        g.fillRect(
            waveformArea
        );

        g.setColour(
            juce::Colour(0xffffb347)
                .withAlpha(0.8f)
        );

        g.setFont(
            juce::Font(
                juce::FontOptions()
                    .withHeight(9.0f)
                    .withStyle("Bold")
            )
        );

        g.drawText(
            "SOLO",
            waveformArea.reduced(10),
            juce::Justification::topRight,
            false
        );
    }
}


// ==========================================================
// Resized
//
// FINAL LAYOUT:
//
// | CHECK | VOCALS | waveform -------------------- | M | S |
// | CHECK | DRUMS  | waveform -------------------- | M | S |
// | CHECK | BASS   | waveform -------------------- | M | S |
// | CHECK | INSTR  | waveform -------------------- | M | S |
//
// ==========================================================

void StemTrackComponent::resized()
{
    auto area =
        getLocalBounds()
            .reduced(2);

    // ======================================================
    // Left Name / Selection Area
    // ======================================================

    auto nameArea =
        area.removeFromLeft(125);

    // Checkbox

    auto selectArea =
        nameArea.removeFromLeft(26);

    // trackSelectButton.setBounds(
    //     selectArea.reduced(3)
    // );

    trackSelectButton.setBounds(
        selectArea.withSizeKeepingCentre(
            20,
            20
        )
    );

    // Track name

    nameLabel.setBounds(
        nameArea.reduced(
            5,
            0
        )
    );

    // ======================================================
    // Right M / S Controls
    // ======================================================

    auto controls =
        area.removeFromRight(92);

    const int buttonWidth =
        42;

    auto muteArea =
        controls.removeFromLeft(
            buttonWidth
        );

    auto soloArea =
        controls.removeFromLeft(
            buttonWidth
        );

    muteButton.setBounds(
        muteArea.reduced(
            4,
            10
        )
    );

    soloButton.setBounds(
        soloArea.reduced(
            4,
            10
        )
    );

    // ======================================================
    // Waveform
    // ======================================================

    // Everything remaining between the name
    // and M/S controls becomes the waveform.

    // No component is placed here because the
    // waveform is painted directly in paint().
}


// ==========================================================
// Mouse Down
// ==========================================================

void StemTrackComponent::mouseDown(
    const juce::MouseEvent& event
)
{
    const auto waveformArea =
        getWaveformArea();

    // ======================================================
    // Right-click = Export Selection
    // ======================================================

    if (event.mods.isPopupMenu())
    {
        if (waveformArea.contains(event.getPosition())
            && hasSelection())
        {
            if (onExportSelectionRequested)
            {
                onExportSelectionRequested(
                    selectionStart,
                    selectionEnd
                );
            }
        }

        return;
    }

    // ======================================================
    // Only waveform accepts selection / DAW drag
    // ======================================================

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
    draggingExistingSelection = false;

    // ======================================================
    // Whole Track Selected
    // ======================================================

    if (trackSelected)
    {
        draggingExistingSelection = true;
        return;
    }

    // ======================================================
    // Existing Partial Selection
    // ======================================================

    if (hasSelection())
    {
        const double normalizedPosition =
            juce::jlimit(
                0.0,
                1.0,
                (
                    static_cast<double>(
                        event.position.x
                    )
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
            draggingExistingSelection = true;
            return;
        }
    }

    // ======================================================
    // Create New Partial Selection
    // ======================================================

    selectionMouseStartX =
        event.getPosition().x;

    selecting = true;

    const double normalizedPosition =
        juce::jlimit(
            0.0,
            1.0,
            (
                static_cast<double>(
                    event.position.x
                )
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
    // ======================================================
    // Drag Existing Selection To DAW
    // ======================================================

    if (draggingExistingSelection)
    {
        if (!externalDragStarted
            && event.getDistanceFromDragStart() >= 8)
        {
            if (startExternalFileDrag())
            {
                externalDragStarted = true;
                draggingExistingSelection = false;
            }
        }

        return;
    }

    // ======================================================
    // No Selection Creation
    // ======================================================

    if (!selecting)
        return;

    const auto waveformArea =
        getWaveformArea();

    const double totalLength =
        thumbnail.getTotalLength();

    if (totalLength <= 0.0)
        return;

    // ======================================================
    // Mouse Positions
    // ======================================================

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

    // ======================================================
    // Convert X To Time
    // ======================================================

    const double startRatio =
        (
            startX
            - waveformArea.getX()
        )
        / static_cast<double>(
            waveformArea.getWidth()
        );

    const double currentRatio =
        (
            currentX
            - waveformArea.getX()
        )
        / static_cast<double>(
            waveformArea.getWidth()
        );

    const double startTime =
        startRatio
        * totalLength;

    const double currentTime =
        currentRatio
        * totalLength;

    // ======================================================
    // Selection Direction
    // ======================================================

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
    // ======================================================
    // External DAW Drag Finished
    // ======================================================

    if (externalDragStarted)
    {
        externalDragStarted = false;
        draggingExistingSelection = false;
        return;
    }

    // ======================================================
    // Existing Selection Clicked
    // ======================================================

    if (draggingExistingSelection)
    {
        draggingExistingSelection = false;
        return;
    }

    // ======================================================
    // Finish Selection
    // ======================================================

    if (!selecting)
        return;

    selecting = false;

    // ======================================================
    // Tiny Selection = Seek
    // ======================================================

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
        {
            onSeek(
                seekPosition
            );
        }
    }
    else
    {
        // ==================================================
        // Partial Selection Created
        // ==================================================

        trackSelected = false;

        trackSelectButton.setToggleState(
            false,
            juce::dontSendNotification
        );

        updateTrackSelectionButton();

        if (onSelectionChanged)
        {
            onSelectionChanged();
        }

        // ==================================================
        // Solo Selected Region
        // ==================================================

        if (soloed
            && !muted
            && hasSelection())
        {
            if (onSoloSelectionRequested)
            {
                onSoloSelectionRequested(
                    selectionStart,
                    selectionEnd
                );
            }
        }
    }

    repaint();
}


// ==========================================================
// Create Selection Drag File
// ==========================================================

bool StemTrackComponent::createSelectionDragFile()
{
    if (!audioFile.existsAsFile())
        return false;

    if (!hasSelection())
        return false;

    const double selectionLength =
        selectionEnd - selectionStart;

    if (selectionLength <= 0.0)
        return false;

    // ======================================================
    // Temporary Directory
    // ======================================================

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

    // ======================================================
    // Safe Filename
    // ======================================================

    const juce::String safeName =
        stemName.replaceCharacters(
            "\\/:*?\"<>|",
            "________"
        );

    temporaryDragFile =
        tempDirectory.getChildFile(
            safeName
            + "_selection_"
            + juce::String(
                juce::Time::getCurrentTime()
                    .toMilliseconds()
            )
            + ".wav"
        );

    // ======================================================
    // Reader
    // ======================================================

    juce::AudioFormatManager readerFormatManager;

    readerFormatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(
        readerFormatManager.createReaderFor(
            audioFile
        )
    );

    if (reader == nullptr)
        return false;

    // ======================================================
    // Sample Range
    // ======================================================

    const juce::int64 startSample =
        static_cast<juce::int64>(
            selectionStart
            * reader->sampleRate
        );

    const juce::int64 endSample =
        static_cast<juce::int64>(
            selectionEnd
            * reader->sampleRate
        );

    const juce::int64 numberOfSamples =
        endSample - startSample;

    if (numberOfSamples <= 0)
        return false;

    // ======================================================
    // WAV Writer
    // ======================================================

    juce::WavAudioFormat wavFormat;

    std::unique_ptr<juce::FileOutputStream> outputStream(
        temporaryDragFile.createOutputStream()
    );

    if (outputStream == nullptr)
        return false;

    constexpr int bitsPerSample = 24;

    auto* writer =
        wavFormat.createWriterFor(
            outputStream.get(),
            reader->sampleRate,
            static_cast<unsigned int>(
                reader->numChannels
            ),
            bitsPerSample,
            {},
            0
        );

    if (writer == nullptr)
    {
        temporaryDragFile.deleteFile();
        return false;
    }

    // Writer owns the stream.

    outputStream.release();

    std::unique_ptr<juce::AudioFormatWriter> writerOwner(
        writer
    );

    // ======================================================
    // Render Selection
    // ======================================================

    constexpr int blockSize = 8192;

    juce::AudioBuffer<float> buffer(
        static_cast<int>(
            reader->numChannels
        ),
        blockSize
    );

    juce::int64 remaining =
        numberOfSamples;

    while (remaining > 0)
    {
        const int samplesThisBlock =
            static_cast<int>(
                juce::jmin<juce::int64>(
                    remaining,
                    blockSize
                )
            );

        buffer.clear();

        if (!reader->read(
                &buffer,
                0,
                samplesThisBlock,
                startSample
                + (
                    numberOfSamples
                    - remaining
                ),
                true,
                true))
        {
            writerOwner.reset();
            temporaryDragFile.deleteFile();
            return false;
        }

        if (!writerOwner->writeFromAudioSampleBuffer(
                buffer,
                0,
                samplesThisBlock
            ))
        {
            writerOwner.reset();
            temporaryDragFile.deleteFile();
            return false;
        }

        remaining -= samplesThisBlock;
    }

    // ======================================================
    // Finalize
    // ======================================================

    writerOwner.reset();

    return temporaryDragFile.existsAsFile()
        && temporaryDragFile.getSize() > 44;
}


// ==========================================================
// Begin Control Area Drag
// ==========================================================

void StemTrackComponent::beginControlAreaDrag()
{
    if (!audioFile.existsAsFile())
        return;

    const juce::File fileToDrag =
        audioFile;

    if (!fileToDrag.existsAsFile())
        return;

    juce::StringArray files;

    files.add(
        fileToDrag.getFullPathName()
    );

    const bool started =
        juce::DragAndDropContainer::
            performExternalDragDropOfFiles(
                files,
                false,
                this,
                [this]
                {
                    externalDragStarted =
                        false;

                    draggingFromControlArea =
                        false;
                }
            );

    if (started)
    {
        externalDragStarted = true;
    }
}


// ==========================================================
// External File Drag
// ==========================================================

bool StemTrackComponent::startExternalFileDrag()
{
    if (!audioFile.existsAsFile())
        return false;

    juce::File fileToDrag;

    // ======================================================
    // Whole Track
    // ======================================================

    if (trackSelected)
    {
        fileToDrag =
            audioFile;
    }
    else
    {
        // ==================================================
        // Partial Selection
        // ==================================================

        if (!hasSelection())
            return false;

        if (!createSelectionDragFile())
            return false;

        fileToDrag =
            temporaryDragFile;
    }

    if (!fileToDrag.existsAsFile())
        return false;

    juce::StringArray files;

    files.add(
        fileToDrag.getFullPathName()
    );

    return juce::DragAndDropContainer::
        performExternalDragDropOfFiles(
            files,
            false,
            this,
            [this]
            {
                externalDragStarted =
                    false;

                draggingExistingSelection =
                    false;
            }
        );
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
    draggingExistingSelection = false;

    trackSelected = false;

    trackSelectButton.setToggleState(
        false,
        juce::dontSendNotification
    );

    updateTrackSelectionButton();

    if (temporaryDragFile.existsAsFile())
    {
        temporaryDragFile.deleteFile();
    }

    temporaryDragFile =
        juce::File();

    repaint();

    if (onSelectionChanged)
    {
        onSelectionChanged();
    }

    if (onTrackSelectionChanged)
    {
        onTrackSelectionChanged(
            false
        );
    }
}


// ==========================================================
// Waveform Area
//
// IMPORTANT:
// THIS IS THE ONLY getWaveformArea()
// IN THIS FILE.
//
// Layout:
//
// [ NAME 125px ][ WAVEFORM ][ M/S 92px ]
//
// ==========================================================

// juce::Rectangle<int>
// StemTrackComponent::getWaveformArea() const
// {
//     auto area =
//         getLocalBounds()
//             .reduced(2);

//     // Remove left name area.

//     area.removeFromLeft(
//         125
//     );

//     // Remove right M/S area.

//     area.removeFromRight(
//         92
//     );

//     return area.reduced(
//         2,
//         10
//     );
// }


// ==========================================================
// Waveform Area
// ==========================================================

juce::Rectangle<int>
StemTrackComponent::getWaveformArea() const
{
    return getLocalBounds()
        .reduced(1)
        .withTrimmedLeft(150)
        .reduced(8, 7);
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

    draggingExistingSelection =
        false;

    trackSelected =
        false;

    trackSelectButton.setToggleState(
        false,
        juce::dontSendNotification
    );

    updateTrackSelectionButton();

    if (temporaryDragFile.existsAsFile())
    {
        temporaryDragFile.deleteFile();
    }

    temporaryDragFile =
        juce::File();

    thumbnail.clear();

    if (!audioFile.existsAsFile())
    {
        repaint();
        return;
    }

    // ======================================================
    // Load Audio Into Thumbnail
    // ======================================================

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

    draggingExistingSelection =
        false;

    trackSelected =
        false;

    trackSelectButton.setToggleState(
        false,
        juce::dontSendNotification
    );

    updateTrackSelectionButton();

    if (temporaryDragFile.existsAsFile())
    {
        temporaryDragFile.deleteFile();
    }

    temporaryDragFile =
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

    const double totalLength =
        thumbnail.getTotalLength();

    if (totalLength > 0.0)
    {
        playheadPosition =
            juce::jmin(
                playheadPosition,
                totalLength
            );
    }

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
    {
        onMixingChanged();
    }
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


// ==========================================================
// Track Selection
// ==========================================================

void StemTrackComponent::setTrackSelected(
    bool shouldBeSelected
)
{
    trackSelected =
        shouldBeSelected;

    trackSelectButton.setToggleState(
        trackSelected,
        juce::dontSendNotification
    );

    updateTrackSelectionButton();

    if (trackSelected)
    {
        const double totalLength =
            thumbnail.getTotalLength();

        if (totalLength > 0.0)
        {
            selectionStart =
                0.0;

            selectionEnd =
                totalLength;
        }
    }
    else
    {
        selectionStart =
            0.0;

        selectionEnd =
            0.0;
    }

    if (onTrackSelectionChanged)
    {
        onTrackSelectionChanged(
            trackSelected
        );
    }

    if (onSelectionChanged)
    {
        onSelectionChanged();
    }

    repaint();
}


// ==========================================================
// Update Track Selection Button
// ==========================================================

void StemTrackComponent::updateTrackSelectionButton()
{
    trackSelectButton.setButtonText(
        trackSelected
            ? "✓"
            : "□"
    );
}
