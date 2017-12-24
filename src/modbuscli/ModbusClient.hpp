#ifndef MODBUS_CLIENT_HPP
#define MODBUS_CLIENT_HPP

#include <boost/asio.hpp>
#include <stdexcept>


class ModbusClient {
public:
    inline                              ModbusClient(const modbus::tcp::UnitId& unitId, std::unique_ptr<OutputFormatter> out);

    inline void                         setTransactionId(const modbus::tcp::TransactionId& transactionId);
    inline void                         connect(const boost::asio::ip::tcp::endpoint& ep);
    inline void                         readCoils(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numCoils);
    inline void                         readDiscreteInputs(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numInputs);
    inline void                         readInputRegisters(const modbus::tcp::Address& startAddress, const modbus::tcp::NumRegs& numRegs);
    inline void                         readHoldingRegisters(const modbus::tcp::Address& startAddress, const modbus::tcp::NumRegs& numRegs);

    inline void                         setFormatter(std::unique_ptr<OutputFormatter> out);

private:
    std::unique_ptr<OutputFormatter>    m_outFormatter;
    boost::asio::io_service             m_io;
    boost::asio::ip::tcp::socket        m_socket;
    modbus::tcp::UnitId                 m_unitId;
    modbus::tcp::TransactionId          m_transactionId;
    std::vector<uint8_t>                m_tx_buffer;
    std::vector<uint8_t>                m_rx_buffer;

    void                                readValues(std::function<void(modbus::tcp::Encoder&)> encodeFn, std::function<void(void)> displayFn);

    void                                recvResponse();
};


ModbusClient::ModbusClient(const modbus::tcp::UnitId& unitId, std::unique_ptr<OutputFormatter> out) :
    m_outFormatter(std::move(out)),
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


void ModbusClient::setFormatter(std::unique_ptr<OutputFormatter> out) {
    m_outFormatter = std::move(out);
}


void ModbusClient::connect(const boost::asio::ip::tcp::endpoint& ep) {
    m_socket.connect(ep);
}


void ModbusClient::readCoils(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numCoils) {
    readValues([this, &startAddress, numCoils](modbus::tcp::Encoder& encoder) {
        encoder.encodeReadCoilsReq(startAddress, numCoils, m_tx_buffer);
    },
    [this]() {
        m_outFormatter->displayReadCoils(m_tx_buffer, m_rx_buffer);
    });
}


void ModbusClient::readDiscreteInputs(const modbus::tcp::Address& startAddress, const modbus::tcp::NumBits& numInputs) {
    readValues([this, &startAddress, numInputs](modbus::tcp::Encoder& encoder) {
        encoder.encodeReadDiscreteInputsReq(startAddress, numInputs, m_tx_buffer);
    },
    [this]() {
        m_outFormatter->displayReadDiscreteInputs(m_tx_buffer, m_rx_buffer);
    });
}


void ModbusClient::readInputRegisters(const modbus::tcp::Address& startAddress, const modbus::tcp::NumRegs& numRegs) {
    readValues([this, &startAddress, numRegs](modbus::tcp::Encoder& encoder) {
        encoder.encodeReadInputRegistersReq(startAddress, numRegs, m_tx_buffer);
    },
    [this]() {
        m_outFormatter->displayReadInputRegisters(m_tx_buffer, m_rx_buffer);
    });
}


void ModbusClient::readHoldingRegisters(const modbus::tcp::Address& startAddress, const modbus::tcp::NumRegs& numRegs) {
    readValues([this, &startAddress, &numRegs](modbus::tcp::Encoder& encoder) {
        encoder.encodeReadHoldingRegistersReq(startAddress, numRegs, m_tx_buffer);
    },
    [this]() {
        m_outFormatter->displayReadHoldingRegisters(m_tx_buffer, m_rx_buffer);
    });
}


void ModbusClient::readValues(std::function<void(modbus::tcp::Encoder&)> encodeFn, std::function<void(void)> displayFn) {
    modbus::tcp::Encoder encoder(m_unitId, m_transactionId);

    encodeFn(encoder);
    m_outFormatter->displayOutgoing(m_tx_buffer);
    boost::asio::write(m_socket, boost::asio::buffer(m_tx_buffer));

    recvResponse();

    modbus::tcp::decoder_views::Header rsp_header_view(m_rx_buffer);

    if (rsp_header_view.isError())
        m_outFormatter->displayErrorResponse(m_tx_buffer, m_rx_buffer);
    else
        displayFn();
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

#endif
