if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(NETWORK_BASE_NAME "network_epoll")
else()
    set(NETWORK_BASE_NAME "network_epoll")
endif()

add_library(network_mgr OBJECT network_mgr.cpp ${NETWORK_BASE_NAME}/${NETWORK_BASE_NAME}.cpp)