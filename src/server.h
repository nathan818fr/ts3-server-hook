#pragma once

#include <deque>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "asio.h"
#include "protocol.h"

namespace ts3_server_hook {

class Connection;
typedef std::shared_ptr<Connection> connection_ptr;
typedef std::function<void(connection_ptr)> connection_on_close_fn;

class Server {
  public:
    Server() : acceptor_(io_context_), heartbeat_timer_(io_context_) {}

    void start(const std::string& ts3_version);

    void capture(CaptureType type, int virtual_server_id, bool is_server_query, const void* data, int len);

  private:
    void do_accept();

    void do_heartbeat();

    void handle_connection_close(const connection_ptr& connection);

    std::string ts3_version_;

    asio::io_context io_context_;
    asio_localsp::acceptor acceptor_;
    asio::steady_timer heartbeat_timer_;
    uint32_t connections_id_counter_ = 0;
    std::set<connection_ptr> connections_;
};

class Connection : public std::enable_shared_from_this<Connection> {
  public:
    explicit Connection(std::string name, asio_localsp::socket socket, connection_on_close_fn on_close)
        : name_(std::move(name)), socket_(std::move(socket)), on_close_(std::move(on_close)) {}

    [[nodiscard]] const std::string& name() const {
        return name_;
    }

    void start();

    /**
     * Send a buffer to the client.
     * @attention Must be called from the io_context thread.
     *
     * @param buf The buffer to send.
     */
    void send(const buffer_ptr& buf);

  private:
    void do_read();

    void do_write();

    void close(const std::string& cause, const asio::error_code& err = {});

    std::string name_;
    asio_localsp::socket socket_;
    connection_on_close_fn on_close_;

    bool closed_ = false;
    std::deque<buffer_ptr> write_queue_;
};

}  // namespace ts3_server_hook
