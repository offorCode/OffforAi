#include "MainComponent.h"
#include "AppConfig.h"


//==============================================================
// MainComponent
//==============================================================

MainComponent::MainComponent()
{
    setSize(
        700,
        450
    );


    //==========================================================
    // Title
    //==========================================================

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


    //==========================================================
    // Version
    //==========================================================

    versionLabel.setText(
        "v" + juce::String(
            OfforStemSplitter::VERSION
        ),
        juce::dontSendNotification
    );

    versionLabel.setFont(
        juce::Font(14.0f)
    );

    versionLabel.setJustificationType(
        juce::Justification::centred
    );

    versionLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::grey
    );

    addAndMakeVisible(
        versionLabel
    );


    //==========================================================
    // Select Button
    //==========================================================

    addAndMakeVisible(
        selectButton
    );

    selectButton.onClick = [this]
    {
        selectAudioFile();
    };


    //==========================================================
    // Separate Button
    //==========================================================

    addAndMakeVisible(
        separateButton
    );

    separateButton.onClick = [this]
    {
        runSeparator();
    };


    //==========================================================
    // File Label
    //==========================================================

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


    //==========================================================
    // Status Label
    //==========================================================

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
}


//==============================================================
// Destructor
//==============================================================

MainComponent::~MainComponent()
{
    if (separationThread != nullptr)
    {
        separationThread->stopThread(
            5000
        );

        separationThread.reset();
    }
}


//==============================================================
// Paint
//==============================================================

void MainComponent::paint(
    juce::Graphics& g
)
{
    g.fillAll(
        juce::Colours::black
    );
}


//==============================================================
// Resized
//==============================================================

void MainComponent::resized()
{
    auto area = getLocalBounds()
        .reduced(40);


    //==========================================================
    // Title
    //==========================================================

    titleLabel.setBounds(
        area.removeFromTop(60)
    );


    //==========================================================
    // Version
    //==========================================================

    versionLabel.setBounds(
        area.removeFromTop(25)
    );


    area.removeFromTop(20);


    //==========================================================
    // Select Button
    //==========================================================

    selectButton.setBounds(
        area.removeFromTop(40)
            .withSizeKeepingCentre(
                220,
                40
            )
    );


    area.removeFromTop(20);


    //==========================================================
    // File Label
    //==========================================================

    fileLabel.setBounds(
        area.removeFromTop(40)
    );


    area.removeFromTop(20);


    //==========================================================
    // Separate Button
    //==========================================================

    separateButton.setBounds(
        area.removeFromTop(50)
            .withSizeKeepingCentre(
                180,
                45
            )
    );


    area.removeFromTop(30);


    //==========================================================
    // Status
    //==========================================================

    statusLabel.setBounds(
        area.removeFromTop(40)
    );
}


//==============================================================
// Select Audio File
//==============================================================

void MainComponent::selectAudioFile()
{
    auto chooser =
        std::make_shared<juce::FileChooser>(
            "Select audio file...",
            juce::File{},
            "*.mp3;*.wav;*.flac;*.ogg"
        );


    chooser->launchAsync(
        juce::FileBrowserComponent::openMode
        |
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


//==============================================================
// Find Project Root
//==============================================================

juce::File MainComponent::getProjectRoot() const
{
    /*
        Development layout:

        OfforAI
        │
        ├── demucs-engine
        │   └── separator.py
        │
        ├── demucs-env
        │   └── Scripts
        │       └── python.exe
        │
        └── OfforStemSplitter
            └── build
                └── OfforStemSplitter_artefacts
                    └── Release
                        └── Offor Stem Splitter.exe
    */

    auto executable =
        juce::File::getSpecialLocation(
            juce::File::currentExecutableFile
        );


    auto releaseFolder =
        executable.getParentDirectory();


    auto artefactsFolder =
        releaseFolder.getParentDirectory();


    auto buildFolder =
        artefactsFolder.getParentDirectory();


    auto projectFolder =
        buildFolder.getParentDirectory();


    return projectFolder.getParentDirectory();
}


//==============================================================
// Python Executable
//==============================================================

juce::File MainComponent::getPythonExecutable() const
{
    auto projectRoot =
        getProjectRoot();


    return projectRoot
        .getChildFile("demucs-env")
        .getChildFile("Scripts")
        .getChildFile("python.exe");
}


//==============================================================
// Separator Script
//==============================================================

juce::File MainComponent::getSeparatorScript() const
{
    auto projectRoot =
        getProjectRoot();


    return projectRoot
        .getChildFile("demucs-engine")
        .getChildFile("separator.py");
}


//==============================================================
// Run Separator
//==============================================================

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


    //==========================================================
    // Prevent multiple separation jobs
    //==========================================================

    if (separationThread != nullptr)
    {
        if (separationThread->isThreadRunning())
        {
            statusLabel.setText(
                "Status: Separation already running...",
                juce::dontSendNotification
            );

            return;
        }

        separationThread.reset();
    }


    //==========================================================
    // Check Python
    //==========================================================

    auto python =
        getPythonExecutable();


    if (!python.existsAsFile())
    {
        statusLabel.setText(
            "Error: Python environment not found",
            juce::dontSendNotification
        );

        return;
    }


    //==========================================================
    // Check separator.py
    //==========================================================

    auto separator =
        getSeparatorScript();


    if (!separator.existsAsFile())
    {
        statusLabel.setText(
            "Error: separator.py not found",
            juce::dontSendNotification
        );

        return;
    }


    //==========================================================
    // Update UI
    //==========================================================

    statusLabel.setText(
        "Status: Separating...",
        juce::dontSendNotification
    );


    selectButton.setEnabled(
        false
    );

    separateButton.setEnabled(
        false
    );


    //==========================================================
    // Start background thread
    //==========================================================

    separationThread =
        std::make_unique<SeparationThread>(
            *this,
            selectedFile
        );


    separationThread->startThread();
}


//==============================================================
// Separation Finished
//==============================================================

void MainComponent::separationFinished(
    bool success,
    const juce::String& message
)
{
    selectButton.setEnabled(
        true
    );

    separateButton.setEnabled(
        true
    );


    if (success)
    {
        statusLabel.setText(
            "Status: Separation complete",
            juce::dontSendNotification
        );

        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon,
            "Offor Stem Splitter",
            message
        );
    }
    else
    {
        statusLabel.setText(
            "Status: Separation failed",
            juce::dontSendNotification
        );

        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Offor Stem Splitter",
            message
        );
    }
}


