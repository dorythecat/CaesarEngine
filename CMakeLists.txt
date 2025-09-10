cmake_minimum_required(VERSION 3.5...4.0)
project(CaesarEngine VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)

message(STATUS "using ${CMAKE_GENERATOR}")

# For knowing what these do, and where I have gotten this information, check the awesome article at https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags (I know it's for embedded systems, but it's still a  pretty good article)
# For more info on all of these warning flags, check out https://gcc.gnu.org/onlinedocs/gcc-3.4.0/gcc/Warning-Options.html
# For more information of all of these optimization flags, check out https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Optimize-Options.html

# Set warning flags (Only for GCC and Clang, MSVC has its own set of flags)
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    set(WARN_FLAGS -Wall -Wextra -Wundef -Wdouble-promotion -Wformat=2 -fno-common -Wconversion -pedantic -Wno-unused-parameter)
endif(CMAKE_COMPILER_IS_GNUCXX)

# Set debug and release flags
set(DEBUG_FLAGS -g3 -Og -DDEBUG)
set(RELEASE_FLAGS -O3)

# GLFW settings
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

# Add subdirectories
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/glfw")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/libs/glfw/include ${PROJECT_SOURCE_DIR}/libs/glfw/deps ${PROJECT_SOURCE_DIR}/libs/glad/include ${PROJECT_SOURCE_DIR}/libs/stb)

if(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp) # Get all the source files
add_executable(${PROJECT_NAME} ${SOURCES} ${PROJECT_SOURCE_DIR}/libs/glad/src/glad.c
        src/line/line.cpp
        src/line/line.h) # Create the executable
target_link_libraries(${PROJECT_NAME} glfw) # Link the executable with the libraries

# Copy all resource files to the build directory
file(GLOB_RECURSE RESOURCE_FILES ${PROJECT_SOURCE_DIR}/res/*)
file(GLOB_RECURSE SHADER_FILES ${PROJECT_SOURCE_DIR}/res/shaders/*)
if(NOT EXISTS ${PROJECT_BINARY_DIR}/res/) # Create resource directory if it doesn't exist already
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/res/)
endif()
if(NOT EXISTS ${PROJECT_BINARY_DIR}/res/shaders)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/res/shaders)
endif()
foreach(RESOURCE_FILE ${RESOURCE_FILES})
    configure_file(${RESOURCE_FILE} ${PROJECT_BINARY_DIR}/res/ COPYONLY)
endforeach()
foreach(SHADER_FILE ${SHADER_FILES})
    configure_file(${SHADER_FILE} ${PROJECT_BINARY_DIR}/res/shaders COPYONLY)
endforeach()

# Apply warning flags to all configurations
target_compile_options(${PROJECT_NAME} PRIVATE ${WARN_FLAGS})

# Apply configuration-specific flags
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${RELEASE_FLAGS}>
)

# Link libraries as before
target_link_libraries(${PROJECT_NAME} glfw)

