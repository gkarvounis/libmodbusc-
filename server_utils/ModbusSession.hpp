#ifndef MODBUS_SESSION_HPP
#define MODBUS_SESSION_HPP

#include "ModbusRequestHandler.hpp"
#include <boost/asio.hpp>
#include <iostream>

template <typename ModbusDevice>
class ModbusSession : public std::enable_shared_from_this<ModbusSession<ModbusDevice>> {
public:
    inline                          ModbusSession(boost::asio::io_service& io, ModbusDevice& device);
    inline                          ~ModbusSession();

    boost::asio::ip::tcp::socket&   socket();

    template <typename Callback>
    void                            start(Callback done_cb);

    inline void                     stop();

private:
    ModbusRequestHandler<ModbusDevice> m_modbus_handler;
    boost::asio::ip::tcp::socket    m_socket;
    uint8_t                         m_rx_buffer[1024];
    uint8_t                         m_tx_buffer[1024];
    std::function<void(void)>       m_done_cb;

    void                            init_header_reception();
    void                            on_header_received(const boost::system::error_code& ec);
    void                            init_payload_reception();
    void                            on_payload_received(const boost::system::error_code& ec);
    void                            handle_message();
    void                            init_response_sending(std::size_t size);
    void                            on_response_sent(const boost::system::error_code& ec);
};


template <typename ModbusDevice>
ModbusSession<ModbusDevice>::ModbusSession(boost::asio::io_service& io, ModbusDevice& device) :
    m_modbus_handler(device, m_tx_buffer, sizeof(m_tx_buffer)),
    m_socket(io)
{
    std::cout << "new session " << this << std::endl;
}


template <typename ModbusDevice>
ModbusSession<ModbusDevice>::~ModbusSession() {
    std::cout << "end session " << this << std::endl;
}


template <typename ModbusDevice>
boost::asio::ip::tcp::socket& ModbusSession<ModbusDevice>::socket() {
    return m_socket;
}


template <typename ModbusDevice>
template <typename Callback>
void ModbusSession<ModbusDevice>::start(Callback done_cb) {
    m_done_cb = done_cb;
    init_header_reception();
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::init_header_reception() {
    auto self = this->shared_from_this();

    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(m_rx_buffer, sizeof(modbus::tcp::serialized::Header)),
        [self, this](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            on_header_received(ec);
        });
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::on_header_received(const boost::system::error_code& ec) {
    if (ec) {
        m_done_cb();
        m_done_cb = nullptr;
        return;
    }

    init_payload_reception();
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::init_payload_reception() {
    auto* h = reinterpret_cast<modbus::tcp::serialized::Header*>(m_rx_buffer);
    std::size_t payload_size = ntohs(h->length) - 2;

    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(m_rx_buffer + sizeof(modbus::tcp::serialized::Header), payload_size),
        [this](const boost::system::error_code& ec, std::size_t) {
            on_payload_received(ec);
        });
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::on_payload_received(const boost::system::error_code& ec) {
    if (ec) {
        m_done_cb();
        m_done_cb = nullptr;
        return;
    }

    handle_message();
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::handle_message() {
    using namespace modbus::tcp::serialized;

    const auto *h = reinterpret_cast<const Header*>(m_rx_buffer);
    std::size_t rsp_size;

    switch (h->functionCode) {
        case modbus::tcp::READ_COILS: {
            const auto* req = reinterpret_cast<const ReadValuesReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleReadCoils(*req);
            } break;

        case modbus::tcp::READ_DISCRETE_INPUTS: {
            const auto* req = reinterpret_cast<const ReadValuesReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleReadDiscreteInputs(*req);
            } break;

        case modbus::tcp::READ_HOLDING_REGISTERS:{
            const auto* req = reinterpret_cast<const ReadValuesReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleReadHoldingRegisters(*req);
            } break;

        case modbus::tcp::READ_INPUT_REGISTERS: {
            const auto* req = reinterpret_cast<const ReadValuesReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleReadInputRegisters(*req);
            } break;

        case modbus::tcp::WRITE_COIL: {
            const auto *req = reinterpret_cast<const WriteValueReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleWriteCoil(*req);
            } break;

        case modbus::tcp::WRITE_REGISTER: {
            const auto *req = reinterpret_cast<const WriteValueReq*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleWriteRegister(*req);
            } break;

        case modbus::tcp::WRITE_COILS: {
            const auto* req = reinterpret_cast<const SetBitsRequest*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleWriteCoils(*req);
            } break;

        case modbus::tcp::WRITE_REGISTERS: {
            const auto* req = reinterpret_cast<const SetRegsRequest*>(m_rx_buffer);
            rsp_size = m_modbus_handler.handleWriteRegisters(*req);
            } break;

        default:
            m_done_cb();
            m_done_cb = nullptr;
            return;
    }

    init_response_sending(rsp_size);
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::init_response_sending(std::size_t size) {
    auto self = this->shared_from_this();

    boost::asio::async_write(
        m_socket,
        boost::asio::buffer(m_tx_buffer, size),
        [self, this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
            on_response_sent(ec);
        });
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::on_response_sent(const boost::system::error_code& ec) {
    if (ec) {
        if (m_done_cb) {
            m_done_cb();
            m_done_cb = nullptr;
        }
        return;
    }

    init_header_reception();
}


template <typename ModbusDevice>
void ModbusSession<ModbusDevice>::stop() {
    auto self = this->shared_from_this();

    m_socket.get_io_service().post([self, this]() {
        m_socket.close();
    });
}

#endif
