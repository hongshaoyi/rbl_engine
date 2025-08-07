if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(NETWORK_BASE_NAME "network_epoll")
else()
    set(NETWORK_BASE_NAME "network_select")
endif()

set(NETWORK_MGR_NAME "network_mgr")
add_library(${NETWORK_MGR_NAME}_obj OBJECT ${NETWORK_MGR_NAME}.cpp ${NETWORK_BASE_NAME}/${NETWORK_BASE_NAME}.cpp)

if(IS_OPEN_TEST)
    add_library(${NETWORK_BASE_NAME}_obj OBJECT ${NETWORK_BASE_NAME}/${NETWORK_BASE_NAME}.cpp)
    add_library(${NETWORK_BASE_NAME}_interface INTERFACE)
    target_include_directories(${NETWORK_BASE_NAME}_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/${NETWORK_BASE_NAME}
    )

    add_library(${NETWORK_MGR_NAME}_interface INTERFACE)
    target_include_directories(${NETWORK_MGR_NAME}_interface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()