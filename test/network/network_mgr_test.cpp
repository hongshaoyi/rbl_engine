#include "gtest/gtest.h"
#include "network_mgr.h"

TEST(NetworkMgr, InitLoopExit) {
    NetworkMgr network_mgr;

    EXPECT_FALSE(network_mgr.network_loop());
    EXPECT_FALSE(network_mgr.network_exit());

    ASSERT_TRUE(network_mgr.network_init());

    //network_loop暂时测不了，后面补
    //ASSERT_TRUE(network_mgr.network_loop(1000));

    ASSERT_TRUE(network_mgr.network_exit());
}

TEST(NetworkMgr, ListenScoket) {
    NetworkMgr network_mgr;

    ASSERT_TRUE(network_mgr.network_init());
    ASSERT_TRUE(network_mgr.listen_socket("127.0.0.1", 20000));
    ASSERT_TRUE(network_mgr.network_exit());
}