#ifndef __NETWORK_EPOLL_H__
#define __NETWORK_EPOLL_H__

#include "../network_base.h"

class NetworkEpoll final : public NetworkBase<NetworkEpoll> {
    int epoll_fd_ = 0;
    int MAX_EVENT_NUM_ = 0;

    //利用友元特性来使父类能调用子类的私有函数
    //从而将impl相关函数转为私有，防止外部调用
    friend NetworkBase<NetworkEpoll>;

    bool impl_network_init(int max_event_num) noexcept;
    int impl_network_event_monitor(NetworkEvent *event_array, int timeout) noexcept;
    bool impl_network_release() noexcept;

    bool impl_add_fd(int fd) noexcept;
    bool impl_del_fd(int fd) noexcept;
    bool impl_enable_events(int fd, bool is_read, bool is_write) noexcept;

public:
    NetworkEpoll() = default;
    ~NetworkEpoll() = default;

    NetworkEpoll(NetworkEpoll &epoll) = delete;
    NetworkEpoll& operator=(NetworkEpoll &epoll) = delete;
    NetworkEpoll(NetworkEpoll &&epoll) = delete;
    NetworkEpoll& operator=(NetworkEpoll &&epoll) = delete;
};

#endif