#include "UpdateChecker.h"

// ============================================================
// VERSION COMPARISON
// ============================================================

juce::String UpdateChecker::compareVersions(
    const juce::String& currentVersion,
    const juce::String& latestVersion)
{
    auto current = currentVersion.trim();
    auto latest  = latestVersion.trim();

    // --------------------------------------------------------
    // Remove optional "v" prefix
    // --------------------------------------------------------

    if (current.startsWithIgnoreCase("v"))
        current = current.substring(1);

    if (latest.startsWithIgnoreCase("v"))
        latest = latest.substring(1);

    // --------------------------------------------------------
    // Split versions
    // --------------------------------------------------------

    auto currentParts =
        juce::StringArray::fromTokens(
            current,
            ".",
            ""
        );

    auto latestParts =
        juce::StringArray::fromTokens(
            latest,
            ".",
            ""
        );

    // --------------------------------------------------------
    // Ensure major.minor.patch
    // --------------------------------------------------------

    while (currentParts.size() < 3)
        currentParts.add("0");

    while (latestParts.size() < 3)
        latestParts.add("0");

    // --------------------------------------------------------
    // Compare major / minor / patch
    // --------------------------------------------------------

    for (int i = 0; i < 3; ++i)
    {
        const int currentNumber =
            currentParts[i].getIntValue();

        const int latestNumber =
            latestParts[i].getIntValue();

        if (latestNumber > currentNumber)
            return "newer";

        if (latestNumber < currentNumber)
            return "older";
    }

    return "same";
}


// ============================================================
// CHECK FOR UPDATE
// ============================================================

void UpdateChecker::checkForUpdate(
    const juce::String& currentVersion,
    Callback callback)
{
    // ========================================================
    // YOUR UPDATE MANIFEST URL
    // ========================================================

    const juce::URL updateUrl(
        "https://chezchris.onrender.com/api/v1/stem-splitter/offor-stem-splitter/update"
    );

    // ========================================================
    // INVALID URL
    // ========================================================

    if (!updateUrl.isWellFormed())
    {
        UpdateInfo info;

        info.currentVersion =
            currentVersion;

        if (callback)
        {
            juce::MessageManager::callAsync(
                [callback, info]()
                {
                    callback(info);
                }
            );
        }

        return;
    }

    // ========================================================
    // BACKGROUND THREAD
    // ========================================================

    juce::Thread::launch(
        [this,
         updateUrl,
         currentVersion,
         callback]()
        {
            UpdateInfo info;

            info.currentVersion =
                currentVersion;

            // =================================================
            // CONNECT
            // =================================================

            std::unique_ptr<juce::InputStream> stream =
                updateUrl.createInputStream(
                    juce::URL::InputStreamOptions(
                        juce::URL::ParameterHandling::inAddress
                    )
                    .withConnectionTimeoutMs(10000)
                    .withNumRedirectsToFollow(5)
                );

            // =================================================
            // CONNECTION FAILED
            // =================================================

            if (stream == nullptr)
            {
                juce::MessageManager::callAsync(
                    [callback, info]()
                    {
                        if (callback)
                            callback(info);
                    }
                );

                return;
            }

            // =================================================
            // READ RESPONSE
            // =================================================

            const juce::String jsonText =
                stream->readEntireStreamAsString();

            if (jsonText.isEmpty())
            {
                juce::MessageManager::callAsync(
                    [callback, info]()
                    {
                        if (callback)
                            callback(info);
                    }
                );

                return;
            }

            // =================================================
            // PARSE JSON
            // =================================================

            const auto json =
                juce::JSON::parse(jsonText);

            if (!json.isObject())
            {
                juce::MessageManager::callAsync(
                    [callback, info]()
                    {
                        if (callback)
                            callback(info);
                    }
                );

                return;
            }

            auto* object =
                json.getDynamicObject();

            if (object == nullptr)
            {
                juce::MessageManager::callAsync(
                    [callback, info]()
                    {
                        if (callback)
                            callback(info);
                    }
                );

                return;
            }

            // =================================================
            // UPDATE INFORMATION
            // =================================================

            info.latestVersion =
                object->getProperty(
                    "latestVersion"
                ).toString();

            info.minimumVersion =
                object->getProperty(
                    "minimumVersion"
                ).toString();

            info.downloadUrl =
                object->getProperty(
                    "downloadUrl"
                ).toString();

            // =================================================
            // RELEASE NOTES
            // =================================================

            const auto releaseNotesValue =
                object->getProperty(
                    "releaseNotes"
                );

            if (auto* notesArray =
                    releaseNotesValue.getArray())
            {
                for (const auto& note : *notesArray)
                {
                    info.releaseNotes.add(
                        note.toString()
                    );
                }
            }

            // =================================================
            // COMPARE VERSIONS
            // =================================================

            if (info.latestVersion.isNotEmpty())
            {
                const auto comparison =
                    compareVersions(
                        currentVersion,
                        info.latestVersion
                    );

                info.updateAvailable =
                    comparison == "newer";
            }

            // =================================================
            // RETURN TO MESSAGE THREAD
            // =================================================

            juce::MessageManager::callAsync(
                [callback, info]()
                {
                    if (callback)
                        callback(info);
                }
            );
        }
    );
}