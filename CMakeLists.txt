cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

project(sfmlgamejam05)
set(EXECUTABLE_NAME Copcop)

# includes
set(SFML_ROOT "C:/Program Files (x86)/SFML")
#find_package(SFML COMPONENTS audio graphics window system)
find_package(SFML 2.3.2 REQUIRED COMPONENTS graphics window audio network system)


set(CMAKE_CXX_FLAGS "-std=c++14 -Wno-deprecated ${CMAKE_CXX_FLAGS}")
# -pedantic -Wshadow -Wall -Wextra -Wconversion


#file(GLOB_RECURSE SRC "src/*.cpp")
#file(GLOB_RECURSE SRC "src/*.h")

set(SRC
"src/load_json.cpp"
"src/save_text.cpp"
"src/main.cpp"

"src/Group.hpp"
"src/sfml_vector_math.hpp"

"src/json.hpp"
"src/common.hpp"
"src/asset.hpp"
"src/persist.hpp"
"src/speech.hpp"

"src/dialogue.hpp"
"src/tilemap.hpp"
"src/pathfind.hpp"
"src/megasprite.hpp"
"src/scene.hpp"

"src/resource_path.hpp"
)

if(APPLE)
    set(SRC ${SRC} "osx/resource_path.mm")
else()
    set(SRC ${SRC} "other/resource_path.cpp")
endif()

include_directories(${SFML_INCLUDE_DIR} "src/")
if(APPLE)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SRC})
else()
    add_executable(${EXECUTABLE_NAME} ${SRC})
endif()
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

# Create a stand-alone bundle on OS X
if(APPLE)
    set(MAKE_INSTALL_NAME_DIR @rpath)

    # Add Apple libraries
    FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${EXECUTABLE_NAME} ${FOUNDATION_LIBRARY})

    # Add Info.plist to bundle
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/osx/Info.plist.in)

    # Copy assets into the bundle
    set(BUNDLE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}.app)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION ${BUNDLE_PATH}/Contents/Resources)
    install(FILES ${PROJECT_SOURCE_DIR}/osx/Icon.icns DESTINATION ${BUNDLE_PATH}/Contents/Resources)

    # Make the bundle drag-n-drop
    get_filename_component(SFML_LIBRARY_DIR ${SFML_SYSTEM_LIBRARY} DIRECTORY)
    # Copy freetype manually, since it otherwise just causes problems (I think because it uses @executable_path instead of @rpath)
    install(DIRECTORY ${SFML_LIBRARY_DIR}/freetype.framework DESTINATION ${BUNDLE_PATH}/Contents/Frameworks)
    # Let BundleUtilities do everything else automaitcally
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${BUNDLE_PATH}\" \"\" \"${LIBRARIES_DIR}\")
        " COMPONENT Runtime)
endif()
