#ifndef __NETWORK_SELECT_H__
#define __NETWORK_SELECT_H__

#include "../network_base.h"
#include <unordered_map>
#include <memory>
#include <queue>

#ifdef __linux__
#include <sys/select.h>
#else
#include <winsock2.h>
#endif

class NetworkSelect final : public NetworkBase<NetworkSelect> {
    fd_set read_fds_;
    fd_set write_fds_;
    fd_set error_fds_;
    std::unordered_map<int, std::unique_ptr<NetworkEvent>> fd_map_;
    std::queue<std::unique_ptr<NetworkEvent>> event_queue_;
    int MAX_EVENT_NUM_ = 0;
    const int NETWORK_EVENT_SIZE_ = sizeof(NetworkEvent);

    friend NetworkBase<NetworkSelect>;

    bool impl_network_init(int max_event_num) noexcept;
    int impl_network_event_monitor(NetworkEvent *event_list, int timeout) noexcept;
    bool impl_network_release() noexcept {return true;};

    bool impl_add_fd(int fd) noexcept;
    bool impl_del_fd(int fd) noexcept;
    bool impl_enable_events(int fd, bool is_read, bool is_write) noexcept;

public:
    NetworkSelect() = default;
    ~NetworkSelect() = default;

    NetworkSelect(NetworkSelect &select) = delete;
    NetworkSelect& operator=(NetworkSelect &select) = delete;
    NetworkSelect(NetworkSelect &&select) = delete;
    NetworkSelect& operator=(NetworkSelect &&select) = delete;
};

#endif