#include "network_mgr.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;

void
NetworkMgr::set_socket_noblocking(int fd) noexcept {
    int flag = fcntl(fd, F_GETFL, 0);

    if (flag == -1)
        return;

    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

bool
NetworkMgr::add_socket(int fd, SOCKET_TYPE socket_type) noexcept {
    if (socket_list_.find(fd) != socket_list_.end()) {
        printf("[action:add socket]the fd is exist, fd: %d\n", fd);

        return false;
    }

    if (!network_.add_fd(fd)) {
        printf("[action:add socket]fd add failed, fd: %d\n", fd);

        return false;
    }

    set_socket_noblocking(fd);

    socket_list_.emplace(fd, new SocketData {fd, socket_type});

    return true;
}

void
NetworkMgr::remove_socket(int fd) noexcept {
    if (socket_list_.find(fd) == socket_list_.end()) {
        printf("[action:remove socket]the fd is not exist, fd: %d\n", fd);

        return;
    }

    network_.del_fd(fd);
    socket_list_.erase(fd);
}

void
NetworkMgr::accept_socket(int fd) noexcept {
    sockaddr_in address;
    socklen_t addr_len = sizeof(address);

    memset(&address, 0, addr_len);

    int client_fd = accept(fd, reinterpret_cast<sockaddr*>(&address), &addr_len);

    if (client_fd < 0) {
        printf("[action:accept socket]accept failed, errno: %d, reason: %s\n", errno, strerror(errno));

        return;
    }

    if (!add_socket(client_fd, CONNECTED_TYPE)) {
        printf("[action:accept socket]add failed, fd: %d\n", client_fd);

        close(client_fd);

        return;
    }
}

void
NetworkMgr::read_socket(unique_ptr<SocketData> &data) noexcept {
    unique_ptr<ReadBufferData> new_data(new ReadBufferData {READ_BUFFER_SIZE_});

    for (;;) {
        int read_len = read(data->fd_, new_data->buffer_.get(), READ_BUFFER_SIZE_);

        if (read_len <= 0) {
            if (read_len == 0)
                deal_error_event(data->fd_);
            else
                switch (errno) {
                    case EINTR:
                    case EAGAIN:
                        break;
                    default:
                        printf("[action:read socket]no deal error, errno: %d, reason: %s\n", errno, strerror(errno));
                }

            break;
        }
        else {
            data->read_list_.emplace_back(new_data.release());

            if (read_len == READ_BUFFER_SIZE_)
                new_data.reset(new ReadBufferData {READ_BUFFER_SIZE_});
            else
                break;
        }
    }
}

void
NetworkMgr::write_socket(unique_ptr<SocketData> &data) noexcept {
    while (!data->write_list_.empty()) {
        unique_ptr<WriteBufferData> &buffer = data->write_list_.front();

        for (;;) {
            int write_len = write(data->fd_, buffer->get_start_ptr(), buffer->remain_size_);

            if (write_len <= 0) {
                switch (errno) {
                    case EINTR:
                        continue;
                    case EAGAIN:
                        break;
                    default:
                        printf("[action:write socket]no deal error, errno: %d, reason: %s\n", errno, strerror(errno));

                        break;
                }

                return;
            }
            else {
                buffer->remain_size_ -= write_len;

                if (buffer->remain_size_ > 0)
                    return;
                else
                    data->write_list_.pop_front();
            }
        }
    }
}

void
NetworkMgr::deal_read_event(int fd) noexcept {
    if (socket_list_.find(fd) == socket_list_.end()) {
        printf("[action:deal read event]the fd is not exist, fd: %d\n", fd);

        return;
    }

    unique_ptr<SocketData> &data = socket_list_[fd];

    switch (data->socket_type_) {
        case LISTENED_TYPE:
            accept_socket(fd);

            break;
        case CONNECTED_TYPE:
            read_socket(data);

            break;
        default:
            printf("[action:deal read event]unknown socket type: %d\n", data->socket_type_);

            break;
    }
}

void
NetworkMgr::deal_write_event(int fd) noexcept {
    if (socket_list_.find(fd) == socket_list_.end()) {
        printf("[action:deal write event]the fd is not exist, fd: %d\n", fd);

        return;
    }

    unique_ptr<SocketData> &data = socket_list_[fd];

    write_socket(data);
}

void
NetworkMgr::deal_error_event(int fd) noexcept {
    remove_socket(fd);
    close(fd);
}

bool
NetworkMgr::network_init() noexcept {
    bool result = network_.network_init(MAX_EVENT_NUM_);

    if (!result) {
        printf("[action:network init]init failed!\n");

        return false;
    }

    is_inited_ = true;

    return true;
}

bool
NetworkMgr::network_loop(int timeout) noexcept {
    while (true) {
        int event_num = network_.network_event_monitor(event_list_, timeout);

        if (event_num < 0)
            if (event_num == -1) {
                printf("[action:network loop]loop error, errno: %d, reason: %s\n", errno, strerror(errno));

                switch (errno) {
                    case EINTR:
                        printf("[action:network loop]interrupted by singal, continue!\n");

                        continue;
                    case ENOMEM:
                        printf("[action:network loop]out of memory, try later!\n");
                        sleep(1);

                        continue;
                    default:
                        printf("[action:]other error, stop!\n");

                        return false;
                }
            }
            else if (event_num == -2) {
                printf("[action:network loop]customize error!\n");

                return false;
            }
            else {
                printf("[action:network loop]unknown error!\n");

                return false;
            }

        for (int index = 0; index < event_num; ++index) {
            NetworkEvent event = event_list_[index];

            if (event.is_error_) {
                deal_error_event(event.fd_);

                continue;
            }

            if (event.is_read_)
                deal_read_event(event.fd_);

            if (event.is_write_)
                deal_write_event(event.fd_);
        }
    }

    return true;
}

bool
NetworkMgr::network_exit() noexcept {
    bool result = network_.network_release();

    if (!result) {
        printf("[action:network exit]release failed!\n");

        return false;
    }

    return true;
}

bool
NetworkMgr::listen_socket(const string &ip, const int port) noexcept {
    if (ip.length() == 0) {
        printf("[action:listen socket]the ip is invailed, ip: %s\n", ip);

        return false;
    }

    if (port <= 0) {
        printf("[action:listen socket]the port is invailed, port: %d\n", port);

        return false;
    }

    if (!is_inited_) {
        printf("[action:listen socket]the network is not inited!\n");

        return false;
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        printf("[action:listen socket]create socket failed, errno: %d, reason: %s\n", errno, strerror(errno));

        return false;
    }

    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &(address.sin_addr));
    address.sin_port = htons(port);

    int bind_result = bind(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));

    if (bind_result != 0) {
        printf("[action:listen socket]bind failed, errno: %d, reason: %s\n", errno, strerror(errno));

        switch (errno) {
            case EACCES:
                printf("[action:listen socket]the port is protected, port: %d\n", port);

                break;
            case EADDRINUSE:
                printf("[action:listen socket]the port is in use, port: %d\n", port);

                break;
            default:
                printf("[action:listen socket]other error!\n");

                break;
        }

        close(socket_fd);

        return false;
    }


    int listen_result = listen(socket_fd, DEFAULT_BACKLOG_);

    if (listen_result != 0) {
        printf("[action:listen socket]listen failed, errno: %d, reason: %s\n", errno, strerror(errno));

        close(socket_fd);

        return false;
    }

    if (!add_socket(socket_fd, LISTENED_TYPE)) {
        printf("[action:listen socket]add fd failed, fd: %d\n", socket_fd);

        close(socket_fd);

        return false;
    }

    return true;
}
