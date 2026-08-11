#pragma once

#include <JuceHeader.h>

#include <atomic>

#include <memory>

#include "AppConfig.h"
#include "StemTrackComponent.h"
#include "SettingsComponent.h"
#include "LicenseManager.h"


// ============================================================
// OFFOR STEM SPLITTER
// Main Application Component
// ============================================================

class MainComponent
    : public juce::Component,
      public juce::Timer,
      public juce::FileDragAndDropTarget,
      public juce::KeyListener
{
public:

    // ========================================================
    // Constructor / Destructor
    // ========================================================

    MainComponent();

    ~MainComponent() override;


    // ========================================================
    // JUCE Component
    // ========================================================

    void paint(
        juce::Graphics& g
    ) override;

    void resized() override;

    void mouseDown(
        const juce::MouseEvent& event
    ) override;

    juce::MouseCursor getMouseCursor() override;


    // ========================================================
    // Timer
    // ========================================================

    void timerCallback() override;


    // ========================================================
    // Drag & Drop
    // ========================================================

    bool isInterestedInFileDrag(
        const juce::StringArray& files
    ) override;

    void fileDragEnter(
        const juce::StringArray& files,
        int x,
        int y
    ) override;

    void fileDragExit(
        const juce::StringArray& files
    ) override;

    void filesDropped(
        const juce::StringArray& files,
        int x,
        int y
    ) override;


    // ========================================================
    // Keyboard
    // ========================================================

    bool keyPressed(
        const juce::KeyPress& key,
        juce::Component* originatingComponent
    ) override;


    // void focusGained(
    //     FocusChangeType cause
    // ) override;

private:

    // ========================================================
    // LICENSE
    // ========================================================

    LicenseManager licenseManager;

    void checkLicenseBeforeSeparation();

    void showLicenseDialog();

    std::unique_ptr<juce::DialogWindow> licenseWindow;



    // ========================================================
    // EXPORT FORMAT
    // ========================================================

    enum class ExportFormat
    {
        wav,
        flac,
        aiff
    };


    // ========================================================
    // AUDIO
    // ========================================================

    juce::AudioFormatManager formatManager;

    juce::AudioDeviceManager audioDeviceManager;

    juce::AudioSourcePlayer audioSourcePlayer;

    juce::MixerAudioSource mixerSource;


    // ========================================================
    // STEM AUDIO
    // ========================================================

    std::unique_ptr<
        juce::AudioFormatReaderSource
    > vocalsReaderSource;

    std::unique_ptr<
        juce::AudioFormatReaderSource
    > drumsReaderSource;

    std::unique_ptr<
        juce::AudioFormatReaderSource
    > bassReaderSource;

    std::unique_ptr<
        juce::AudioFormatReaderSource
    > instrumentalReaderSource;


    std::unique_ptr<
        juce::AudioTransportSource
    > vocalsTransport;

    std::unique_ptr<
        juce::AudioTransportSource
    > drumsTransport;

    std::unique_ptr<
        juce::AudioTransportSource
    > bassTransport;

    std::unique_ptr<
        juce::AudioTransportSource
    > instrumentalTransport;


    // ========================================================
    // STEM FILES
    // ========================================================

    juce::File vocalsFile;

    juce::File drumsFile;

    juce::File bassFile;

    juce::File instrumentalFile;


    // ========================================================
    // SELECTED AUDIO
    // ========================================================

    juce::File selectedFile;

    juce::File outputFolder;


    // ========================================================
    // UI
    // ========================================================

    juce::Label headerTitleLabel;

    juce::Label fileLabel;

    juce::Label statusLabel;

    juce::Label progressLabel;

    juce::Label companyLabel;

    juce::Label versionLabel;


    // ========================================================
    // LOGO
    // ========================================================

    juce::Image offorLogo;



    // ========================================================
    // CANCEL
    // ========================================================

    juce::TextButton cancelButton;


    // ========================================================
    // TRANSPORT
    // ========================================================

    juce::TextButton playButton {
        "PLAY"
    };

    juce::TextButton pauseButton {
        "PAUSE"
    };

    juce::TextButton stopButton {
        "STOP"
    };


    // ========================================================
    // SETTINGS
    // ========================================================

    juce::TextButton settingsButton
    {
        "SETTINGS"
    };

    std::unique_ptr<juce::DialogWindow> settingsWindow;

    std::unique_ptr<SettingsComponent> settingsComponent;


    // ========================================================
    // EXPORT
    // ========================================================

    // juce::ComboBox exportMenuBox;

    // juce::ComboBox exportFormatBox;

    // juce::ToggleButton normalizeButton {
    //     "Normalize"
    // };


    // ========================================================
    // STEM EXPORT BUTTONS
    // ========================================================

    juce::TextButton exportSelectionButton {
        "Export Selection"
    };

    juce::TextButton exportAllButton {
        "Export All Stems"
    };

    juce::TextButton openOutputButton {
        "Open Output Folder"
    };


    // ========================================================
    // EXPORT SECTION
    // ========================================================

    // juce::TextButton exportSectionButton {
    //     "EXPORT OPTIONS"
    // };

    // bool exportSectionExpanded = false;

    // bool exportControlsVisible = false;

    // juce::Rectangle<int> exportSectionBounds;


    // ========================================================
    // DROP ZONE
    // ========================================================

    juce::Rectangle<int> dropZoneBounds;

    bool isDragOver = false;


    // ========================================================
    // STEM TRACKS
    // ========================================================

    StemTrackComponent vocalsTrack {
        "Vocals"
    };

    StemTrackComponent drumsTrack {
        "Drums"
    };

    StemTrackComponent bassTrack {
        "Bass"
    };

    StemTrackComponent instrumentalTrack {
        "Instrumental"
    };


    // ========================================================
    // STEM LABELS
    // ========================================================

    juce::Label vocalsLabel;

    juce::Label drumsLabel;

    juce::Label bassLabel;

    juce::Label instrumentalLabel;


    // ========================================================
    // PLAYBACK
    // ========================================================

    class PlaybackTimer : public juce::Timer
    {
    public:
        explicit PlaybackTimer(MainComponent& owner)
            : owner(owner)
        {
        }

        void timerCallback() override
        {
            owner.updatePlayback();
        }

    private:
        MainComponent& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            PlaybackTimer
        );
    };

    std::unique_ptr<PlaybackTimer> playbackTimer;


    // ========================================================
    // PROGRESS TEXT ANIMATION
    // ========================================================

    class ProgressAnimationTimer : public juce::Timer
    {
    public:

        explicit ProgressAnimationTimer(
            MainComponent& owner
        )
            : owner(owner)
        {
        }

        void timerCallback() override
        {
            owner.updateSeparationAnimation();
        }

    private:

        MainComponent& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            ProgressAnimationTimer
        );
    };


    std::unique_ptr<ProgressAnimationTimer>
        progressAnimationTimer;

    bool isPlaying = false;

    double playbackPosition = 0.0;

    double playbackLength = 0.0;

    // ========================================================
    // PLAYBACK SETTINGS
    // ========================================================

    bool autoPlayEnabled = false;
    bool loopSelectionEnabled = false;

    juce::ToggleButton autoPlayButton {
        "Auto-play"
    };
    juce::ToggleButton loopSelectionButton {
        "Loop Selection"
    };

    // ========================================================
    // PLAYBACK SELECTION
    // ========================================================

    double playbackSelectionStart = 0.0;

    double playbackSelectionEnd = 0.0;

    bool playbackSelectionActive = false;


    // ========================================================
    // SEPARATOR PROCESS
    // ========================================================

    std::unique_ptr<
        juce::ChildProcess
    > separatorProcess;

    
    // ========================================================
    // LICENSE / INSTALLATION ID
    // ========================================================

    juce::String installationId;

    juce::String getInstallationId();

    void loadOrCreateInstallationId();


    // ========================================================
    // SEPARATOR OUTPUT THREAD
    // ========================================================

    class SeparatorOutputThread
        : public juce::Thread
    {
    public:

        explicit SeparatorOutputThread(
            MainComponent& owner
        )
            : juce::Thread(
                "Offor Separator Output"
            ),
              owner(owner)
        {
        }

        void run() override;

    private:

        MainComponent& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            SeparatorOutputThread
        );
    };


    std::unique_ptr<
        SeparatorOutputThread
    > separatorOutputThread;


    // ========================================================
    // SEPARATOR OUTPUT
    // ========================================================

    juce::String separatorOutputBuffer;

    juce::CriticalSection separatorOutputLock;


    // ========================================================
    // SEPARATION STATE
    // ========================================================

    bool separationRunning = false;

    // double separationProgress = 0.0;
    std::atomic<double> separationProgress { 0.0 };

    int progressAnimationFrame = 0;


    // ========================================================
    // SEPARATOR
    // ========================================================

    void runSeparator();

    void cancelSeparator();

    void startSeparatorOutputThread();

    void stopSeparatorOutputThread();

    void readSeparatorOutput();

    void parseSeparatorProgress(
        const juce::String& output
    );

    void checkSeparatorProcess();

    void updateSeparationAnimation();


    // ========================================================
    // AUDIO FILE
    // ========================================================

    void selectAudioFile();

    void loadSelectedFile(
        const juce::File& file
    );


    // ========================================================
    // STEM UI
    // ========================================================

    void clearSeparatedStems();

    void showSeparatedStems();

    void setStemUIVisible(
        bool shouldBeVisible
    );


    // ========================================================
    // AUDIO PLAYBACK
    // ========================================================

    bool loadStemAudio();

    void unloadStemAudio();

    void updateTrackMixing();

    void startPlayback();

    void pausePlayback();

    void stopPlayback();

    void updatePlayback();


    // ========================================================
    // SEEKING
    // ========================================================

    void setupSeekCallbacks();

    void setupStemTrackCallbacks();


    double getPlaybackStartPosition() const;

    double getPlaybackEndPosition() const;


    // ========================================================
    // TRANSPORT UI
    // ========================================================

    void setTransportVisible(
        bool shouldBeVisible
    );


    // ========================================================
    // STATUS
    // ========================================================

    void setStatus(
        const juce::String& message
    );


    // ========================================================
    // WINDOW
    // ========================================================

    void expandForTracks();

    void shrinkToInputWindow();


    // ========================================================
    // EXPORT
    // ========================================================

    void updateExportControlsVisibility();

    void updateExportSectionVisibility();

    void exportSelectedAudio();

    void exportAllStems();

    void openOutputFolder();


    // ========================================================
    // EXPORT HELPERS
    // ========================================================

    ExportFormat getSelectedExportFormat() const;

    juce::String getExportExtension() const;

    juce::String getExportFilter() const;

    bool writeAudioFile(
        const juce::File& sourceFile,
        const juce::File& destination,
        double startTime,
        double endTime,
        bool normalize
    );


    // ========================================================
    // KEYBOARD
    // ========================================================

    void handleKeyboardShortcut(
        const juce::KeyPress& key
    );


    // ========================================================
    // NON-COPYABLE
    // ========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MainComponent
    );
};

