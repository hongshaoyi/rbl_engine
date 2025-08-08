#include "gtest/gtest.h"
#include "network_epoll.h"

#include <sys/socket.h>

TEST(Epoll, InitMonitorRelease) {
    NetworkEpoll network_epoll;

    EXPECT_EQ(network_epoll.network_event_monitor(nullptr), -2);

    EXPECT_FALSE(network_epoll.network_release());

    ASSERT_TRUE(network_epoll.network_init(64));

    NetworkEvent *event_list = new NetworkEvent[64];
    ASSERT_EQ(network_epoll.network_event_monitor(event_list, 1000), 0);
    delete[] event_list;

    ASSERT_TRUE(network_epoll.network_release());
}

TEST(Epoll, AddOrDelFd) {
    NetworkEpoll network_epoll;

    ASSERT_TRUE(network_epoll.network_init(64));

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_epoll.add_fd(socket_fd));
    EXPECT_FALSE(network_epoll.add_fd(socket_fd));

    ASSERT_TRUE(network_epoll.del_fd(socket_fd));
    EXPECT_FALSE(network_epoll.del_fd(socket_fd));

    close(socket_fd);

    ASSERT_TRUE(network_epoll.network_release());
}

TEST(Epoll, EnableEvents) {
    NetworkEpoll network_epoll;

    ASSERT_TRUE(network_epoll.network_init(64));

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_epoll.add_fd(socket_fd));

    ASSERT_TRUE(network_epoll.enable_events(socket_fd, true, false));
    ASSERT_TRUE(network_epoll.enable_events(socket_fd, false, true));
    ASSERT_TRUE(network_epoll.enable_events(socket_fd, true, true));
    ASSERT_TRUE(network_epoll.enable_events(socket_fd, false, false));

    ASSERT_TRUE(network_epoll.del_fd(socket_fd));

    close(socket_fd);

    ASSERT_TRUE(network_epoll.network_release());
}