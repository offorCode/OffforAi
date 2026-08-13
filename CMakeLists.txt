cmake_minimum_required(VERSION 3.22)

project(OfforStemSplitter VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../../JUCE JUCE)

# ==========================================================
# Offor Stem Splitter Resources
# ==========================================================

juce_add_binary_data(OfforStemSplitterResources
    SOURCES
        Resources/OfforStemSplitter.png
)

# ==========================================================
# Application
# ==========================================================

juce_add_gui_app(OfforStemSplitter
    COMPANY_NAME "ONNTECH"
    PRODUCT_NAME "Offor Stem Splitter"

    FORMATS
        VST3
        Standalone


)

juce_generate_juce_header(OfforStemSplitter)

# ==========================================================
# Source Files
# ==========================================================

target_sources(OfforStemSplitter
    PRIVATE

        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h

        Source/StemTrackComponent.cpp
        Source/StemTrackComponent.h

        Source/SettingsComponent.cpp
        Source/LicenseManager.cpp

        Source/UpdateChecker.cpp

        Source/OfforStemSplitter.rc
)

# ==========================================================
# Compile Definitions
# ==========================================================

target_compile_definitions(OfforStemSplitter
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=1
)

# ==========================================================
# Libraries
# ==========================================================

target_link_libraries(OfforStemSplitter
    PRIVATE

        juce::juce_gui_extra
        juce::juce_audio_utils

        OfforStemSplitterResources
)
