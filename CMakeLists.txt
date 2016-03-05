cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

project(sfmlgamejam05)

# includes
set(SFML_ROOT "C:/Program Files (x86)/SFML")
#find_package(SFML COMPONENTS audio graphics window system)
find_package(SFML 2.3.2 REQUIRED COMPONENTS graphics window audio network system)


set(CMAKE_CXX_FLAGS "-std=c++14 -pedantic -Wall -Wextra -Wshadow -Wconversion ${CMAKE_CXX_FLAGS}")


#file(GLOB_RECURSE SRC "src/*.cpp")
#file(GLOB_RECURSE SRC "src/*.h")
set(SRC
"src/main.cpp"
"src/asset.hpp"
"src/persist.hpp"
"src/dialogue.hpp"
"src/scene.hpp"
"src/json.hpp"
"src/load_json.hpp"
)

include_directories(${SFML_INCLUDE_DIR} "src/")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES}) #${SFML_DEPENDENCIES}
