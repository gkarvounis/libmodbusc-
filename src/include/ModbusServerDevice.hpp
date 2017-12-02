#ifndef MODBUS_DEVICE_HPP
#define MODBUS_DEVICE_HPP

namespace modbus {
namespace tcp {

class ModbusDevice {
public:
                            ModbusDevice();
    virtual                ~ModbusDevice();

    void                    handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer);

protected:

    virtual bool            getCoil(const Address& address) const = 0;
    virtual bool            getInput(const Address& address) const = 0;
    virtual uint16_t        getHoldingRegister(const Address& addr) const = 0;
    virtual uint16_t        getInputRegister(const Address& addr) const = 0;

    virtual void            setCoil(const Address& address, bool value) = 0;
    virtual void            setRegister(const Address& address, uint16_t value) = 0;

private:
    inline void             handleReadCoilsReq              (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadDiscreteInputsReq     (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadHoldingRegistersReq   (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleReadInputRegistersReq     (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleWriteSingleCoilReq        (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
    inline void             handleWriteSingleRegisterReq    (const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const;
};


ModbusDevice::ModbusDevice() {
}


ModbusDevice::~ModbusDevice() {
}


void ModbusDevice::handleMessage(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) {
    modbus::tcp::Decoder decoder;
    modbus::tcp::Decoder::Header header;

    decoder.decodeHeader(rx_buffer, header);

    switch (header.functionCode) {
        case FunctionCode::READ_COILS:
            handleReadCoilsReq(rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_DISCRETE_INPUTS:
            handleReadDiscreteInputsReq(rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_HOLDING_REGISTERS:
            handleReadHoldingRegistersReq(rx_buffer, tx_buffer);
            break;

        case FunctionCode::READ_INPUT_REGISTERS:
            handleReadInputRegistersReq(rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_COIL:
            handleWriteSingleCoilReq(rx_buffer, tx_buffer);
            break;

        case FunctionCode::WRITE_REGISTER:
            handleWriteSingleRegisterReq(rx_buffer, tx_buffer);
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


void ModbusDevice::handleReadCoilsReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


void ModbusDevice::handleReadDiscreteInputsReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


void ModbusDevice::handleReadHoldingRegistersReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


void ModbusDevice::handleReadInputRegistersReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


void ModbusDevice::handleWriteSingleCoilReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


void ModbusDevice::handleWriteSingleRegisterReq(const std::vector<uint8_t>& rx_buffer, std::vector<uint8_t>& tx_buffer) const {
}


} // namespace tcp
} // namespace modbus
#endif
