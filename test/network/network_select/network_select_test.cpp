#include "gtest/gtest.h"
#include "network_select.h"

#include <sys/socket.h>

TEST(Select, InitMonitorRelease) {
    NetworkSelect network_select;

    ASSERT_TRUE(network_select.network_init(64));

    NetworkEvent *event_list = new NetworkEvent[64];
    ASSERT_EQ(network_select.network_event_monitor(event_list, 1000), 0);
    delete[] event_list;

    ASSERT_TRUE(network_select.network_release());
}

TEST(Select, AddOrDelFd) {
    NetworkSelect network_select;

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_select.add_fd(socket_fd));
    EXPECT_FALSE(network_select.add_fd(socket_fd));

    ASSERT_TRUE(network_select.del_fd(socket_fd));
    EXPECT_FALSE(network_select.del_fd(socket_fd));

    close(socket_fd);
}

TEST(Select, EnableEvents) {
    NetworkSelect network_select;

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_select.add_fd(socket_fd));

    ASSERT_TRUE(network_select.enable_events(socket_fd, true, false));
    ASSERT_TRUE(network_select.enable_events(socket_fd, false, true));
    ASSERT_TRUE(network_select.enable_events(socket_fd, true, true));
    ASSERT_TRUE(network_select.enable_events(socket_fd, false, false));

    ASSERT_TRUE(network_select.del_fd(socket_fd));

    close(socket_fd);
}