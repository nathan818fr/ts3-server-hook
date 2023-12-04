#include "server.h"

#include <algorithm>
#include <thread>

#include "asio.h"
#include "debug.h"
#include "util/cutils.h"

namespace ts3_server_hook {

void Server::start(const std::string& ts3_version) {
    if (DEBUG_ENABLED) {
        log() << "Start server (ts3_version=" << ts3_version << ")" << log_endl;
    }

    ts3_version_ = ts3_version;

    // Listen on a local socket
    try {
        std::string path_name;
        bool unlink_socket;
        const char* env_path_name = std::getenv("TS3SERVERHOOK_SOCKET_PATH");
        if (env_path_name && env_path_name[0]) {
            path_name = env_path_name;
            unlink_socket = false;  // user is responsible for cleaning up the socket
        } else {
            path_name = "ts3-server-hook.sock";
            unlink_socket = true;
        }

        auto endpoint = asio_localsp::endpoint(path_name);
        if (unlink_socket) {
            unlink(endpoint.path().c_str());  // ignore errors
        }
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(asio_localsp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        log() << "Socket server listening on: " << endpoint << log_endl;
    } catch (const std::exception& e) {
        log() << "Failed to start socket server: " << e.what() << log_endl;
    }

    // Run the io_context in a dedicated thread
    std::thread thr([this] {
        do_accept();     // Start accepting
        do_heartbeat();  // Start heartbeat timer
        try {
            io_context_.run();
        } catch (const std::exception& e) {
            log() << "Error running socket server: " << e.what() << log_endl;
        }
    });
    thr.detach();
}

void Server::capture(CaptureType type, int virtual_server_id, bool is_server_query, const void* data, int len) {
    if (DEBUG_ENABLED) {
        log() << "Captured packet (type=" << to_underlying(type) << ", virtual_server_id=" << virtual_server_id
              << ", is_server_query=" << log_bool(is_server_query) << ", len=" << len << ")" << log_endl
              << log_hex_dump(data, len) << log_endl;
    }

    // Broadcast capture packet
    auto packet = Protocol::write_capture(type, virtual_server_id, is_server_query, data, len);
    asio::post(io_context_, [this, packet = std::move(packet)] {
        for (const auto& connection : connections_) {
            connection->send(packet);
        }
    });
}

void Server::do_accept() {
    acceptor_.async_accept([this](const asio::error_code& err, asio_localsp::socket new_socket) {
        if (!err) {
            auto name = std::to_string(++connections_id_counter_);
            auto connection =
                std::make_shared<Connection>(name, std::move(new_socket), TS3SH_HANDLER(handle_connection_close));

            connections_.insert(connection);
            log() << "Socket client connected (connection=" << connection->name()
                  << ", #connections=" << connections_.size() << ")" << log_endl;
            connection->start();
            connection->send(Protocol::write_handshake(ts3_version_));  // Send handshake
        }

        do_accept();  // Resume accepting
    });
}

void Server::do_heartbeat() {
    heartbeat_timer_.expires_after(asio::chrono::milliseconds(Protocol::HEARTBEAT_INTERVAL_MS));
    heartbeat_timer_.async_wait([this](const asio::error_code& err) {
        if (!err) {
            auto packet = Protocol::write_hearbeat();
            for (const auto& connection : connections_) {
                connection->send(packet);
            }
        }

        do_heartbeat();  // Resume heartbeat
    });
}

void Server::handle_connection_close(const connection_ptr& connection) {
    connections_.erase(connection);
    log() << "Socket client disconnected (connection=" << connection->name() << ", #connections=" << connections_.size()
          << ")" << log_endl;
}

void Connection::start() {
    do_read();  // Start reading (needed to detect disconnections or errors)
}

void Connection::send(const buffer_ptr& buf) {
    if (closed_) {
        return;  // Don't bother sending data to a closed connection
    }

    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(buf);  // Append buffer to the write queue to retain it
    if (!write_in_progress) {     // Resume writing if needed
        do_write();
    }
}

static char g_ignored_read_buf_[1] = {};

void Connection::do_read() {
    auto self = shared_from_this();  // Retain self during async operation
    asio::async_read(socket_, asio::buffer(&g_ignored_read_buf_, 1), [this, self](const asio::error_code& err, size_t) {
        if (err) {
            close("read", err);
            return;
        }

        // Fully ignore the incoming data
        // (maybe in the future we can add some sort of control/query protocol)

        do_read();  // Resume reading
    });
}

void Connection::do_write() {
    auto self = shared_from_this();  // Retain self during async operation
    asio::async_write(
        socket_, asio::buffer(write_queue_.front()->data(), write_queue_.front()->len()),
        [this, self](const asio::error_code& err, std::size_t) {
            if (err) {
                // Clear the write queue on error to avoid keeping references to buffers longer than needed
                // Don't move this to the close() method to keep write responsability to the narrowest scope
                // Note that:
                // - Any close() while do_write() is in progress will cause this branch to be called
                // - Nothing more could be added to this queue after the close() call
                write_queue_.clear();
                close("write", err);
                return;
            }

            write_queue_.pop_front();     // Release the buffer we just wrote
            if (!write_queue_.empty()) {  // And continue writing if needed
                do_write();
            }
        }
    );
}

void Connection::close(const std::string& cause, const asio::error_code& err) {
    if (closed_) {
        return;
    }
    closed_ = true;

    // Log unexpected errors
    if (err && err != asio::error::eof) {
        log() << "Socket client encountered an error (connection=" << name_ << ", cause=" << cause
              << ", err=" << err.message() << ")" << log_endl;
    }

    // Close the socket, ignoring exceptions as the underlying descriptor is closed anyway
    try {
        socket_.close();
    } catch (const std::exception&) {
    }

    // Notify the connection owner
    on_close_(shared_from_this());
}

}  // namespace ts3_server_hook
