#include "network_select.h"

#include <cstdio>
#include <sys/time.h>
#include <errno.h>
#include <cstring>

using namespace std;

bool
NetworkSelect::impl_network_init(int max_event_num) noexcept {
    MAX_EVENT_NUM_ = max_event_num;

    return true;
}

int
NetworkSelect::impl_network_event_monitor(NetworkEvent *event_list, int timeout) noexcept {
    int now_index = 0;

    if (!event_queue_.empty()) {
        memcpy(&event_list[now_index], event_queue_.front().get(), NETWORK_EVENT_SIZE_);
        event_queue_.pop();
        ++now_index;

        if (now_index >= MAX_EVENT_NUM_)
            return now_index;
    }

    int max_fd = 0;
    FD_ZERO(&read_fds_);
    FD_ZERO(&write_fds_);
    FD_ZERO(&error_fds_);

    for (auto &iter : fd_map_) {
        auto &network_event = iter.second;
        bool is_set = false;

        if (network_event->is_read_)
            FD_SET(network_event->fd_, &read_fds_);

        if (network_event->is_write_)
            FD_SET(network_event->fd_, &write_fds_);

        if (network_event->is_error_)
            FD_SET(network_event->fd_, &error_fds_);

        if ((network_event->is_read_ || network_event->is_write_ || network_event->is_error_) && network_event->fd_ > max_fd)
            max_fd = network_event->fd_;
    }

    timeval tv {timeout / 1000000, timeout % 1000000};

    int result = select(max_fd + 1, &read_fds_, &write_fds_, &error_fds_, timeout < 0 ? nullptr : &tv);

    if (result == -1) {
        printf("[action:impl network event monitor]select failed! errno: %d, reason: %s\n", errno, strerror(errno));

        return -1;
    }

    if (result > 0) {
        unique_ptr<NetworkEvent> event(new NetworkEvent {0, false, false, false});

        for (auto &iter : fd_map_) {
            int fd = iter.second->fd_;

            event->is_read_ = FD_ISSET(fd, &read_fds_);
            event->is_write_ = FD_ISSET(fd, &write_fds_);
            event->is_error_ = FD_ISSET(fd, &error_fds_);

            if (event->is_read_ || event->is_write_ || event->is_error_) {
                event->fd_ = fd;

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
NetworkSelect::impl_add_fd(int fd) noexcept {
    if (fd >= 1024) {
        printf("[action:impl add fd]the fd is over than 1024!\n");

        return false;
    }

    if (fd_map_.find(fd) != fd_map_.end()) {
        printf("[action:impl add fd]the fd is repeat! fd: %d\n", fd);

        return false;
    }

    fd_map_.emplace(fd, new NetworkEvent {fd, true, false, true});

    return true;
}

bool
NetworkSelect::impl_del_fd(int fd) noexcept {
    if (fd_map_.find(fd) == fd_map_.end()) {
        printf("[action:impl add fd]the fd is not exist! fd: %d\n", fd);

        return false;
    }

    fd_map_.erase(fd);

    return true;
}

bool
NetworkSelect::impl_enable_events(int fd, bool is_read, bool is_write) noexcept {
    if (fd_map_.find(fd) == fd_map_.end()) {
        printf("[action:impl emable events]the fd is not exist! fd: %d\n", fd);

        return false;
    }

    fd_map_[fd]->is_read_ = is_read;
    fd_map_[fd]->is_write_ = is_write;

    return true;
}