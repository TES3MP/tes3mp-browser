IF(NOT WIN32 AND NOT APPLE)
    add_definitions(-DGLOBAL_DATA_PATH="${GLOBAL_DATA_PATH}")
    add_definitions(-DGLOBAL_CONFIG_PATH="${GLOBAL_CONFIG_PATH}")
ENDIF()

set(COMPONENT_FILES
        process/processinvoker.cpp
        settings/settings.cpp

        files/collections.cpp
        files/configurationmanager.cpp
        files/escape.cpp
        files/linuxpath.cpp
        files/multidircollection.cpp
        files/windowspath.cpp
        )

set(COMPONENT_MOC_FILES
        process/processinvoker.hpp
        )

QT5_WRAP_CPP(MOC_SRCS ${COMPONENT_MOC_FILES})

add_library(components STATIC ${COMPONENT_FILES} ${MOC_SRCS})

target_link_libraries(components Qt5::Widgets Qt5::Core)

target_link_libraries(components
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        )

# Fix for not visible pthreads functions for linker with glibc 2.15
if (UNIX AND NOT APPLE)
    target_link_libraries(components ${CMAKE_THREAD_LIBS_INIT})
endif()
