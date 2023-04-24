include_guard()

macro(set_lib_output_directories base subdir)
	# _LIBRARY_ is used by linux, whereas windows is _RUNTIME_.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${base}/${CMAKE_BUILD_TYPE}/${subdir}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${base}/${CMAKE_BUILD_TYPE}/${subdir}")

	foreach(CONFIG_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES)
		string(TOUPPER ${CONFIG_TYPE} CONFIG_PROPERTY)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} "${base}/${CONFIG_TYPE}/${subdir}")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} "${base}/${CONFIG_TYPE}/${subdir}")
	endforeach()
endmacro()

macro(set_lib_ints_directories base subdir)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${base}/${CMAKE_BUILD_TYPE}/${subdir}")
	set(CMAKE_PDB_OUTPUT_DIRECTORY "${base}/${CMAKE_BUILD_TYPE}/${subdir}")

	foreach(CONFIG_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES)
		string(TOUPPER ${CONFIG_TYPE} CONFIG_PROPERTY)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} "${base}/${CONFIG_TYPE}/${subdir}")
		set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} "${base}/${CONFIG_TYPE}/${subdir}")
	endforeach()
endmacro()

macro(set_app_ito app)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result)

	if(result)
		set_target_properties(${app} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
			INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON
		)
	endif()
endmacro()
