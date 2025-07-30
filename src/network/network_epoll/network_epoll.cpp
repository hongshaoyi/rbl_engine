#include "network_epoll.h"

#include <sys/epoll.h>
#include <cstdio>
#include <unistd.h>

bool
NetworkEpoll::network_init(int max_event_num) noexcept {
    epoll_fd_ = epoll_create(1024);

    if (epoll_fd_ <= 0) {
        printf("[action:io multiplexing init]create epoll failed!\n");

        return false;
    }

    MAX_EVENT_NUM_ = max_event_num;

    return true;
}

int
NetworkEpoll::network_event_monitor(NetWorkEvent *event_list, int timeout) noexcept {
    if (epoll_fd_ <= 0) {
        printf("[action:io multiplexing loop]epoll is not inited!\n");

        return -1;
    }

    epoll_event events[MAX_EVENT_NUM_];

    int event_num = epoll_wait(epoll_fd_, events, MAX_EVENT_NUM_, timeout);

    for (int index = 0; index < event_num; ++index) {
        event_list[index].fd_ = events[index].data.fd;

        uint32_t event_flag = events[index].events;
        event_list[index].is_read_ = (event_flag & EPOLLIN) != 0;
        event_list[index].is_write_ = (event_flag & EPOLLOUT) != 0;
        event_list[index].is_error_ = (event_flag & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) != 0;
    }

    return event_num;
}

bool
NetworkEpoll::network_release() noexcept {
    if (epoll_fd_ <= 0) {
        printf("[action:io multiplexing loop]epoll is not inited!\n");

        return false;
    }

    close(epoll_fd_);

    return true;
}

bool
NetworkEpoll::add_fd(int fd) noexcept {
    epoll_event event;
    event.events = EPOLLRDHUP | EPOLLIN;
    event.data.fd = fd;

    int result = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);

    if (result != 0) {
        printf("[action:add fd]add failed!\n");

        return false;
    }

    return true;
}

bool
NetworkEpoll::del_fd(int fd) noexcept {
    int result = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);

    if (result != 0) {
        printf("[action:del fd]del failed!\n");

        return false;
    }

    return true;
}

bool
NetworkEpoll::enable_events(int fd, bool is_read, bool is_write) noexcept {
    epoll_event event;
    event.events = EPOLLRDHUP | (is_read ? EPOLLIN : 0) | (is_write ? EPOLLOUT : 0);
    event.data.fd = fd;

    int result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);

    if (result != 0) {
        printf("[action:enable events]enable failed!\n");

        return false;
    }

    return true;
}