cmake_minimum_required(VERSION 3.10)
project(CourseworkBase)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)

message(STATUS "Found SDL3: ${SDL3_VERSION} (${SDL3_DIR})")
message(STATUS "Found SDL3_ttf: ${SDL3_ttf_VERSION} (${SDL3_ttf_DIR})")
message(STATUS "Found SDL3_image: ${SDL3_image_VERSION} (${SDL3_image_DIR})")

include_directories(
    ${PROJECT_SOURCE_DIR}/CourseworkBase
    ${PROJECT_SOURCE_DIR}/CourseworkBase/include
    ${PROJECT_SOURCE_DIR}/CourseworkBase/include/framework
    ${PROJECT_SOURCE_DIR}/CourseworkBase/include/demos
    ${PROJECT_SOURCE_DIR}/CourseworkBase/include/demos/advanced
)

set(SOURCE_FILES
    # Main entry point
    CourseworkBase/mainfunction.cpp

    # Framework source files
    CourseworkBase/src/framework/BaseEngine.cpp
    CourseworkBase/src/framework/DisplayableObject.cpp
    CourseworkBase/src/framework/DrawingSurface.cpp
    CourseworkBase/src/framework/FontManager.cpp
    CourseworkBase/src/framework/SimpleImage.cpp
    CourseworkBase/src/framework/TileManager.cpp

    # Demo source files
    CourseworkBase/src/demos/BouncingBall.cpp
    CourseworkBase/src/demos/BouncingBallMain.cpp
    CourseworkBase/src/demos/SimpleDemo.cpp
    CourseworkBase/src/demos/MazeDemoMain.cpp
    CourseworkBase/src/demos/MazeDemoObject.cpp
    CourseworkBase/src/demos/MazeDemoTileManager.cpp

    # Psyhm9 coursework source files
    CourseworkBase/src/Psyhm9Enemy.cpp
    CourseworkBase/src/Psyhm9Game.cpp
    CourseworkBase/src/Psyhm9Level.cpp
    CourseworkBase/src/Psyhm9Player.cpp
    CourseworkBase/src/Psyhm9TileManager.cpp

    # Advanced demo source files
    CourseworkBase/src/demos/advanced/DraggingDemo.cpp
    CourseworkBase/src/demos/advanced/FlashingDemo.cpp
    CourseworkBase/src/demos/advanced/ImageMappingDemo.cpp
    CourseworkBase/src/demos/advanced/StarfieldDemo.cpp
    CourseworkBase/src/demos/advanced/ZoomingDemo.cpp
)

# Optional: Enable address sanitizer for debugging (comment out if not needed)
# Uncomment the following lines to enable memory error detection
# add_compile_options(-g -fsanitize=address)
# add_link_options(-fsanitize=address)

add_executable(SDL3_App ${SOURCE_FILES})
target_link_libraries(SDL3_App SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image)

add_custom_command(TARGET SDL3_App PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/CourseworkBase/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
    COMMENT "Copying resources to build directory..."
)
