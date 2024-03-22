#
# Add a target for generating docs for the project using `doxygen` (i.e: cmake --build build --target docs)
#

function(add_docs_target)
    find_package(Doxygen COMPONENTS dot)

    if(Doxygen_FOUND)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
        doxygen_add_docs(docs ${PROJECT_SOURCE_DIR})
        message(STATUS "Doxygen has been setup and docs target is now available.")
    endif()
endfunction()