#ifndef __NETWORK_BASE_H__
#define __NETWORK_BASE_H__

struct NetworkEvent {
    int fd_;
    bool is_read_;
    bool is_write_;
    bool is_error_;
};

/*
//抽象类实现
class NetworkBase {
public:
    virtual bool network_init(int max_event_num) noexcept = 0;
    virtual int network_event_monitor(NetWorkEvent *event_array, int timeout) noexcept = 0;
    virtual bool network_release() noexcept = 0;

    virtual bool add_fd(int fd) noexcept = 0;
    virtual bool del_fd(int fd) noexcept = 0;
    virtual bool enable_events(int fd, bool is_read, bool is_write) noexcept = 0;

    virtual ~NetworkBase() {};
};
*/

//想要抽象类的效果，但是不需要多态，改用CRTP实现，去掉虚函数的消耗
template<typename Derived>
class NetworkBase {
public:
    bool network_init(int max_event_num) noexcept {
        return static_cast<Derived*>(this)->impl_network_init(max_event_num);
    }
    int network_event_monitor(NetworkEvent *event_array, int timeout = -1) noexcept {
        return static_cast<Derived*>(this)->impl_network_event_monitor(event_array, timeout);
    }
    bool network_release() noexcept {
        return static_cast<Derived*>(this)->impl_network_release();
    }

    bool add_fd(int fd) noexcept {
        return static_cast<Derived*>(this)->impl_add_fd(fd);
    }
    bool del_fd(int fd) noexcept {
        return static_cast<Derived*>(this)->impl_del_fd(fd);
    }
    bool enable_events(int fd, bool is_read, bool is_write) noexcept {
        return static_cast<Derived*>(this)->impl_enable_events(fd, is_read, is_write);
    }
};

#endif