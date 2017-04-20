#ifndef MODBUS_CLIENT_HPP
#define MODBUS_CLIENT_HPP

#include "ModbusEncoder.hpp"
#include <boost/asio.hpp>
#include <stdexcept>

struct ModbusErrorRsp : public std::runtime_error {
    ModbusErrorRsp(modbus::tcp::FunctionCode funcCode) : std::runtime_error("Modbus error " + std::to_string(funcCode)) {}
};


class ModbusClient {
public:
    inline                              ModbusClient(uint16_t unitId);

    inline void                         setTransactionId(uint16_t transactionId);
    inline void                         connect(const std::string& ip, uint16_t port);
    inline void                         readCoils(uint16_t startAddr, uint16_t numCoils, modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp);

private:
    boost::asio::io_service             m_io;
    boost::asio::ip::tcp::socket        m_socket;
    uint16_t                            m_unitId;
    uint16_t                            m_transactionId;
};


ModbusClient::ModbusClient(uint16_t unitId) :
    m_io(),
    m_socket(m_io),
    m_unitId(unitId),
    m_transactionId(1)
{}


void ModbusClient::setTransactionId(uint16_t transactionId) {
    m_transactionId = transactionId;
}


void ModbusClient::connect(const std::string& ip, uint16_t port) {
    m_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
}


void ModbusClient::readCoils(uint16_t startAddr, uint16_t numCoils, modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp) {
    modbus::tcp::encoder::ReadCoilsReq::Buffer req_buf;
    modbus::tcp::encoder::ReadCoilsReq req(req_buf);
    req.setUnitId(m_unitId);
    req.setTransactionId(m_transactionId);
    req.setStartAddress(startAddr);
    req.setNumValues(numCoils);

    boost::asio::write(m_socket, boost::asio::buffer(&req_buf, req.message_size()));

    boost::asio::read(m_socket, boost::asio::buffer(&rsp, sizeof(rsp.header)));
    uint16_t payload_size = ntohs(rsp.header.length) - 2;
    uint8_t *payload = reinterpret_cast<uint8_t*>(&rsp) + sizeof(rsp.header);
    boost::asio::read(m_socket, boost::asio::buffer(payload, payload_size));

    if (rsp.header.functionCode & 0x80) {
        const auto* err_buf = reinterpret_cast<modbus::tcp::encoder::ErrorResponse::Buffer*>(&rsp);
        throw ModbusErrorRsp(static_cast<modbus::tcp::FunctionCode>(err_buf->errCode));
    }
}

#endif
