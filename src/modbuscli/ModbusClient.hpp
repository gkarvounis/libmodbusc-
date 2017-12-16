#ifndef MODBUS_CLIENT_HPP
#define MODBUS_CLIENT_HPP

#include <vector>
#include <string>
#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include <boost/asio.hpp>
#include <stdexcept>


class ModbusClient {
public:
    inline                              ModbusClient(const modbus::tcp::UnitId& unitId);

    inline void                         setTransactionId(const modbus::tcp::TransactionId& transactionId);
    inline void                         connect(const boost::asio::ip::tcp::endpoint& ep);
    inline void                         readCoils(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numCoils);

private:
    boost::asio::io_service             m_io;
    boost::asio::ip::tcp::socket        m_socket;
    modbus::tcp::UnitId                 m_unitId;
    modbus::tcp::TransactionId          m_transactionId;
    std::vector<uint8_t>                m_tx_buffer;
    std::vector<uint8_t>                m_rx_buffer;

    void                                recvResponse();
    void                                printReadCoilsResult(uint16_t startAddress, uint16_t numCoils);
};


ModbusClient::ModbusClient(const modbus::tcp::UnitId& unitId) :
    m_io(),
    m_socket(m_io),
    m_unitId(unitId),
    m_transactionId(1),
    m_tx_buffer(),
    m_rx_buffer()
{}


void ModbusClient::setTransactionId(const modbus::tcp::TransactionId& transactionId) {
    m_transactionId = transactionId;
}


void ModbusClient::connect(const boost::asio::ip::tcp::endpoint& ep) {
    m_socket.connect(ep);
}

#include <iostream>

void ModbusClient::readCoils(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numCoils) {
    modbus::tcp::Encoder encoder(m_unitId, m_transactionId);

    encoder.encodeReadCoilsReq(startAddress, numCoils, m_tx_buffer);
    boost::asio::write(m_socket, boost::asio::buffer(m_tx_buffer));

    recvResponse();
    printReadCoilsResult(startAddress.get(), numCoils.get());
}


void ModbusClient::recvResponse() {
    m_rx_buffer.resize(sizeof(modbus::tcp::Header));
    boost::asio::read(m_socket, boost::asio::buffer(m_rx_buffer));

    modbus::tcp::decoder_views::Header rsp_header_view(m_rx_buffer);
    uint16_t payload_size = rsp_header_view.getLength() - 2;
    m_rx_buffer.resize(sizeof(modbus::tcp::Header) + payload_size);
    uint8_t *payload = m_rx_buffer.data() + sizeof(modbus::tcp::Header);

    boost::asio::read(m_socket, boost::asio::buffer(payload, payload_size));
}


void ModbusClient::printReadCoilsResult(uint16_t startAddress, uint16_t numCoils) {
    for (auto c: m_rx_buffer)
        std::cout << std::hex << (int)c << ' ';

    std::cout << std::endl;

    modbus::tcp::decoder_views::Header rsp_header_view(m_rx_buffer);
    std::cout << "unitId:" << static_cast<unsigned>(rsp_header_view.getUnitId().get()) << std::endl
              << "transactionId:" << rsp_header_view.getTransactionId().get() << std::endl;

    if (rsp_header_view.isError()) {
        modbus::tcp::decoder_views::ErrorResponse rsp(m_rx_buffer);
        std::cout << "Device responded with error " << static_cast<unsigned>(rsp.getCode()) << std::endl;
        return;
    }

    modbus::tcp::decoder_views::ReadCoilsRsp rsp(m_rx_buffer);

    std::size_t numRows = numCoils / 8;
    if (numCoils % 8 != 0)
        numRows++;

    for (std::size_t row = 0; row < numRows; ++row) {
        uint16_t fromCoil = startAddress + row * 8;
        uint16_t toCoil = std::min(fromCoil + 7, numCoils + startAddress - 1);

        std::cout << "coils " << fromCoil << " to " << toCoil << ": ";

        for (unsigned i = fromCoil; i <= toCoil; ++i) {
            std::cout << rsp.getBit(i) << ' ';
        }

        std::cout << std::endl;
    }
}

#endif
