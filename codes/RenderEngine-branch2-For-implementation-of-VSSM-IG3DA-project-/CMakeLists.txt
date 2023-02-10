cmake_minimum_required(VERSION 3.9)

project(IceRender LANGUAGES CXX)

# this will check folder "External/CMakeLists.txt"
add_subdirectory(External)


# update below command when add/remove folders
file(GLOB Sources "Sources/*.h" "Sources/*.cpp" "Sources/*.hpp" 
                  "Sources/*/*.hpp" "Sources/*/*.cpp" "Sources/*/*.h"
                  
)

# if above codes are not working, put the below things into it
# "Sources/helpers/*.h" "Sources/helpers/*.cpp" "Sources/helpers/*.hpp"
# "Sources/singleton/*.h" "Sources/singleton/*.cpp" "Sources/singleton/*.hpp"
# "Sources/rasterizer/*.h" "Sources/rasterizer/*.cpp" "Sources/rasterizer/*.hpp"
# "Sources/mesh/*.h" "Sources/mesh/*.cpp" "Sources/mesh/*.hpp"
# "Sources/transform/*.h" "Sources/transform/*.cpp" "Sources/transform/*.hpp"

add_executable(IceRender ${Sources})


set_target_properties(IceRender PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(IceRender LINK_PRIVATE glfw)

target_link_libraries(IceRender LINK_PRIVATE glm)

# the below two codes are replaced by the next line code. refer:https://github.com/Dav1dde/glad/issues/186
# include_directories(${CMAKE_CURRENT_SOURCE_DIR} External/glad/include)
# "External/glad/src/glad.c" # put this into above "file()"

target_link_libraries(IceRender LINK_PRIVATE glad)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} External/stb_image/)

# include "nlohmann_json"
# (how to use it)[https://github.com/nlohmann/json#examples]
# just use it for configuring the scene (maybe delete it later)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} External/nlohmann/)