cmake_minimum_required(VERSION 3.22)

project(PaperDrums VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)

if (DEFINED JUCE_SOURCE_DIR)
    # Use -DJUCE_SOURCE_DIR=C:/path/to/JUCE for an offline build.
    add_subdirectory(${JUCE_SOURCE_DIR} JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.8
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_binary_data(PaperDrumsAssets
    NAMESPACE PaperDrumsAssets
    SOURCES
        assets/paper_handling.wav
        assets/pencil_on_paper.wav
        assets/paper_tear.wav
        assets/paper_crumple.wav
)

juce_add_plugin(PaperDrums
    COMPANY_NAME "Paper Noise"
    COMPANY_WEBSITE "https://example.com/paperdrums"
    BUNDLE_ID "com.papernoise.paperdrums"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE PapN
    PLUGIN_CODE PDrm
    FORMATS VST3 Standalone
    PRODUCT_NAME "Paper Drums"
)

target_sources(PaperDrums PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(PaperDrums PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PaperDrums PRIVATE
    PaperDrumsAssets
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    juce::juce_recommended_lto_flags
)

if (MSVC)
    target_compile_options(PaperDrums PRIVATE /W4)
else()
    target_compile_options(PaperDrums PRIVATE -Wall -Wextra -Wpedantic)
endif()
