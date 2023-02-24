include_guard()

# Enables static analysis for this CMake project
function(enable_static_analysis)
    option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

    # Optionally enable the clang-tidy static analysis tool if it exists
    if(ENABLE_CLANG_TIDY)
        find_program(CLANGTIDY clang-tidy)

        if(CLANGTIDY)
            set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
        else()
            message(SEND_ERROR "Static analysis option 'clang-tidy' requested but executable was not found.")
        endif()
    endif()
endfunction()
