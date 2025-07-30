#include "base/singleton.h"
#include "network/network_mgr.h"

#include <cstdio>

int
main(int argc, char *argv[])
{
    NetworkMgr &network_mgr = Singleton<NetworkMgr>::get_instance();

    network_mgr.network_init();
//    network_mgr.network_loop(1000);
    network_mgr.network_exit();

    printf("test success!\n");

    return 0;
}