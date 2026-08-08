#include "MainComponent.h"
#include "AppConfig.h"


// ==============================================================
// CONSTRUCTOR
// ==============================================================

MainComponent::MainComponent()
{
    // ==========================================================
    // Audio format
    // ==========================================================

    formatManager.registerBasicFormats();


    // ==========================================================
    // Audio device
    // ==========================================================

    auto error =
        audioDeviceManager.initialise(
            0,
            2,
            nullptr,
            true
        );


    if (error.isNotEmpty())
    {
        setStatus(
            "Audio device error: " + error
        );
    }


    audioSourcePlayer.setSource(
        &mixerSource
    );


    audioDeviceManager.addAudioCallback(
        &audioSourcePlayer
    );


    // ==========================================================
    // Window
    // ==========================================================

    setSize(
        700,
        300
    );


    // ==========================================================
    // Title
    // ==========================================================

    titleLabel.setText(
        OfforStemSplitter::PRODUCT_NAME,
        juce::dontSendNotification
    );


    titleLabel.setFont(
        juce::Font(28.0f)
    );


    titleLabel.setJustificationType(
        juce::Justification::centred
    );


    addAndMakeVisible(
        titleLabel
    );


    // ==========================================================
    // Select button
    // ==========================================================

    addAndMakeVisible(
        selectButton
    );


    selectButton.onClick = [this]
    {
        selectAudioFile();
    };


    // ==========================================================
    // Separate button
    // ==========================================================

    addAndMakeVisible(
        separateButton
    );


    separateButton.onClick = [this]
    {
        runSeparator();
    };


    // ==========================================================
    // Transport
    // ==========================================================

    addAndMakeVisible(
        playButton
    );


    addAndMakeVisible(
        pauseButton
    );


    addAndMakeVisible(
        stopButton
    );


    playButton.onClick = [this]
    {
        startPlayback();
    };


    pauseButton.onClick = [this]
    {
        pausePlayback();
    };


    stopButton.onClick = [this]
    {
        stopPlayback();
    };


    // ==========================================================
    // File label
    // ==========================================================

    fileLabel.setText(
        "No file selected",
        juce::dontSendNotification
    );


    fileLabel.setJustificationType(
        juce::Justification::centred
    );


    addAndMakeVisible(
        fileLabel
    );


    // ==========================================================
    // Status
    // ==========================================================

    statusLabel.setText(
        "Status: Waiting...",
        juce::dontSendNotification
    );


    statusLabel.setJustificationType(
        juce::Justification::centred
    );


    addAndMakeVisible(
        statusLabel
    );


    // ==========================================================
    // Stem tracks
    // ==========================================================

    addAndMakeVisible(
        vocalsTrack
    );


    addAndMakeVisible(
        drumsTrack
    );


    addAndMakeVisible(
        bassTrack
    );


    addAndMakeVisible(
        instrumentalTrack
    );


    // ==========================================================
    // Track control callbacks
    // ==========================================================

    vocalsTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };


    drumsTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };


    bassTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };


    instrumentalTrack.onTrackStateChanged =
        [this]
    {
        updateTrackMixing();
    };


    // ==========================================================
    // Stem labels
    // ==========================================================

    auto setupStemLabel =
        [](juce::Label& label,
           const juce::String& text)
    {
        label.setText(
            text,
            juce::dontSendNotification
        );


        label.setJustificationType(
            juce::Justification::centredLeft
        );
    };


    setupStemLabel(
        vocalsLabel,
        "01  Vocals"
    );


    setupStemLabel(
        drumsLabel,
        "02  Drums"
    );


    setupStemLabel(
        bassLabel,
        "03  Bass"
    );


    setupStemLabel(
        instrumentalLabel,
        "04  Instrumental"
    );


    // ==========================================================
    // Output button
    // ==========================================================

    addAndMakeVisible(
        openOutputButton
    );


    openOutputButton.onClick = [this]
    {
        openOutputFolder();
    };


    // ==========================================================
    // Initially hide output labels
    // ==========================================================

    clearSeparatedStems();
}


// ==============================================================
// DESTRUCTOR
// ==============================================================

