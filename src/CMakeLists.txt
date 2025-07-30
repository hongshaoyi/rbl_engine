add_subdirectory(network)

add_executable(rbl_engine engine_main.cpp)
target_link_libraries(rbl_engine PRIVATE
    $<TARGET_OBJECTS:network_mgr_obj>
)