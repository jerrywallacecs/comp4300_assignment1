cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(Imgui)
FetchContent_GetProperties(Imgui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(Imgui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(Imgui-SFML)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics ImGui-SFML::ImGui-SFML)

# handling font
set(FONT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/fonts/airstrike.ttf")
set(FONT_DEST "$<TARGET_FILE_DIR:main>/fonts/airstrike.ttf")
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:main>/fonts"
)

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/fonts/airstrike.ttf"
        "$<TARGET_FILE_DIR:main>/fonts/airstrike.ttf"
    COMMENT "Copying airstrike.ttf to bin/fonts/"
)

# handling config file
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:main>/config"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/config.txt"
            "$<TARGET_FILE_DIR:main>/config/config.txt"
    COMMENT "Copying config.txt -> $<TARGET_FILE_DIR:main>/config/"
)