MainComponent::~MainComponent()
{
    playbackTimer.stopTimer();

    unloadStemAudio();

    audioSourcePlayer.setSource(
        nullptr
    );

    audioDeviceManager.removeAudioCallback(
        &audioSourcePlayer
    );

    separatorProcess.reset();
}


// ==============================================================
// PAINT
// ==============================================================

void MainComponent::paint(
    juce::Graphics& g
)
{
    g.fillAll(
        juce::Colours::black
    );
}


// ==============================================================
// RESIZED
// ==============================================================

void MainComponent::resized()
{
    auto area =
        getLocalBounds().reduced(15);


    // ==========================================================
    // TITLE
    // ==========================================================

    titleLabel.setBounds(
        area.removeFromTop(35)
    );

    area.removeFromTop(4);


    // ==========================================================
    // TRANSPORT
    // ==========================================================

    auto transportArea =
        area.removeFromTop(34)
            .withSizeKeepingCentre(
                240,
                32
            );

    playButton.setBounds(
        transportArea
            .removeFromLeft(80)
            .reduced(2)
    );

    pauseButton.setBounds(
        transportArea
            .removeFromLeft(80)
            .reduced(2)
    );

    stopButton.setBounds(
        transportArea
            .removeFromLeft(80)
            .reduced(2)
    );

    area.removeFromTop(4);


    // ==========================================================
    // SELECT
    // ==========================================================

    selectButton.setBounds(
        area.removeFromTop(32)
            .withSizeKeepingCentre(
                180,
                32
            )
    );

    area.removeFromTop(4);


    // ==========================================================
    // FILE
    // ==========================================================

    fileLabel.setBounds(
        area.removeFromTop(20)
    );

    area.removeFromTop(4);


    // ==========================================================
    // SEPARATE
    // ==========================================================

    separateButton.setBounds(
        area.removeFromTop(34)
            .withSizeKeepingCentre(
                150,
                34
            )
    );

    area.removeFromTop(4);


    // ==========================================================
    // STATUS
    // ==========================================================

    statusLabel.setBounds(
        area.removeFromTop(18)
    );

    area.removeFromTop(5);


    // ==========================================================
    // STEM TRACKS
    // ==========================================================

    const int trackHeight = 58;


    // ----------------------------------------------------------
    // VOCALS
    // ----------------------------------------------------------

    auto vocalsArea =
        area.removeFromTop(trackHeight);

    vocalsTrack.setBounds(
        vocalsArea
    );

    vocalsLabel.setBounds(
        vocalsArea
            .withTrimmedLeft(8)
            .withTrimmedTop(2)
            .withHeight(18)
    );

    area.removeFromTop(4);


    // ----------------------------------------------------------
    // DRUMS
    // ----------------------------------------------------------

    auto drumsArea =
        area.removeFromTop(trackHeight);

    drumsTrack.setBounds(
        drumsArea
    );

    drumsLabel.setBounds(
        drumsArea
            .withTrimmedLeft(8)
            .withTrimmedTop(2)
            .withHeight(18)
    );

    area.removeFromTop(4);


    // ----------------------------------------------------------
    // BASS
    // ----------------------------------------------------------

    auto bassArea =
        area.removeFromTop(trackHeight);

    bassTrack.setBounds(
        bassArea
    );

    bassLabel.setBounds(
        bassArea
            .withTrimmedLeft(8)
            .withTrimmedTop(2)
            .withHeight(18)
    );

    area.removeFromTop(4);


    // ----------------------------------------------------------
    // INSTRUMENTAL
    // ----------------------------------------------------------

    auto instrumentalArea =
        area.removeFromTop(trackHeight);

    instrumentalTrack.setBounds(
        instrumentalArea
    );

    instrumentalLabel.setBounds(
        instrumentalArea
            .withTrimmedLeft(8)
            .withTrimmedTop(2)
            .withHeight(18)
    );

    area.removeFromTop(5);


    // ==========================================================
    // OUTPUT FOLDER
    // ==========================================================

    openOutputButton.setBounds(
        area.removeFromTop(30)
            .withSizeKeepingCentre(
                190,
                30
            )
    );
}

