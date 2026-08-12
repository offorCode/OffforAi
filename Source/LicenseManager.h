#pragma once

#include <JuceHeader.h>

class LicenseManager
{
public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    LicenseManager();


    // ========================================================
    // LOCAL LICENSE STATE
    // ========================================================

    bool isActivated() const;

    bool activate(const juce::String& licenseKey);

    juce::String getStoredLicense() const;


    // ========================================================
    // USAGE
    // ========================================================

    int getUsageCount() const;

    void incrementUsage();

    bool hasFreeUsesRemaining() const;


    // ========================================================
    // INSTALLATION ID
    // ========================================================

    juce::String getInstallationId() const;


    // ========================================================
    // SERVER
    // ========================================================

    bool registerInstallation();

    bool checkUsage();


private:

    // ========================================================
    // SETTINGS
    // ========================================================

    static constexpr int FREE_USES = 60;

    static constexpr const char* PRODUCT_NAME =
        "OfforStemSplitter";

    static constexpr const char* PRODUCT_VERSION =
        "1.1.0";

    static constexpr const char* LICENSE_SERVER =
        "https://chezchris.onrender.com";


    // ========================================================
    // JUCE PROPERTIES
    // ========================================================

    juce::ApplicationProperties properties;


    juce::PropertiesFile* getProperties();


    // ========================================================
    // INSTALLATION ID
    // ========================================================

    juce::String createInstallationId();


    // ========================================================
    // SERVER RESPONSE
    // ========================================================

    bool lastServerAllowed = false;

    int lastServerFreeUses = 0;

    int lastServerFreeUsesLimit = FREE_USES;


    // ========================================================
    // NON-COPYABLE
    // ========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        LicenseManager
    );
};