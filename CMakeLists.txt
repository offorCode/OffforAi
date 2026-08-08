cmake_minimum_required(VERSION 3.22)

project(OfforStemSplitter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../../JUCE JUCE)

juce_add_gui_app(OfforStemSplitter
    PRODUCT_NAME "Offor Stem Splitter"
)

juce_generate_juce_header(OfforStemSplitter)

target_sources(OfforStemSplitter
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp

        Source/StemTrackComponent.cpp
        Source/StemTrackComponent.h
)

target_compile_definitions(OfforStemSplitter
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(OfforStemSplitter
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
)