// ==============================================================
// STATUS
// ==============================================================

void MainComponent::setStatus(
    const juce::String& message
)
{
    statusLabel.setText(
        message,
        juce::dontSendNotification
    );
}

// ==============================================================
// UPDATE TRACK MIXING
// ==============================================================

void MainComponent::updateTrackMixing()
{
    const bool vocalsSolo =
        vocalsTrack.isSoloed();

    const bool drumsSolo =
        drumsTrack.isSoloed();

    const bool bassSolo =
        bassTrack.isSoloed();

    const bool instrumentalSolo =
        instrumentalTrack.isSoloed();

    const bool anySolo =
        vocalsSolo
        || drumsSolo
        || bassSolo
        || instrumentalSolo;


    // ==========================================================
    // VOCALS
    // ==========================================================

    if (vocalsTransport)
    {
        const bool audible =
            anySolo
                ? vocalsSolo
                : !vocalsTrack.isMuted();

        vocalsTransport->setGain(
            audible ? vocalsTrack.getVolume() : 0.0f
        );
    }


    // ==========================================================
    // DRUMS
    // ==========================================================

    if (drumsTransport)
    {
        const bool audible =
            anySolo
                ? drumsSolo
                : !drumsTrack.isMuted();

        drumsTransport->setGain(
            audible ? drumsTrack.getVolume() : 0.0f
        );
    }


    // ==========================================================
    // BASS
    // ==========================================================

    if (bassTransport)
    {
        const bool audible =
            anySolo
                ? bassSolo
                : !bassTrack.isMuted();

        bassTransport->setGain(
            audible ? bassTrack.getVolume() : 0.0f
        );
    }


    // ==========================================================
    // INSTRUMENTAL
    // ==========================================================

    if (instrumentalTransport)
    {
        const bool audible =
            anySolo
                ? instrumentalSolo
                : !instrumentalTrack.isMuted();

        instrumentalTransport->setGain(
            audible ? instrumentalTrack.getVolume() : 0.0f
        );
    }
}

// ==============================================================
// CLEAR STEMS
// ==============================================================

void MainComponent::clearSeparatedStems()
{
    vocalsLabel.setVisible(false);

    drumsLabel.setVisible(false);

    bassLabel.setVisible(false);

    instrumentalLabel.setVisible(false);

    openOutputButton.setVisible(false);


    vocalsTrack.setVisible(false);

    drumsTrack.setVisible(false);

    bassTrack.setVisible(false);

    instrumentalTrack.setVisible(false);


    unloadStemAudio();
}


// ==============================================================
// SHOW STEMS
// ==============================================================

void MainComponent::showSeparatedStems()
{
    if (!outputFolder.isDirectory())
        return;


    // ==========================================================
    // Locate stems
    // ==========================================================

    vocalsFile =
        outputFolder.getChildFile(
            "01_Vocals.wav"
        );


    drumsFile =
        outputFolder.getChildFile(
            "02_Drums.wav"
        );


    bassFile =
        outputFolder.getChildFile(
            "03_Bass.wav"
        );


    instrumentalFile =
        outputFolder.getChildFile(
            "04_Instrumental.wav"
        );


    // ==========================================================
    // Verify stems
    // ==========================================================

    if (!vocalsFile.existsAsFile()
        || !drumsFile.existsAsFile()
        || !bassFile.existsAsFile()
        || !instrumentalFile.existsAsFile())
    {
        setStatus(
            "ERROR: Stem files not found"
        );

        return;
    }


    // ==========================================================
    // Load waveform data
    // ==========================================================

    vocalsTrack.setAudioFile(
        vocalsFile
    );


    drumsTrack.setAudioFile(
        drumsFile
    );


    bassTrack.setAudioFile(
        bassFile
    );


    instrumentalTrack.setAudioFile(
        instrumentalFile
    );


    // ==========================================================
    // Load actual audio playback
    // ==========================================================

    if (!loadStemAudio())
    {
        setStatus(
            "ERROR: Could not load stem audio"
        );

        return;
    }


    // ==========================================================
    // Get timeline length
    // ==========================================================

    playbackLength =
        vocalsTrack.getAudioLength();


    playbackPosition = 0.0;


    // ==========================================================
    // Reset playhead
    // ==========================================================

    vocalsTrack.setPlayheadPosition(
        playbackPosition
    );


    drumsTrack.setPlayheadPosition(
        playbackPosition
    );


    bassTrack.setPlayheadPosition(
        playbackPosition
    );


    instrumentalTrack.setPlayheadPosition(
        playbackPosition
    );


    // ==========================================================
    // Show tracks
    // ==========================================================

    vocalsLabel.setVisible(true);

    drumsLabel.setVisible(true);

    bassLabel.setVisible(true);

    instrumentalLabel.setVisible(true);


    vocalsTrack.setVisible(true);

    drumsTrack.setVisible(true);

    bassTrack.setVisible(true);

    instrumentalTrack.setVisible(true);


    openOutputButton.setVisible(true);


    // ==========================================================
    // Expand window for tracks
    // ==========================================================

    expandForTracks();


    // ==========================================================
    // Refresh layout
    // ==========================================================

    resized();


    // ==========================================================
    // Status
    // ==========================================================

    setStatus(
        "Status: Separation complete"
    );


    repaint();
}


