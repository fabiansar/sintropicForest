cmake_minimum_required(VERSION 3.10)
project(3DGraphicsEngine)
set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(3DEngine 
    src/main_new.cpp
    src/GraphicsEngine.cpp
)

# Link libraries
target_link_libraries(3DEngine PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    glad::glad
    imgui::imgui
)