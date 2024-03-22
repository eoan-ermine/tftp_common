#
# Add a target for formating the project using `clang-format` (i.e: cmake --build build --target format)
#

function(add_clang_format_target)
    if(NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
			find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
    endif()

    if(${PROJECT_NAME}_CLANG_FORMAT_BINARY)
            add_custom_target(format
                    COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
                    -i ${ALL_SOURCES}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
			message(STATUS "Format the project using the `clang-format` target (i.e: cmake --build build --target format).")

            add_custom_target(check-format
                    COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY} -Wno-error=unknown
                    -n --Werror ${ALL_SOURCES}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    endif()
endfunction()