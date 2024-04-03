cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(ConstMicroDistanceSystem LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -Wno-comment -Wno-sign-compare -Wno-write-strings -g2 -ggdb")
set(CMAKE_PREFIX_PATH "d:/Qt/6.6.2/mingw_64") # Qt Kit Dir
set(OpenCV_DIR "D:/OpenCV-4.9.0/build/x64/mingw/install") # OpenCV Dir
set(QDEPLOY_EXE ${CMAKE_PREFIX_PATH}/bin/windeployqt6.exe) # windeployqt6 executable

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build/install/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/deploy)

add_definitions(-D_STDCALL_SUPPORTED)

find_package(Qt6 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS
find_package(OpenCV REQUIRED) # OpenCV

aux_source_directory(./src srcs)
aux_source_directory(./include includes)
aux_source_directory(./module modules)
aux_source_directory(./tasks tasks)
aux_source_directory(./utils utils)
aux_source_directory(./tests tests)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(include ${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_DIR} ./lib)

add_executable(${PROJECT_NAME}
    # WIN32 # If you need a terminal for debug, please comment this statement 
    ${modules} ${tasks} ${utils} ${srcs} 
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets # Qt6 Shared Library
                                              ${OpenCV_LIBS}
                                              MvCameraControl.lib
                                              ADVMOT.lib) 

file(GLOB test_src_list tests/*.cpp)
foreach(src ${test_src_list})
    string(REGEX MATCH "tests/.*" relative_path ${src})
    string(REGEX REPLACE "tests/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})
    
    add_executable(${target_name} ${src} ${modules})
    target_link_libraries(${target_name} PRIVATE Qt6::Widgets # Qt6 Shared Library
                                              ${OpenCV_LIBS}
                                              MvCameraControl.lib
                                              ADVMOT.lib) 
endforeach()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${QDEPLOY_EXE} ARGS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe
                   COMMENT "Qt deploying...")
