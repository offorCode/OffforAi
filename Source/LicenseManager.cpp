#include "LicenseManager.h"

// ============================================================
// CONSTRUCTOR
// ============================================================

LicenseManager::LicenseManager()
{
    juce::PropertiesFile::Options options;

    options.applicationName = "OfforStemSplitter";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "OFFOR";
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    properties.setStorageParameters(options);
}


// ============================================================
// GET PROPERTIES
// ============================================================

juce::PropertiesFile* LicenseManager::getProperties()
{
    return properties.getUserSettings();
}


// ============================================================
// ACTIVATED
// ============================================================

bool LicenseManager::isActivated() const
{
    auto* settings = properties.getUserSettings();

    if (settings == nullptr)
        return false;

    return settings->getBoolValue(
        "activated",
        false
    );
}


// ============================================================
// STORED LICENSE
// ============================================================

juce::String LicenseManager::getStoredLicense() const
{
    auto* settings = properties.getUserSettings();

    if (settings == nullptr)
        return {};

    return settings->getValue(
        "licenseKey",
        {}
    );
}


// ============================================================
// USAGE COUNT
// ============================================================

int LicenseManager::getUsageCount() const
{
    auto* settings = properties.getUserSettings();

    if (settings == nullptr)
        return 0;

    return settings->getIntValue(
        "usageCount",
        0
    );
}


// ============================================================
// FREE USES REMAINING
// ============================================================

bool LicenseManager::hasFreeUsesRemaining() const
{
    if (isActivated())
        return true;

    return getUsageCount() < FREE_USES;
}


// ============================================================
// INCREMENT USAGE
// ============================================================

void LicenseManager::incrementUsage()
{
    if (isActivated())
        return;

    auto* settings = getProperties();

    if (settings == nullptr)
        return;

    const int currentCount = getUsageCount();

    settings->setValue(
        "usageCount",
        currentCount + 1
    );

    settings->saveIfNeeded();
}


// ============================================================
// ACTIVATE
// ============================================================

bool LicenseManager::activate(
    const juce::String& licenseKey
)
{
    auto cleanedKey = licenseKey.trim();

    if (cleanedKey.isEmpty())
        return false;

    // --------------------------------------------------------
    // TEMPORARY:
    // The real server verification will replace this.
    // --------------------------------------------------------

    if (cleanedKey == "OFFOR-DEMO-2026")
    {
        auto* settings = getProperties();

        if (settings == nullptr)
            return false;

        settings->setValue(
            "activated",
            true
        );

        settings->setValue(
            "licenseKey",
            cleanedKey
        );

        settings->saveIfNeeded();

        return true;
    }

    return false;
}