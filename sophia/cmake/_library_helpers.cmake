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
