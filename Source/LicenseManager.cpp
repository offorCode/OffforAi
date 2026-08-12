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

    // --------------------------------------------------------
    // Make sure this installation has a permanent ID.
    // --------------------------------------------------------

    auto* settings = getProperties();

    if (settings != nullptr)
    {
        auto installationId =
            settings->getValue("installationId", {});

        if (installationId.isEmpty())
        {
            installationId = createInstallationId();

            settings->setValue(
                "installationId",
                installationId
            );

            settings->saveIfNeeded();
        }

        std::cout << "OFFOR Installation ID: "
          << installationId.toStdString()
          << std::endl;
    }
}


// ============================================================
// GET PROPERTIES
// ============================================================

juce::PropertiesFile* LicenseManager::getProperties()
{
    return properties.getUserSettings();
}


// ============================================================
// CREATE INSTALLATION ID
// ============================================================

juce::String LicenseManager::createInstallationId()
{
    // JUCE Uuid does not need fromString().
    // Generate a new UUID and store it as a string.

    return juce::Uuid().toString()
        .removeCharacters("-");
}


// ============================================================
// INSTALLATION ID
// ============================================================

juce::String LicenseManager::getInstallationId() const
{
    auto* settings =
        const_cast<juce::ApplicationProperties&>(
            properties
        ).getUserSettings();

    if (settings == nullptr)
        return {};

    return settings->getValue(
        "installationId",
        {}
    );
}


// ============================================================
// ACTIVATED
// ============================================================

