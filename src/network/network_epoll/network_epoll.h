#ifndef __NETWORK_EPOLL_H__
#define __NETWORK_EPOLL_H__

#include "../network_base.h"

class NetworkEpoll : public NetworkBase {
    int epoll_fd_ = 0;
    int MAX_EVENT_NUM_ = 0;

public:
    bool network_init(int max_event_num) noexcept override;
    int network_event_monitor(NetWorkEvent *event_list, int timeout = -1) noexcept override;
    bool network_release() noexcept override;

    bool add_fd(int fd) noexcept override;
    bool del_fd(int fd) noexcept override;
    bool enable_events(int fd, bool is_read, bool is_write) noexcept override;

    NetworkEpoll() {};
    ~NetworkEpoll() {};

    NetworkEpoll(NetworkEpoll &epoll) = delete;
    NetworkEpoll& operator=(NetworkEpoll &epoll) = delete;
    NetworkEpoll(NetworkEpoll &&epoll) = delete;
    NetworkEpoll& operator=(NetworkEpoll &&epoll) = delete;
};

#endif