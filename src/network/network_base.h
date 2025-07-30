#ifndef __NETWORK_BASE_H__
#define __NETWORK_BASE_H__


struct NetWorkEvent {
    int fd_;
    bool is_read_;
    bool is_write_;
    bool is_error_;
};

class NetworkBase {
public:
    virtual bool network_init(int max_event_num) noexcept = 0;
    virtual int network_event_monitor(NetWorkEvent *event_list, int timeout) noexcept = 0;
    virtual bool network_release() noexcept = 0;

    virtual bool add_fd(int fd) noexcept = 0;
    virtual bool del_fd(int fd) noexcept = 0;
    virtual bool enable_events(int fd, bool is_read, bool is_write) noexcept = 0;

    virtual ~NetworkBase() {};
};

#endif