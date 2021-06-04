cmake_minimum_required(VERSION 3.17)
project(OTO_Gravity)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-v)

set(DOWNLOADS "/Users/maxroschupkin/Downloads")

set(SFML_DIR "${DOWNLOADS}/SFML-2.5.1")
set(SFML_INCLUDE_DIR "${SFML_DIR}/include")
set(SFML_LIBRARY_DIR "${SFML_DIR}/lib")

link_directories(${SFML_LIBRARY_DIR})
include_directories(${SFML_INCLUDE_DIR})


set(YAML_DIR "${DOWNLOADS}/yaml-cpp-master")

link_directories(${YAML_DIR}/build)
include_directories(${YAML_DIR}/include)

set(INC_DIRS AppSystem)
include_directories(${INC_DIRS})


set(src AppSystem/EventHandler.cpp)

find_package(SFML 2.5.1 COMPONENTS system audio window graphics network REQUIRED)

link_libraries(yaml-cpp)
link_libraries (sfml-graphics sfml-window sfml-system sfml-audio)

file(COPY config.yaml font.otf DESTINATION ./)

add_executable(OTO_Gravity main.cpp ${src})