bool LicenseManager::isActivated() const
{
    auto* settings =
        const_cast<juce::ApplicationProperties&>(
            properties
        ).getUserSettings();

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
    auto* settings =
        const_cast<juce::ApplicationProperties&>(
            properties
        ).getUserSettings();

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
    auto* settings =
        const_cast<juce::ApplicationProperties&>(
            properties
        ).getUserSettings();

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
// INCREMENT LOCAL USAGE
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
// REGISTER INSTALLATION
// ============================================================

bool LicenseManager::registerInstallation()
{
    std::cout << "=== REGISTER INSTALLATION CALLED ==="
          << std::endl;
    const auto installationId = getInstallationId();

    std::cout << "Registering installation ID: "
          << installationId.toStdString()
          << std::endl;

    if (installationId.isEmpty())
        return false;

    // --------------------------------------------------------
    // Build JSON request
    // --------------------------------------------------------

    auto requestObject = std::make_unique<juce::DynamicObject>();

    requestObject->setProperty(
        "installationId",
        installationId
    );

    requestObject->setProperty(
        "product",
        PRODUCT_NAME
    );

    requestObject->setProperty(
        "version",
        PRODUCT_VERSION
    );

    juce::var requestVar(requestObject.release());

    const auto requestJson =
        juce::JSON::toString(requestVar);

    // --------------------------------------------------------
    // Create POST URL
    // --------------------------------------------------------

    juce::URL url(
        juce::String(LICENSE_SERVER)
        + "/api/v1/license/register"
    );

    url = url.withPOSTData(requestJson);

    int statusCode = 0;

    auto options =
        juce::URL::InputStreamOptions(
            juce::URL::ParameterHandling::inPostData
        )
        .withHttpRequestCmd("POST")
        .withExtraHeaders(
            "Content-Type: application/json\r\n"
            "Accept: application/json"
        )
        .withConnectionTimeoutMs(10000)
        .withNumRedirectsToFollow(5)
        .withStatusCode(&statusCode);

    auto stream = url.createInputStream(options);

    if (stream == nullptr)
    {
        DBG("License registration failed: no connection.");
        return false;
    }

    const auto response =
        stream->readEntireStreamAsString();

    DBG(
        "License register response: "
        + response
    );

    if (statusCode < 200 || statusCode >= 300)
    {
        DBG(
            "License registration HTTP status: "
            + juce::String(statusCode)
        );

        return false;
    }

    // --------------------------------------------------------
    // Parse response
    // --------------------------------------------------------

    const auto json =
        juce::JSON::parse(response);

    if (!json.isObject())
        return false;

    const auto* object =
        json.getDynamicObject();

    if (object == nullptr)
        return false;

    const bool success =
        static_cast<bool>(
            object->getProperty("success")
        );

    return success;
}


// ============================================================
// CHECK USAGE WITH SERVER
// ============================================================

bool LicenseManager::checkUsage()
{
    // --------------------------------------------------------
    // Activated licenses do not need the free-use check.
    // --------------------------------------------------------

    if (isActivated())
    {
        lastServerAllowed = true;
        return true;
    }

    const auto installationId =
        getInstallationId();

    if (installationId.isEmpty())
    {
        lastServerAllowed = false;
        return false;
    }

    // --------------------------------------------------------
    // Build JSON
    // --------------------------------------------------------

    auto requestObject = std::make_unique<juce::DynamicObject>();

    requestObject->setProperty(
        "installationId",
        installationId
    );

    requestObject->setProperty(
        "product",
        PRODUCT_NAME
    );

    requestObject->setProperty(
        "version",
        PRODUCT_VERSION
    );

    juce::var requestVar(requestObject.release());

    const auto requestJson =
        juce::JSON::toString(requestVar);

    // --------------------------------------------------------
    // POST
    // --------------------------------------------------------

    juce::URL url(
        juce::String(LICENSE_SERVER)
        + "/api/v1/license/use"
    );

    url = url.withPOSTData(requestJson);

    int statusCode = 0;

    auto options =
        juce::URL::InputStreamOptions(
            juce::URL::ParameterHandling::inPostData
        )
        .withHttpRequestCmd("POST")
        .withExtraHeaders(
            "Content-Type: application/json\r\n"
            "Accept: application/json"
        )
        .withConnectionTimeoutMs(10000)
        .withNumRedirectsToFollow(5)
        .withStatusCode(&statusCode);

    auto stream = url.createInputStream(options);

    if (stream == nullptr)
    {
        DBG("License usage check failed: no connection.");

        lastServerAllowed = false;

        return false;
    }

    const auto response =
        stream->readEntireStreamAsString();

    DBG(
        "License usage response: "
        + response
    );

    if (statusCode < 200 || statusCode >= 300)
    {
        DBG(
            "License usage HTTP status: "
            + juce::String(statusCode)
        );

        lastServerAllowed = false;

        return false;
    }

    // --------------------------------------------------------
    // Parse response
    // --------------------------------------------------------

    const auto json =
        juce::JSON::parse(response);

    if (!json.isObject())
    {
        lastServerAllowed = false;
        return false;
    }

    const auto* object =
        json.getDynamicObject();

    if (object == nullptr)
    {
        lastServerAllowed = false;
        return false;
    }

    // --------------------------------------------------------
    // Read server result
    // --------------------------------------------------------

    lastServerAllowed =
        static_cast<bool>(
            object->getProperty("allowed")
        );

    lastServerFreeUses =
        static_cast<int>(
            object->getProperty("freeUses")
        );

    lastServerFreeUsesLimit =
        static_cast<int>(
            object->getProperty("freeUsesLimit")
        );

    // --------------------------------------------------------
    // Keep local usage synchronized with server.
    // --------------------------------------------------------

    auto* settings = getProperties();

    if (settings != nullptr)
    {
        settings->setValue(
            "usageCount",
            lastServerFreeUses
        );

        settings->saveIfNeeded();
    }

    // --------------------------------------------------------
    // If server says activated, remember it locally.
    // --------------------------------------------------------

    const bool serverActivated =
        static_cast<bool>(
            object->getProperty("activated")
        );

    if (serverActivated && settings != nullptr)
    {
        settings->setValue(
            "activated",
            true
        );

        settings->saveIfNeeded();
    }

    return lastServerAllowed;
}


// ============================================================
// ACTIVATE LICENSE
// ============================================================

bool LicenseManager::activate(
    const juce::String& licenseKey
)
{
    const auto cleanedKey =
        licenseKey.trim();

    if (cleanedKey.isEmpty())
        return false;

    const auto installationId =
        getInstallationId();

    if (installationId.isEmpty())
        return false;

    // --------------------------------------------------------
    // BUILD JSON
    // --------------------------------------------------------

    auto requestObject =
        std::make_unique<juce::DynamicObject>();

    requestObject->setProperty(
        "licenseKey",
        cleanedKey
    );

    requestObject->setProperty(
        "installationId",
        installationId
    );

    requestObject->setProperty(
        "product",
        PRODUCT_NAME
    );

    requestObject->setProperty(
        "version",
        PRODUCT_VERSION
    );

    juce::var requestVar(
        requestObject.release()
    );

    const auto requestJson =
        juce::JSON::toString(
            requestVar
        );

    // --------------------------------------------------------
    // CREATE URL
    // --------------------------------------------------------

    juce::URL url(
        juce::String(LICENSE_SERVER)
        + "/api/v1/license/activate"
    );

    url = url.withPOSTData(
        requestJson
    );

    int statusCode = 0;

    auto options =
        juce::URL::InputStreamOptions(
            juce::URL::ParameterHandling::inPostData
        )
        .withHttpRequestCmd("POST")
        .withExtraHeaders(
            "Content-Type: application/json\r\n"
            "Accept: application/json"
        )
        .withConnectionTimeoutMs(10000)
        .withNumRedirectsToFollow(5)
        .withStatusCode(&statusCode);

    auto stream =
        url.createInputStream(
            options
        );

    if (stream == nullptr)
    {
        std::cout
            << "LICENSE: ACTIVATION CONNECTION FAILED"
            << std::endl;

        return false;
    }

    const auto response =
        stream->readEntireStreamAsString();

    std::cout
        << "LICENSE: ACTIVATION RESPONSE: "
        << response.toStdString()
        << std::endl;

    if (statusCode < 200 || statusCode >= 300)
    {
        std::cout
            << "LICENSE: ACTIVATION HTTP ERROR: "
            << statusCode
            << std::endl;

        return false;
    }

    // --------------------------------------------------------
    // PARSE RESPONSE
    // --------------------------------------------------------

    const auto json =
        juce::JSON::parse(
            response
        );

    if (!json.isObject())
        return false;

    const auto* object =
        json.getDynamicObject();

    if (object == nullptr)
        return false;

    const bool success =
        static_cast<bool>(
            object->getProperty(
                "success"
            )
        );

    if (!success)
        return false;

    // --------------------------------------------------------
    // SAVE ACTIVATION LOCALLY
    // --------------------------------------------------------

    auto* settings =
        getProperties();

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

    std::cout
        << "LICENSE: ACTIVATED SUCCESSFULLY"
        << std::endl;

    return true;
}