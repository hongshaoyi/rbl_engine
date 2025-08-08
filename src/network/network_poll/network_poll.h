#ifndef __NETWORK_POLL_H__
#define __NETWORK_POLL_H__

#include "../network_base.h"
#include <poll.h>
#include <unordered_map>
#include <memory>
#include <queue>

class NetworkPoll final : public NetworkBase<NetworkPoll> {
    static const int MAX_ARRAY_SIZE_ = 65535;
    pollfd fd_array_[MAX_ARRAY_SIZE_];
    int now_size_ = 0;
    std::unordered_map<int, int> fd_map_;
    std::queue<std::unique_ptr<NetworkEvent>> event_queue_;
    int MAX_EVENT_NUM_ = 0;
    const int NETWORK_EVENT_SIZE_ = sizeof(NetworkEvent);

    friend NetworkBase<NetworkPoll>;

    bool impl_network_init(int max_event_num) noexcept;
    int impl_network_event_monitor(NetworkEvent *event_array, int timeout) noexcept;
    bool impl_network_release() noexcept {return true;};

    bool impl_add_fd(int fd) noexcept;
    bool impl_del_fd(int fd) noexcept;
    bool impl_enable_events(int fd, bool is_read, bool is_write) noexcept;

public:
    NetworkPoll() = default;
    ~NetworkPoll() = default;

    NetworkPoll(NetworkPoll &poll) = delete;
    NetworkPoll& operator=(NetworkPoll &poll) = delete;
    NetworkPoll(NetworkPoll &&poll) = delete;
    NetworkPoll& operator=(NetworkPoll &&poll) = delete;
};

#endif