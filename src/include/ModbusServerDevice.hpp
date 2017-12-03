#ifndef MODBUS_DEVICE_HPP
#define MODBUS_DEVICE_HPP

namespace modbus {
namespace tcp {

struct error : public std::runtime_error {
    error(const std::string& msg) : std::runtime_error(msg) {}
};


struct unit_id_mismatch : public error {
    unit_id_mismatch(UnitId& /*received*/, UnitId& /*myid*/) : error("unit id mismatch") {}
};


class ServerDevice {
public:
                            ServerDevice(const UnitId& unitId);
    virtual                ~ServerDevice();

    void                    handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);

protected:
    virtual bool            getCoil(const Address& address) const = 0;
    virtual bool            getDiscreteInput(const Address& address) const = 0;
    virtual uint16_t        getHoldingRegister(const Address& addr) const = 0;
    virtual uint16_t        getInputRegister(const Address& addr) const = 0;

    virtual void            setCoil(const Address& address, bool value) = 0;
    virtual void            setRegister(const Address& address, uint16_t value) = 0;

private:
    inline void             handleReadCoilsReq              (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadDiscreteInputsReq     (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadHoldingRegistersReq   (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadInputRegistersReq     (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleWriteSingleCoilReq        (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);
    inline void             handleWriteSingleRegisterReq    (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);

    UnitId                  m_unitId;
};


ServerDevice::ServerDevice(const UnitId& unitId) : m_unitId(unitId) {
}


ServerDevice::~ServerDevice() {
}


void ServerDevice::handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Decoder::Header header;

    decoder.decodeHeader(rx_buffer, header);

    if ((m_unitId.get() != 0) && (header.unitId.get() != m_unitId.get()))
        throw unit_id_mismatch(m_unitId, header.unitId);

    switch (header.functionCode) {
        case FunctionCode::READ_COILS:
            handleReadCoilsReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_DISCRETE_INPUTS:
            handleReadDiscreteInputsReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_HOLDING_REGISTERS:
            handleReadHoldingRegistersReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_INPUT_REGISTERS:
            handleReadInputRegistersReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_COIL:
            handleWriteSingleCoilReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_REGISTER:
            handleWriteSingleRegisterReq(header.transactionId, rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_COILS:
            //handleWriteCoilsReq(rx_buffer, tx_buffer);
            throw std::logic_error("Function code not supported");
            break;

        case FunctionCode::WRITE_REGISTERS:
            //handleWriteRegistersReq(rx_buffer, tx_buffer);
            throw std::logic_error("Function code not supported");
            break;

        default:
            throw std::logic_error("Function code not supported");
    }
}


void ServerDevice::handleReadCoilsReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    modbus::tcp::NumBits numCoils;
    decoder.decodeReadCoilsReq(rx_buffer, address, numCoils);

    std::vector<bool> coils;
    coils.reserve(numCoils.get());

    for (std::size_t i = address.get(); i < address.get() + numCoils.get(); ++i)
        coils.push_back(getCoil(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), tx_buffer);
}


void ServerDevice::handleReadDiscreteInputsReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    modbus::tcp::NumBits numInputs;
    decoder.decodeReadDiscreteInputsReq(rx_buffer, address, numInputs);

    std::vector<bool> inputs;
    inputs.reserve(numInputs.get());

    for (std::size_t i = address.get(); i < address.get() + numInputs.get(); ++i)
        inputs.push_back(getDiscreteInput(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadDiscreteInputsRsp(inputs.begin(), inputs.end(), tx_buffer);
}


void ServerDevice::handleReadHoldingRegistersReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    modbus::tcp::NumRegs numRegs;
    decoder.decodeReadHoldingRegistersReq(rx_buffer, address, numRegs);

    std::vector<uint16_t> regs;
    regs.reserve(numRegs.get());

    for (std::size_t i = address.get(); i < address.get() + numRegs.get(); ++i)
        regs.push_back(getHoldingRegister(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadHoldingRegistersRsp(regs.begin(), regs.end(), tx_buffer);
}


void ServerDevice::handleReadInputRegistersReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    modbus::tcp::NumRegs numRegs;
    decoder.decodeReadHoldingRegistersReq(rx_buffer, address, numRegs);

    std::vector<uint16_t> regs;
    regs.reserve(numRegs.get());

    for (std::size_t i = address.get(); i < address.get() + numRegs.get(); ++i)
        regs.push_back(getInputRegister(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadInputRegistersRsp(regs.begin(), regs.end(), tx_buffer);
}


void ServerDevice::handleWriteSingleCoilReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    bool value = false;
    decoder.decodeWriteSingleCoilReq(rx_buffer, address, value);

    setCoil(address, value);

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteSingleCoilRsp(address, value, tx_buffer);
}


void ServerDevice::handleWriteSingleRegisterReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Address address;
    uint16_t value = 0;
    decoder.decodeWriteSingleRegisterReq(rx_buffer, address, value);

    setRegister(address, value);

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteSingleRegisterRsp(address, value, tx_buffer);
}


} // namespace tcp
} // namespace modbus
#endif