// ==============================================================
// LOAD STEM AUDIO
// ==============================================================

bool MainComponent::loadStemAudio()
{
    // ==========================================================
    // Remove old audio
    // ==========================================================

    unloadStemAudio();


    // ==========================================================
    // Create readers
    // ==========================================================

    auto* vocalsReader =
        formatManager.createReaderFor(
            vocalsFile
        );


    auto* drumsReader =
        formatManager.createReaderFor(
            drumsFile
        );


    auto* bassReader =
        formatManager.createReaderFor(
            bassFile
        );


    auto* instrumentalReader =
        formatManager.createReaderFor(
            instrumentalFile
        );


    if (vocalsReader == nullptr
        || drumsReader == nullptr
        || bassReader == nullptr
        || instrumentalReader == nullptr)
    {
        delete vocalsReader;

        delete drumsReader;

        delete bassReader;

        delete instrumentalReader;

        return false;
    }


    // ==========================================================
    // Create reader sources
    // ==========================================================

    vocalsReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            vocalsReader,
            true
        );


    drumsReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            drumsReader,
            true
        );


    bassReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            bassReader,
            true
        );


    instrumentalReaderSource =
        std::make_unique<
            juce::AudioFormatReaderSource
        >(
            instrumentalReader,
            true
        );


    // ==========================================================
    // Create transport sources
    // ==========================================================

    vocalsTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();


    drumsTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();


    bassTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();


    instrumentalTransport =
        std::make_unique<
            juce::AudioTransportSource
        >();


    // ==========================================================
    // Connect readers to transports
    // ==========================================================

    vocalsTransport->setSource(
        vocalsReaderSource.get()
    );


    drumsTransport->setSource(
        drumsReaderSource.get()
    );


    bassTransport->setSource(
        bassReaderSource.get()
    );


    instrumentalTransport->setSource(
        instrumentalReaderSource.get()
    );


    // ==========================================================
    // Add all four stems to mixer
    // ==========================================================

    mixerSource.addInputSource(
        vocalsTransport.get(),
        false
    );


    mixerSource.addInputSource(
        drumsTransport.get(),
        false
    );


    mixerSource.addInputSource(
        bassTransport.get(),
        false
    );


    mixerSource.addInputSource(
        instrumentalTransport.get(),
        false
    );


    return true;
}


// ==============================================================
// UNLOAD STEM AUDIO
// ==============================================================

void MainComponent::unloadStemAudio()
{
    mixerSource.removeAllInputs();


    if (vocalsTransport)
        vocalsTransport->stop();


    if (drumsTransport)
        drumsTransport->stop();


    if (bassTransport)
        bassTransport->stop();


    if (instrumentalTransport)
        instrumentalTransport->stop();


    vocalsTransport.reset();

    drumsTransport.reset();

    bassTransport.reset();

    instrumentalTransport.reset();


    vocalsReaderSource.reset();

    drumsReaderSource.reset();

    bassReaderSource.reset();

    instrumentalReaderSource.reset();
}


