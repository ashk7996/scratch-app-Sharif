cmake_minimum_required(VERSION 3.22)
project(Scratch_G36)

set(CMAKE_CXX_STANDARD 14)

# Must set the path to the main.cpp, for example: scripts/main.cpp if it is inside a folder
add_executable(${PROJECT_NAME} scripts/main.cpp
        scripts/common/common.h
        scripts/common/common.cpp
        scripts/Ui/uiFuncs.h
        scripts/Ui/uiFuncs.cpp
        scripts/Menu/Menu.h
        scripts/Menu/menu.cpp
        scripts/LeftPannel/ModeSelector/ModeSelector.h
        scripts/LeftPannel/ModeSelector/ModeSelector.cpp
        scripts/SpiritFunctions/spiritFunctions.h
        scripts/SpiritFunctions/spiritFunctions.cpp
        scripts/SpiritPreview/spiritPreview.h
        scripts/SpiritPreview/spiritPreview.cpp
        scripts/CodePanel/CodePanel.h
        scripts/CodePanel/CodePanel.cpp
        scripts/CostumesPanel/CostumesPanel.cpp
        scripts/CostumesPanel/CostumesPanel.h
)

# --- SDL2 SETUP ---
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "SDL2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
