#include "MainComponent.h"
#include "AppConfig.h"


// ==============================================================
// CONSTRUCTOR
// ==============================================================

MainComponent::MainComponent()
{
    setSize(
        700,
        600
    );


    // ==========================================================
    // TITLE
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
    // SELECT BUTTON
    // ==========================================================

    addAndMakeVisible(
        selectButton
    );

    selectButton.onClick = [this]
    {
        selectAudioFile();
    };


    // ==========================================================
    // SEPARATE BUTTON
    // ==========================================================

    addAndMakeVisible(
        separateButton
    );

    separateButton.onClick = [this]
    {
        runSeparator();
    };


    // ==========================================================
    // FILE LABEL
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
    // STATUS
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
    // STEM LABELS
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


    addAndMakeVisible(
        vocalsLabel
    );

    addAndMakeVisible(
        drumsLabel
    );

    addAndMakeVisible(
        bassLabel
    );

    addAndMakeVisible(
        instrumentalLabel
    );


    // ==========================================================
    // OUTPUT BUTTON
    // ==========================================================

    addAndMakeVisible(
        openOutputButton
    );

    openOutputButton.onClick = [this]
    {
        openOutputFolder();
    };


    // Initially hide output controls.
    clearSeparatedStems();
}


// ==============================================================
// DESTRUCTOR
// ==============================================================

MainComponent::~MainComponent()
{
    stopTimer();

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
        getLocalBounds().reduced(40);


    // ----------------------------------------------------------
    // Title
    // ----------------------------------------------------------

    titleLabel.setBounds(
        area.removeFromTop(60)
    );


    area.removeFromTop(20);


    // ----------------------------------------------------------
    // Select
    // ----------------------------------------------------------

    selectButton.setBounds(
        area.removeFromTop(40)
            .withSizeKeepingCentre(
                220,
                40
            )
    );


    area.removeFromTop(15);


    // ----------------------------------------------------------
    // File
    // ----------------------------------------------------------

    fileLabel.setBounds(
        area.removeFromTop(35)
    );


    area.removeFromTop(15);


    // ----------------------------------------------------------
    // Separate
    // ----------------------------------------------------------

    separateButton.setBounds(
        area.removeFromTop(45)
            .withSizeKeepingCentre(
                180,
                45
            )
    );


    area.removeFromTop(20);


    // ----------------------------------------------------------
    // Status
    // ----------------------------------------------------------

    statusLabel.setBounds(
        area.removeFromTop(35)
    );


    area.removeFromTop(20);


    // ----------------------------------------------------------
    // Stem area
    // ----------------------------------------------------------

    vocalsLabel.setBounds(
        area.removeFromTop(35)
    );

    drumsLabel.setBounds(
        area.removeFromTop(35)
    );

    bassLabel.setBounds(
        area.removeFromTop(35)
    );

    instrumentalLabel.setBounds(
        area.removeFromTop(35)
    );


    area.removeFromTop(15);


    // ----------------------------------------------------------
    // Output button
    // ----------------------------------------------------------

    openOutputButton.setBounds(
        area.removeFromTop(40)
            .withSizeKeepingCentre(
                220,
                40
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
// CLEAR STEMS
// ==============================================================

void MainComponent::clearSeparatedStems()
{
    vocalsLabel.setVisible(false);
    drumsLabel.setVisible(false);
    bassLabel.setVisible(false);
    instrumentalLabel.setVisible(false);

    openOutputButton.setVisible(false);
}


// ==============================================================
// SHOW STEMS
// ==============================================================

void MainComponent::showSeparatedStems()
{
    if (!outputFolder.isDirectory())
    {
        setStatus(
            "ERROR: Output folder not found"
        );

        return;
    }


    const auto vocals =
        outputFolder.getChildFile(
            "01_Vocals.wav"
        );

    const auto drums =
        outputFolder.getChildFile(
            "02_Drums.wav"
        );

    const auto bass =
        outputFolder.getChildFile(
            "03_Bass.wav"
        );

    const auto instrumental =
        outputFolder.getChildFile(
            "04_Instrumental.wav"
        );


    // ----------------------------------------------------------
    // Verify files
    // ----------------------------------------------------------

    if (!vocals.existsAsFile() ||
        !drums.existsAsFile() ||
        !bass.existsAsFile() ||
        !instrumental.existsAsFile())
    {
        setStatus(
            "ERROR: One or more stems are missing"
        );

        return;
    }


    // ----------------------------------------------------------
    // Display
    // ----------------------------------------------------------

    vocalsLabel.setText(
        "01  Vocals",
        juce::dontSendNotification
    );

    drumsLabel.setText(
        "02  Drums",
        juce::dontSendNotification
    );

    bassLabel.setText(
        "03  Bass",
        juce::dontSendNotification
    );

    instrumentalLabel.setText(
        "04  Instrumental",
        juce::dontSendNotification
    );


    vocalsLabel.setVisible(true);
    drumsLabel.setVisible(true);
    bassLabel.setVisible(true);
    instrumentalLabel.setVisible(true);

    openOutputButton.setVisible(true);


    resized();
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
        std::make_shared<juce::FileChooser>(
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


    // ----------------------------------------------------------
    // Prevent duplicate processes
    // ----------------------------------------------------------

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


    // ----------------------------------------------------------
    // Python
    // ----------------------------------------------------------

    const juce::File pythonExecutable(
        "C:\\AudioDevelopment\\OfforAudioDev"
        "\\OfforAI\\demucs-env\\Scripts\\python.exe"
    );


    // ----------------------------------------------------------
    // separator.py
    // ----------------------------------------------------------

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


    // ----------------------------------------------------------
    // Build command
    // ----------------------------------------------------------

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


    // ----------------------------------------------------------
    // Create process
    // ----------------------------------------------------------

    separatorProcess =
        std::make_unique<juce::ChildProcess>();


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


    // ----------------------------------------------------------
    // Monitor
    // ----------------------------------------------------------

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
        // ------------------------------------------------------
        // Build output path
        //
        // separator.py creates:
        //
        // output\<song-name>
        // ------------------------------------------------------

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


        setStatus(
            "Status: Separation complete"
        );
    }
    else
    {
        setStatus(
            "ERROR: Demucs returned error"
        );
    }


    separatorProcess.reset();
}