// ==============================================================
// OPEN OUTPUT FOLDER
// ==============================================================

void MainComponent::openOutputFolder()
{
    if (outputFolder.isDirectory())
    {
        outputFolder.revealToUser();
    }
}


// ==============================================================
// FILE PICKER
// ==============================================================

void MainComponent::selectAudioFile()
{
    auto chooser =
        std::make_shared<
            juce::FileChooser
        >(
            "Select audio file...",
            juce::File{},
            "*.mp3;*.wav;*.flac"
        );


    chooser->launchAsync(
        juce::FileBrowserComponent::openMode
        |
        juce::FileBrowserComponent::canSelectFiles,

        [this, chooser]
        (const juce::FileChooser&)
        {
            auto file =
                chooser->getResult();


            if (file.existsAsFile())
            {
                selectedFile =
                    file;


                fileLabel.setText(
                    file.getFileName(),
                    juce::dontSendNotification
                );


                setStatus(
                    "Status: Ready"
                );


                clearSeparatedStems();
            }
        }
    );
}


// ==============================================================
// RUN SEPARATOR
// ==============================================================

void MainComponent::runSeparator()
{
    if (!selectedFile.existsAsFile())
    {
        setStatus(
            "Please select an audio file first"
        );

        return;
    }


    // ==========================================================
    // Prevent duplicate process
    // ==========================================================

    if (separatorProcess != nullptr)
    {
        if (separatorProcess->isRunning())
        {
            setStatus(
                "Status: Separation already running..."
            );

            return;
        }


        separatorProcess.reset();
    }


    // ==========================================================
    // Python
    // ==========================================================

    const juce::File pythonExecutable(
        "C:\\AudioDevelopment\\OfforAudioDev"
        "\\OfforAI\\demucs-env\\Scripts\\python.exe"
    );


    // ==========================================================
    // separator.py
    // ==========================================================

    const juce::File separatorScript(
        "C:\\AudioDevelopment\\OfforAudioDev"
        "\\OfforAI\\demucs-engine\\separator.py"
    );


    if (!pythonExecutable.existsAsFile())
    {
        setStatus(
            "ERROR: Python environment not found"
        );

        return;
    }


    if (!separatorScript.existsAsFile())
    {
        setStatus(
            "ERROR: separator.py not found"
        );

        return;
    }


    // ==========================================================
    // Command
    // ==========================================================

    juce::StringArray command;


    command.add(
        pythonExecutable.getFullPathName()
    );


    command.add(
        separatorScript.getFullPathName()
    );


    command.add(
        selectedFile.getFullPathName()
    );


    // ==========================================================
    // Create process
    // ==========================================================

    separatorProcess =
        std::make_unique<
            juce::ChildProcess
        >();


    setStatus(
        "Status: Starting Demucs..."
    );


    const bool started =
        separatorProcess->start(
            command
        );


    if (!started)
    {
        separatorProcess.reset();


        setStatus(
            "ERROR: Could not start Python"
        );


        return;
    }


    startTimer(
        250
    );
}


// ==============================================================
// TIMER
// ==============================================================

void MainComponent::timerCallback()
{
    checkSeparatorProcess();
}


// ==============================================================
// CHECK PROCESS
// ==============================================================

void MainComponent::checkSeparatorProcess()
{
    if (separatorProcess == nullptr)
    {
        stopTimer();

        return;
    }


    if (separatorProcess->isRunning())
    {
        setStatus(
            "Status: Separating..."
        );

        return;
    }


    stopTimer();


    const int exitCode =
        separatorProcess->getExitCode();


    if (exitCode == 0)
    {
        const auto songName =
            selectedFile
                .getFileNameWithoutExtension();


        outputFolder =
            juce::File(
                "C:\\AudioDevelopment\\OfforAudioDev"
                "\\OfforAI\\output"
            )
            .getChildFile(
                songName
            );


        showSeparatedStems();
    }
    else
    {
        setStatus(
            "ERROR: Demucs returned error"
        );
    }


    separatorProcess.reset();
}


// ==============================================================
// START PLAYBACK
// ==============================================================

