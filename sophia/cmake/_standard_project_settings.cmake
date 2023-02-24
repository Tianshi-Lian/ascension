include_guard()

# Set common project settings for this CMake project
function(set_standard_project_settings)
    # Set a default built type if none specified
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(
            STATUS "Setting build type to 'Debug' as none were specified."
        )
        set(CMAKE_BUILD_TYPE
            Debug
            CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    endif()

    # Finds and activates ccache for builds
    find_program(CCACHE ccache)

    if(CCACHE)
        message(STATUS "Using ccache.")
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_CACHEFILE_DIR ${CMAKE_BINARY_DIR})
    else()
        message(STATUS "Could not find ccache.")
    endif()

    # Generates compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

    # Disable compiler extensions to ensure compliance with ISO C++ standards
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Enable link time optimisation by default for release
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        include(CheckIPOSupported)
        check_ipo_supported(RESULT result)

        if(result)
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        endif()
    endif()
endfunction()