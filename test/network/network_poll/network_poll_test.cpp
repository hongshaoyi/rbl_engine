#include "gtest/gtest.h"
#include "network_poll.h"

#include <sys/socket.h>

TEST(Poll, InitMonitorRelease) {
    NetworkPoll network_poll;

    ASSERT_TRUE(network_poll.network_init(64));

    NetworkEvent *event_list = new NetworkEvent[64];
    ASSERT_EQ(network_poll.network_event_monitor(event_list, 1000), 0);
    delete[] event_list;

    ASSERT_TRUE(network_poll.network_release());
}

TEST(Poll, AddOrDelFd) {
    NetworkPoll network_poll;

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_poll.add_fd(socket_fd));
    EXPECT_FALSE(network_poll.add_fd(socket_fd));

    ASSERT_TRUE(network_poll.del_fd(socket_fd));
    EXPECT_FALSE(network_poll.del_fd(socket_fd));

    close(socket_fd);
}

TEST(Poll, EnableEvents) {
    NetworkPoll network_poll;

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    ASSERT_TRUE(network_poll.add_fd(socket_fd));

    ASSERT_TRUE(network_poll.enable_events(socket_fd, true, false));
    ASSERT_TRUE(network_poll.enable_events(socket_fd, false, true));
    ASSERT_TRUE(network_poll.enable_events(socket_fd, true, true));
    ASSERT_TRUE(network_poll.enable_events(socket_fd, false, false));

    ASSERT_TRUE(network_poll.del_fd(socket_fd));

    close(socket_fd);
}