void MainComponent::startPlayback()
{
    if (playbackLength <= 0.0)
    {
        setStatus(
            "No audio loaded"
        );

        return;
    }


    if (!vocalsTransport
        || !drumsTransport
        || !bassTransport
        || !instrumentalTransport)
    {
        setStatus(
            "ERROR: Audio engine not loaded"
        );

        return;
    }


    // ==========================================================
    // Reset if at end
    // ==========================================================

    if (playbackPosition >= playbackLength)
    {
        playbackPosition = 0.0;


        vocalsTransport->setPosition(
            0.0
        );


        drumsTransport->setPosition(
            0.0
        );


        bassTransport->setPosition(
            0.0
        );


        instrumentalTransport->setPosition(
            0.0
        );
    }


    // ==========================================================
    // Synchronise all stems
    // ==========================================================

    vocalsTransport->setPosition(
        playbackPosition
    );


    drumsTransport->setPosition(
        playbackPosition
    );


    bassTransport->setPosition(
        playbackPosition
    );


    instrumentalTransport->setPosition(
        playbackPosition
    );


    // ==========================================================
    // Start all stems
    // ==========================================================

    vocalsTransport->start();

    drumsTransport->start();

    bassTransport->start();

    instrumentalTransport->start();


    isPlaying = true;


    playbackTimer.startTimerHz(
        60
    );


    setStatus(
        "Status: Playing"
    );
}


// ==============================================================
// PAUSE PLAYBACK
// ==============================================================

void MainComponent::pausePlayback()
{
    isPlaying = false;


    if (vocalsTransport)
        vocalsTransport->stop();


    if (drumsTransport)
        drumsTransport->stop();


    if (bassTransport)
        bassTransport->stop();


    if (instrumentalTransport)
        instrumentalTransport->stop();


    playbackTimer.stopTimer();


    setStatus(
        "Status: Paused"
    );
}


// ==============================================================
// STOP PLAYBACK
// ==============================================================

void MainComponent::stopPlayback()
{
    isPlaying = false;


    playbackTimer.stopTimer();


    if (vocalsTransport)
    {
        vocalsTransport->stop();

        vocalsTransport->setPosition(
            0.0
        );
    }


    if (drumsTransport)
    {
        drumsTransport->stop();

        drumsTransport->setPosition(
            0.0
        );
    }


    if (bassTransport)
    {
        bassTransport->stop();

        bassTransport->setPosition(
            0.0
        );
    }


    if (instrumentalTransport)
    {
        instrumentalTransport->stop();

        instrumentalTransport->setPosition(
            0.0
        );
    }


    playbackPosition = 0.0;


    vocalsTrack.setPlayheadPosition(
        0.0
    );


    drumsTrack.setPlayheadPosition(
        0.0
    );


    bassTrack.setPlayheadPosition(
        0.0
    );


    instrumentalTrack.setPlayheadPosition(
        0.0
    );


    setStatus(
        "Status: Stopped"
    );
}


// ==============================================================
// MASTER PLAYBACK UPDATE
// ==============================================================

void MainComponent::updatePlayback()
{
    updateTrackMixing();
    
    if (!isPlaying)
        return;


    playbackPosition +=
        1.0 / 60.0;


    if (playbackPosition >= playbackLength)
    {
        playbackPosition =
            playbackLength;


        isPlaying = false;


        playbackTimer.stopTimer();


        if (vocalsTransport)
            vocalsTransport->stop();


        if (drumsTransport)
            drumsTransport->stop();


        if (bassTransport)
            bassTransport->stop();


        if (instrumentalTransport)
            instrumentalTransport->stop();


        setStatus(
            "Status: Playback complete"
        );
    }


    // ==========================================================
    // Move visual playheads
    // ==========================================================

    vocalsTrack.setPlayheadPosition(
        playbackPosition
    );


    drumsTrack.setPlayheadPosition(
        playbackPosition
    );


    bassTrack.setPlayheadPosition(
        playbackPosition
    );


    instrumentalTrack.setPlayheadPosition(
        playbackPosition
    );
}


void MainComponent::expandForTracks()
{
    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
    {
        window->setSize(
            700,
            550
        );
    }
}