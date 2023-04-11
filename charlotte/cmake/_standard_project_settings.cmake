include_guard()

# Set common project settings for this CMake project
macro(set_standard_project_settings)
	# Setup the build configurations for single or multi config generators
	if(NOT SET_UP_CONFIGURATIONS_DONE)
		set(SET_UP_CONFIGURATIONS_DONE TRUE)

		get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

		if(isMultiConfig)
			message(STATUS "Using multi-config")
			set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
		else()
			if(NOT CMAKE_BUILD_TYPE)
				message("Defaulting to debug build")
				set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
			endif()

			set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
			set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")
		endif()

		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

		set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O2 -g")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g")
		set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
	endif()

	# Finds and activates ccache for builds
	find_program(CCACHE ccache)

	if(CCACHE OR CMAKE_CXX_COMPILER_LAUNCHER)
		if(NOT CMAKE_CXX_COMPILER_LAUNCHER)
			set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
		endif()

		message(STATUS "Using ccache (${CMAKE_CXX_COMPILER_LAUNCHER})")
	else()
		message(STATUS "Could not find ccache")
	endif()

	# Generates compile_commands.json
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

	# Disable compiler extensions to ensure compliance with ISO C++ standards
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
endmacro()