//==============================================================
// SeparationThread Constructor
//==============================================================

MainComponent::SeparationThread::SeparationThread(
    MainComponent& ownerToUse,
    const juce::File& audioFileToUse
)
    : juce::Thread("Offor Demucs Separation"),
      owner(ownerToUse),
      audioFile(audioFileToUse)
{
}


//==============================================================
// SeparationThread Destructor
//==============================================================

MainComponent::SeparationThread::~SeparationThread()
{
    stopThread(
        5000
    );
}


//==============================================================
// SeparationThread Run
//==============================================================

void MainComponent::SeparationThread::run()
{
    auto python =
        owner.getPythonExecutable();


    auto separator =
        owner.getSeparatorScript();


    //==========================================================
    // Build Python command
    //==========================================================

    juce::StringArray command;


    command.add(
        python.getFullPathName()
    );

    command.add(
        separator.getFullPathName()
    );

    command.add(
        audioFile.getFullPathName()
    );


    //==========================================================
    // Start Python process
    //==========================================================

    juce::ChildProcess process;


    if (!process.start(
            command,
            juce::ChildProcess::wantStdOut
            |
            juce::ChildProcess::wantStdErr
        ))
    {
        juce::Component::SafePointer<MainComponent>
            safeOwner(&owner);


        juce::MessageManager::callAsync(
            [safeOwner]
            {
                if (safeOwner != nullptr)
                {
                    safeOwner->separationFinished(
                        false,
                        "Could not start the Demucs Python process."
                    );
                }
            }
        );


        return;
    }


    //==========================================================
    // Wait for process
    //==========================================================

    while (!threadShouldExit())
    {
        if (process.waitForProcessToFinish(500))
            break;
    }


    //==========================================================
    // If JUCE is shutting down, stop the process
    //==========================================================

    if (threadShouldExit())
    {
        if (process.isRunning())
            process.kill();


        return;
    }


    //==========================================================
    // Read Python output
    //==========================================================


   juce::String processOutput =
    process.readAllProcessOutput();

    //==========================================================
    // Check result
    //==========================================================

    const int exitCode =
        process.getExitCode();


    const bool success =
        exitCode == 0;


    juce::Component::SafePointer<MainComponent>
        safeOwner(&owner);


    if (success)
    {
        auto message =
            "The stems have been separated successfully.\n\n"
            "Output:\n"
            + processOutput.trim();


        juce::MessageManager::callAsync(
            [safeOwner, message]
            {
                if (safeOwner != nullptr)
                {
                    safeOwner->separationFinished(
                        true,
                        message
                    );
                }
            }
        );
    }
    else
    {
        auto message =
            "Demucs returned an error.\n\n"
            + processOutput.trim();


        juce::MessageManager::callAsync(
            [safeOwner, message]
            {
                if (safeOwner != nullptr)
                {
                    safeOwner->separationFinished(
                        false,
                        message
                    );
                }
            }
        );
    }
}

