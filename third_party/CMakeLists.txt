if (IS_OPEN_TEST)
    set(GOOGLE_TEST_DIRECTORY "googletest-1.17.0")

    add_library(test_common_interface INTERFACE)
    target_include_directories(test_common_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/${GOOGLE_TEST_DIRECTORY}/googletest/include
    )

    add_subdirectory(${GOOGLE_TEST_DIRECTORY})
endif()