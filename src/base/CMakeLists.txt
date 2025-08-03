if (IS_OPEN_TEST)
    add_library(singleton_interface INTERFACE)
    target_include_directories(singleton_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()