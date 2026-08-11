#pragma once

#include <JuceHeader.h>

class LicenseManager
{
public:

    LicenseManager();

    bool isActivated() const;

    bool activate(const juce::String& licenseKey);

    int getUsageCount() const;

    void incrementUsage();

    bool hasFreeUsesRemaining() const;

    juce::String getStoredLicense() const;

private:

    static constexpr int FREE_USES = 60;

    juce::ApplicationProperties properties;

    juce::PropertiesFile* getProperties();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseManager)
};