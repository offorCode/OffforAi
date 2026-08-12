#pragma once

#include <JuceHeader.h>

class UpdateChecker
{
public:

    struct UpdateInfo
    {
        bool updateAvailable = false;

        juce::String currentVersion;
        juce::String latestVersion;
        juce::String minimumVersion;
        juce::String downloadUrl;

        juce::StringArray releaseNotes;
    };

    using Callback = std::function<void(const UpdateInfo&)>;

    UpdateChecker() = default;

    void checkForUpdate(
        const juce::String& currentVersion,
        Callback callback);

private:

    juce::String compareVersions(
        const juce::String& currentVersion,
        const juce::String& latestVersion);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateChecker)
};