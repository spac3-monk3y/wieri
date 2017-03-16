cmake_minimum_required (VERSION 2.6.0)

project(wiegand_reader C)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (IS_RASP FALSE)
set (LIBS_PATH libs)

if (CMAKE_COMPILER_IS_GNUCC)
    set (CMAKE_C_FLAGS "-lwiringPi -lpthread")
endif()

message (STATUS "Making Wieri v${VERSION_MAJOR}.${VERSION_MINOR}")

set (SRC_PATH .)
add_executable (wieri wieri.c)

include_directories (${SRC_PATH})

# Inih
include_directories (${LIBS_PATH}/inih)
add_library (inih STATIC ${LIBS_PATH}/inih/ini.c)
set_target_properties (inih PROPERTIES LINKER_LANGUAGE C)

if (UNIX)
    set (ARCH_CMD cat cpuinfo)
    set (GREP_CMD grep BCM27)
    set (AWK_CMD awk "{print $3}")
    execute_process (COMMAND ${ARCH_CMD} COMMAND ${GREP_CMD} COMMAND ${AWK_CMD}
        WORKING_DIRECTORY /proc
        RESULT_VARIABLE CMD_STAT
        OUTPUT_VARIABLE ARCH_VER
    )
    if (NOT ${ARCH_VER} STREQUAL "")
        set (IS_RASP TRUE)
        string(REGEX REPLACE "\n$" "" ARCH_VER "${ARCH_VER}")
        message (STATUS "Running on Raspberry Pi with processor: ${ARCH_VER}")
    else ()
        message (FATAL_ERROR "Design for Raspberry Pi")
    endif()
else ()
    message (FATAL_ERROR "Design for Unix")
endif ()

target_link_libraries (
  wieri
  inih
)
