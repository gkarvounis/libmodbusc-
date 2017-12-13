#ifndef MODBUS_SERVER_SESSION_HPP
#define MODBUS_SERVER_SESSION_HPP


#include <boost/asio.hpp>
#include <iostream>

namespace modbus {
namespace tcp {


class ServerSession : public std::enable_shared_from_this<ServerSession> {
public:
    inline                          ServerSession(boost::asio::io_service& io, ServerDevice& device);
    inline                          ~ServerSession();

    boost::asio::ip::tcp::socket&   socket();
    void                            start(std::function<void(void)> done_cb);
    inline void                     stop();

private:
    ServerDevice                   &m_device;
    boost::asio::ip::tcp::socket    m_socket;
    std::vector<uint8_t>            m_rx_buffer;
    std::vector<uint8_t>            m_tx_buffer;
    std::function<void(void)>       m_done_cb;

    void                            init_header_reception();
    void                            on_header_received(const boost::system::error_code& ec);
    void                            init_payload_reception();
    void                            on_payload_received(const boost::system::error_code& ec);
    void                            init_response_sending();
    void                            on_response_sent(const boost::system::error_code& ec);
};


ServerSession::ServerSession(boost::asio::io_service& io, ServerDevice& device) :
    m_device(device),
    m_socket(io),
    m_rx_buffer(),
    m_tx_buffer()
{
    std::cout << "new session " << this << std::endl;
}


ServerSession::~ServerSession() {
    std::cout << "end session " << this << std::endl;
}


boost::asio::ip::tcp::socket& ServerSession::socket() {
    return m_socket;
}


void ServerSession::start(std::function<void(void)> done_cb) {
    m_done_cb = done_cb;
    init_header_reception();
}


void ServerSession::init_header_reception() {
    auto self = this->shared_from_this();

    m_rx_buffer.resize(sizeof(modbus::tcp::Header));

    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(m_rx_buffer, sizeof(modbus::tcp::Header)),
        [self, this](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            on_header_received(ec);
        });
}


void ServerSession::on_header_received(const boost::system::error_code& ec) {
    if (ec) {
        m_done_cb();
        m_done_cb = nullptr;
        return;
    }

    init_payload_reception();
}


void ServerSession::init_payload_reception() {
    modbus::tcp::decoder_views::Header view_header(m_rx_buffer);

    std::size_t payload_size = view_header.getLength() - 2;

    m_rx_buffer.resize(sizeof(modbus::tcp::Header) + payload_size);
    uint8_t* ptr = m_rx_buffer.data() + sizeof(modbus::tcp::Header);

    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(ptr, payload_size),
        [this](const boost::system::error_code& ec, std::size_t) {
            on_payload_received(ec);
        });
}


void ServerSession::on_payload_received(const boost::system::error_code& ec) {
    if (ec) {
        m_done_cb();
        m_done_cb = nullptr;
        return;
    }

    m_device.handleMessage(m_rx_buffer, m_tx_buffer);
    init_response_sending();
}


void ServerSession::init_response_sending() {
    auto self = this->shared_from_this();

    boost::asio::async_write(
        m_socket,
        boost::asio::buffer(m_tx_buffer),
        [self, this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
            on_response_sent(ec);
        });
}


void ServerSession::on_response_sent(const boost::system::error_code& ec) {
    if (ec) {
        if (m_done_cb) {
            m_done_cb();
            m_done_cb = nullptr;
        }
        return;
    }

    init_header_reception();
}


void ServerSession::stop() {
    auto self = this->shared_from_this();

    m_socket.get_io_service().post([self, this]() {
        m_socket.close();
    });
}

} // namespace tcp
} // namespace modbus

#endif
