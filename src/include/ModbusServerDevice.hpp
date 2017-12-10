#ifndef MODBUS_DEVICE_HPP
#define MODBUS_DEVICE_HPP

namespace modbus {
namespace tcp {

struct UnitIdMismatch : public error {
    UnitIdMismatch(const UnitId& /*received*/, const UnitId& /*myid*/) : error("unit id mismatch") {}
};

struct FunctionCodeNotSupported : public error { // exception code 1
    FunctionCodeNotSupported(FunctionCode code) : error("Function code not supported") {}
};

struct BadAddress : public error {  // exception code 2
    BadAddress(const std::string& msg = "") : error(msg) {}
};

struct FailedToReadInputs : public error { // exception code 4
    FailedToReadInputs(const std::string& msg = "") : error(msg) {}
};


class ServerDevice {
public:
                            ServerDevice(const UnitId& unitId);
    virtual                ~ServerDevice();

    void                    handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);

protected:
    virtual bool            getCoil(const Address& address) const;
    virtual bool            getDiscreteInput(const Address& address) const;
    virtual uint16_t        getHoldingRegister(const Address& addr) const;
    virtual uint16_t        getInputRegister(const Address& addr) const;

    virtual void            setCoil(const Address& address, bool value);
    virtual void            setRegister(const Address& address, uint16_t value);

    virtual void            setCoils(const Address& startAddress, const std::vector<bool>& coils);
    virtual void            setRegisters(const Address& startAddress, const std::vector<uint16_t>& regs);

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
        throw UnitIdMismatch(m_unitId, header.getUnitId());

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
    modbus::tcp::Encoder encoder(m_unitId, transactionId);

    try {
        coils.reserve(view.getNumBits().get());

        const std::size_t start = view.getStartAddress().get();
        const std::size_t end = start + view.getNumBits().get();

        for (std::size_t i = start; i < end; ++i)
            coils.push_back(getCoil(modbus::tcp::Address(i)));

        encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), tx_buffer);
    } catch (const FunctionCodeNotSupported& ex) {
        encoder.encodeErrorRsp(FunctionCode::READ_COILS, ExceptionCode::ILLEGAL_FUNCTION, tx_buffer);
    } catch (const NumBitsOutOfRange& ex) {
        encoder.encodeErrorRsp(FunctionCode::READ_COILS, ExceptionCode::ILLEGAL_DATA_VALUE, tx_buffer);
    } catch (const BadAddress& ex) {
        encoder.encodeErrorRsp(FunctionCode::READ_COILS, ExceptionCode::ILLEGAL_DATA_ADDRESS, tx_buffer);
    } catch (const FailedToReadInputs& ex) {
        encoder.encodeErrorRsp(FunctionCode::READ_COILS, ExceptionCode::SLAVE_DEVICE_FAILURE, tx_buffer);
    }
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
    modbus::tcp::decoder_views::WriteRegistersReq view(rx_buffer);

    std::vector<uint16_t> regs;
    for (std::size_t i = 0; i < view.getNumRegs().get(); ++i)
        regs.push_back(view.getRegister(i));

    setRegisters(view.getStartAddress(), regs);

    modbus::tcp::Encoder encoder(m_unitId, transactionId);
    encoder.encodeWriteRegistersRsp(view.getStartAddress(), view.getNumRegs(), tx_buffer);
}


bool ServerDevice::getCoil(const Address& address) const {
    throw FunctionCodeNotSupported(FunctionCode::READ_COILS);
}


bool ServerDevice::getDiscreteInput(const Address& address) const {
    throw FunctionCodeNotSupported(FunctionCode::READ_DISCRETE_INPUTS);
}


uint16_t ServerDevice::getHoldingRegister(const Address& addr) const {
    throw FunctionCodeNotSupported(FunctionCode::READ_HOLDING_REGISTERS);
}


uint16_t ServerDevice::getInputRegister(const Address& addr) const {
    throw FunctionCodeNotSupported(FunctionCode::READ_INPUT_REGISTERS);
}


void ServerDevice::setCoil(const Address& address, bool value) {
    throw FunctionCodeNotSupported(FunctionCode::WRITE_COIL);
}


void ServerDevice::setRegister(const Address& address, uint16_t value) {
    throw FunctionCodeNotSupported(FunctionCode::WRITE_REGISTER);
}



void ServerDevice::setCoils(const Address& startAddress, const std::vector<bool>& coils) {
    throw FunctionCodeNotSupported(FunctionCode::WRITE_COILS);
}


void ServerDevice::setRegisters(const Address& startAddress, const std::vector<uint16_t>& regs) {
    throw FunctionCodeNotSupported(FunctionCode::WRITE_REGISTERS);
}

} // namespace tcp
} // namespace modbus
#endif
