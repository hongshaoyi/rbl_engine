#include "network_poll.h"

#include <cstdio>
#include <errno.h>
#include <cstring>

using namespace std;

bool
NetworkPoll::impl_network_init(int max_event_num) noexcept {
    MAX_EVENT_NUM_ = max_event_num;

    return true;
}

int
NetworkPoll::impl_network_event_monitor(NetworkEvent *event_list, int timeout) noexcept {
    int now_index = 0;

    if (!event_queue_.empty()) {
        memcpy(&event_list[now_index], event_queue_.front().get(), NETWORK_EVENT_SIZE_);
        event_queue_.pop();
        ++now_index;

        if (now_index >= MAX_EVENT_NUM_)
            return now_index;
    }

    int result = poll(fd_array_, now_size_, timeout);

    if (result == -1) {
        printf("[action:impl network event monitor]poll failed, errno: %d, reason: %s\n", errno, strerror(errno));

        return -1;
    }

    if (result > 0) {
        unique_ptr<NetworkEvent> event(new NetworkEvent {0, false, false, false});

        for (int index = 0; index < now_size_; ++index) {
            pollfd &fd_info = fd_array_[index];
            short event_flags = fd_info.revents;

            event->is_read_ = (event_flags & POLLIN) != 0;
            event->is_write_ = (event_flags & POLLOUT) != 0;
            event->is_error_ = (event_flags & (POLLHUP | POLLRDHUP | POLLERR)) != 0;

             if (event->is_read_ || event->is_write_ || event->is_error_) {
                event->fd_ = fd_info.fd;

                if (now_index < MAX_EVENT_NUM_) {
                    memcpy(&event_list[now_index], event.get(), NETWORK_EVENT_SIZE_);
                    memset(event.get(), 0, NETWORK_EVENT_SIZE_);
                    ++now_index;
                }
                else {
                    event_queue_.emplace(event.release());
                    event.reset(new NetworkEvent {0, false, false, false});
                }
            }
        }
    }

    return now_index;
}

bool
NetworkPoll::impl_add_fd(int fd) noexcept {
    if (now_size_ >= MAX_ARRAY_SIZE_) {
        printf("[action:impl add fd]the fd array is full!\n");

        return false;
    }

    if (fd_map_.find(fd) != fd_map_.end()) {
        printf("[action:impl add fd]the fd is repeat, fd: %d\n", fd);

        return false;
    }

    fd_map_.emplace(fd, now_size_);

    fd_array_[now_size_].fd = fd;
    fd_array_[now_size_].events = POLLRDHUP | POLLIN;
    fd_array_[now_size_].revents = 0;
    fd_map_.emplace(fd, now_size_);
    ++now_size_;

    return true;
}

bool
NetworkPoll::impl_del_fd(int fd) noexcept {
    if (fd_map_.find(fd) == fd_map_.end()) {
        printf("[action:impl add fd]the fd is not exist, fd: %d\n", fd);

        return false;
    }

    int index = fd_map_[fd];

    swap(fd_array_[index], fd_array_[now_size_ - 1]);
    fd_map_[fd_array_[index].fd] = index;
    fd_map_.erase(fd);
    --now_size_;

    return true;
}

bool
NetworkPoll::impl_enable_events(int fd, bool is_read, bool is_write) noexcept {
    if (fd_map_.find(fd) == fd_map_.end()) {
        printf("[action:impl emable events]the fd is not exist, fd: %d\n", fd);

        return false;
    }

    int index = fd_map_[fd];
    pollfd &fd_info = fd_array_[index];

    fd_info.events = POLLRDHUP | (is_read ? POLLIN : 0) | (is_write ? POLLOUT : 0);

    return true;
}