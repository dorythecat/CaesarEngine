cmake_minimum_required(VERSION 3.15...3.31)
project(CaesarEngine VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)

message(STATUS "using ${CMAKE_GENERATOR}")

# For knowing what these do, and where I have gotten this information, check the awesome article at https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags (I know it's for embedded systems, but it's still a  pretty good article)
# For more info on all of these warning flags, check out https://gcc.gnu.org/onlinedocs/gcc-3.4.0/gcc/Warning-Options.html
# For more information of all of these optimization flags, check out https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Optimize-Options.html
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wundef -Wdouble-promotion -Wformat=2 -fno-common -Wconversion -pedantic -Wno-unused-parameter") # Bundle the necessary static libraries, as well as enable some useful warnings, and disable some nasty ones
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Og -DDEBUG") # Enable debug symbols and optimisations for debug builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3") # Enable full optimisations for release builds

# GLFW settings
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)


# Add subdirectories
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/glfw")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/libs/glfw/include ${PROJECT_SOURCE_DIR}/libs/glfw/deps ${PROJECT_SOURCE_DIR}/libs/glad/include)

if(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp) # Get all the source files
add_executable(${PROJECT_NAME} ${SOURCES} ${PROJECT_SOURCE_DIR}/libs/glad/src/glad.c) # Create the executable
target_link_libraries(${PROJECT_NAME} glfw) # Link the executable with the libraries
