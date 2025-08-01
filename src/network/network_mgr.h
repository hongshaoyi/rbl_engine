#ifndef __NETWORK_H__
#define __NETWORK_H__

#ifdef __linux__
#include "network_epoll/network_epoll.h"
using Network = NetworkEpoll;
#else
#include "network_epoll/io_multiplexing_epoll.h"
using IOMultiplexing = IOMultiplexingEpoll;
#endif

#include <string>
#include <unordered_map>
#include <list>

class NetworkMgr {
    enum SOCKET_TYPE {
        LISTENED_TYPE,
        CONNECTED_TYPE
    };
    struct ReadBufferData {
        char *buffer_;
        int size_;

        ReadBufferData(int size) {buffer_ = size > 0 ? new char[size] : nullptr; size_ = size > 0 ? size : 0;}
        ~ReadBufferData() {if (size_ > 0) delete[] buffer_;}
    };
    struct WriteBufferData {
        char *buffer_;
        char *start_ptr_;
        int size_;
        int remain_size_;

        WriteBufferData(char *buffer, int size) : buffer_{buffer}, start_ptr_{buffer}, size_{size_}, remain_size_{size_} {};
        ~WriteBufferData() {if (size_ > 0) delete[] buffer_; start_ptr_ = nullptr;};
    };
    struct SocketData {
        std::list<ReadBufferData*> read_list_;
        std::list<WriteBufferData*> write_list_;
        int fd_;
        SOCKET_TYPE socket_type_;

        SocketData(int fd, SOCKET_TYPE socket_type) : fd_{fd}, socket_type_{socket_type} {};
        ~SocketData() {
            for (auto iter = read_list_.begin(); iter != read_list_.end(); ++iter)
                delete *iter;

            for (auto iter = write_list_.begin(); iter != write_list_.end(); ++iter)
                delete *iter;
        }
    };
    static const int DEFAULT_BACKLOG_ = 128;
    static const int MAX_EVENT_NUM_ = 64;
    static const int READ_BUFFER_SIZE_ = 128;
    bool is_inited_ = false;
    NetworkEvent event_list_[MAX_EVENT_NUM_];
    std::unordered_map<int, SocketData*> socket_list_;
    Network network_;

    void set_socket_noblocking(int fd) noexcept;
    bool add_socket(int fd, SOCKET_TYPE socket_type) noexcept;
    void remove_socket(int fd) noexcept;
    void accept_socket(int fd) noexcept;
    void read_socket(SocketData *data) noexcept;
    void write_socket(SocketData *data) noexcept;
    void deal_read_event(int fd) noexcept;
    void deal_write_event(int fd) noexcept;
    void deal_error_event(int fd) noexcept;

public:
    bool network_init() noexcept;
    bool network_loop(int timeout = -1) noexcept;
    bool network_exit() noexcept;

    bool listen_socket(const std::string &ip, const int port) noexcept;
};

#endif