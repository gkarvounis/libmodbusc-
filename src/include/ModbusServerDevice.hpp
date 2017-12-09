#ifndef MODBUS_DEVICE_HPP
#define MODBUS_DEVICE_HPP

namespace modbus {
namespace tcp {

struct error : public std::runtime_error {
    error(const std::string& msg) : std::runtime_error(msg) {}
};


struct unit_id_mismatch : public error {
    unit_id_mismatch(const UnitId& /*received*/, const UnitId& /*myid*/) : error("unit id mismatch") {}
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

    virtual void            setCoils(const Address& startAddress, const std::vector<bool>& coils) = 0;
    virtual void            setRegisters(const Address& startAddress, const std::vector<uint16_t>& regs) = 0;

private:
    inline void             handleReadCoilsReq              (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadDiscreteInputsReq     (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadHoldingRegistersReq   (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadInputRegistersReq     (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleWriteSingleCoilReq        (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);
    inline void             handleWriteSingleRegisterReq    (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);
    inline void             handleWriteCoilsReq             (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);
    inline void             handleWriteRegistersReq         (const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);

    UnitId                  m_unitId;
};


ServerDevice::ServerDevice(const UnitId& unitId) : m_unitId(unitId) {
}


ServerDevice::~ServerDevice() {
}


void ServerDevice::handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::decoder_views::Header header(rx_buffer);

    if ((m_unitId.get() != 0) && (header.getUnitId() != m_unitId))
        throw unit_id_mismatch(m_unitId, header.getUnitId());

    switch (header.getFunctionCode()) {
        case FunctionCode::READ_COILS:
            handleReadCoilsReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_DISCRETE_INPUTS:
            handleReadDiscreteInputsReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_HOLDING_REGISTERS:
            handleReadHoldingRegistersReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_INPUT_REGISTERS:
            handleReadInputRegistersReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_COIL:
            handleWriteSingleCoilReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_REGISTER:
            handleWriteSingleRegisterReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_COILS:
            handleWriteCoilsReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_REGISTERS:
            handleWriteRegistersReq(header.getTransactionId(), rx_buffer, tx_buffer);
            break;

        default:
            throw std::logic_error("Function code not supported");
    }
}


void ServerDevice::handleReadCoilsReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::decoder_views::ReadCoilsReq view(rx_buffer);

    std::vector<bool> coils;
    coils.reserve(view.getNumBits().get());

    const std::size_t start = view.getStartAddress().get();
    const std::size_t end = start + view.getNumBits().get();

    for (std::size_t i = start; i < end; ++i)
        coils.push_back(getCoil(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), tx_buffer);
}


void ServerDevice::handleReadDiscreteInputsReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::decoder_views::ReadDiscreteInputsReq view(rx_buffer);;

    std::vector<bool> inputs;
    inputs.reserve(view.getNumBits().get());

    const std::size_t start = view.getStartAddress().get();
    const std::size_t end = start + view.getNumBits().get();

    for (std::size_t i = start; i < end; ++i)
        inputs.push_back(getDiscreteInput(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadDiscreteInputsRsp(inputs.begin(), inputs.end(), tx_buffer);
}


void ServerDevice::handleReadHoldingRegistersReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::decoder_views::ReadHoldingRegistersReq view(rx_buffer);

    std::vector<uint16_t> regs;
    regs.reserve(view.getNumRegs().get());

    const std::size_t start = view.getStartAddress().get();
    const std::size_t end = start + view.getNumRegs().get();

    for (std::size_t i = start; i < end; ++i)
        regs.push_back(getHoldingRegister(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadHoldingRegistersRsp(regs.begin(), regs.end(), tx_buffer);
}


void ServerDevice::handleReadInputRegistersReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
    modbus::tcp::decoder_views::ReadInputRegistersReq view(rx_buffer);

    std::vector<uint16_t> regs;
    regs.reserve(view.getNumRegs().get());

    const std::size_t start = view.getStartAddress().get();
    const std::size_t end = start + view.getNumRegs().get();

    for (std::size_t i = start; i < end; ++i)
        regs.push_back(getInputRegister(modbus::tcp::Address(i)));

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeReadInputRegistersRsp(regs.begin(), regs.end(), tx_buffer);
}


void ServerDevice::handleWriteSingleCoilReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::decoder_views::WriteSingleCoilReq view(rx_buffer);

    setCoil(view.getAddress(), view.getValue());

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteSingleCoilRsp(view.getAddress(), view.getValue(), tx_buffer);
}


void ServerDevice::handleWriteSingleRegisterReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::decoder_views::WriteSingleRegisterReq view(rx_buffer);

    setRegister(view.getAddress(), view.getValue());

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteSingleRegisterRsp(view.getAddress(), view.getValue(), tx_buffer);
}


void ServerDevice::handleWriteCoilsReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::decoder_views::WriteCoilsReq view(rx_buffer);

    std::vector<bool> coils;
    for (std::size_t i = 0; i < view.getNumBits().get(); ++i)
        coils.push_back(view.getCoil(i));

    setCoils(view.getStartAddress(), coils);

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteCoilsRsp(view.getStartAddress(), view.getNumBits(), tx_buffer);
}


void ServerDevice::handleWriteRegistersReq(const TransactionId& transactionId, const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {

}


} // namespace tcp
} // namespace modbus
#endif
