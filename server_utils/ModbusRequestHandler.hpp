#ifndef MODBUS_REQUEST_HANDLER_HPP
#define MODBUS_REQUEST_HANDLER_HPP

#include "ModbusConsts.hpp"
#include "ModbusEncoder.hpp"
#include <stdexcept>

struct IllegalFunction : public std::runtime_error {
    IllegalFunction(modbus::tcp::FunctionCode funcCode) : std::runtime_error("Function code not supported " + std::to_string(funcCode)) {}
};

struct IllegalDataAddress : public std::runtime_error {
    IllegalDataAddress(const std::string& msg) : std::runtime_error(msg) {}
};

struct SlaveDeviceFailure : public std::runtime_error {
    SlaveDeviceFailure(const std::string& msg) : std::runtime_error(msg) {}
};


template <typename ModbusDevice>
class ModbusRequestHandler {
public:
                                    ModbusRequestHandler(ModbusDevice& backend, uint8_t* tx_buffer, std::size_t capacity);
    std::size_t                     handleReadCoils(const modbus::tcp::serialized::ReadValuesReq& req);
    std::size_t                     handleReadDiscreteInputs(const modbus::tcp::serialized::ReadValuesReq& req);
    std::size_t                     handleReadHoldingRegisters(const modbus::tcp::serialized::ReadValuesReq& req);
    std::size_t                     handleReadInputRegisters(const modbus::tcp::serialized::ReadValuesReq& req);
    std::size_t                     handleWriteCoil(const modbus::tcp::serialized::WriteValueReq& req);
    std::size_t                     handleWriteRegister(const modbus::tcp::serialized::WriteValueReq& req);
    std::size_t                     handleWriteCoils(const modbus::tcp::serialized::SetBitsRequest& req);
    std::size_t                     handleWriteRegisters(const modbus::tcp::serialized::SetRegsRequest& req);

private:
    ModbusDevice                   &m_backend;
    uint8_t                        *m_tx_buffer;
    std::size_t                     m_tx_buffer_capacity;

    template <typename RspType, std::uint16_t MaxNumValues, modbus::tcp::FunctionCode funcCode, typename Callback>
    std::size_t                     handle_read_values_req(const modbus::tcp::serialized::ReadValuesReq& req, Callback cb);

    template <typename Request>
    std::size_t                     createFailureMessage(const Request& req, modbus::tcp::FunctionCode funcCode, modbus::tcp::ExceptionCode ex);
};


template <typename ModbusDevice>
ModbusRequestHandler<ModbusDevice>::ModbusRequestHandler(ModbusDevice& backend, uint8_t* tx_buffer, std::size_t capacity) :
    m_backend(backend),
    m_tx_buffer(tx_buffer),
    m_tx_buffer_capacity(capacity)
{
}

template <typename ModbusDevice>
template <typename Request>
std::size_t ModbusRequestHandler<ModbusDevice>::createFailureMessage(const Request& req, modbus::tcp::FunctionCode funcCode, modbus::tcp::ExceptionCode ex) {
    auto buf = new (m_tx_buffer)modbus::tcp::encoder::ErrorResponse::Buffer();
    modbus::tcp::encoder::ErrorResponse rsp(*buf, funcCode);

    rsp.setUnitId(req.header.unitId);
    rsp.setTransactionId(htons(req.header.transactionId));
    rsp.setExceptionCode(ex);

    return rsp.message_size();
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleReadCoils(const modbus::tcp::serialized::ReadValuesReq& req)
{
    using namespace modbus::tcp;

    return handle_read_values_req<encoder::ReadCoilsRsp, MODBUS_MAX_NUM_BITS_IN_READ_REQUEST, READ_COILS>(req, [this](encoder::ReadCoilsRsp& rsp, uint16_t startAddr, uint16_t numValues) {
        m_backend.getCoils(startAddr, numValues, [&rsp](uint16_t pos, bool value) {
            rsp.setValue(pos, value);
        });
    });
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleReadDiscreteInputs(const modbus::tcp::serialized::ReadValuesReq& req) {
    using namespace modbus::tcp;
    return handle_read_values_req<encoder::ReadDiscreteInputsRsp, MODBUS_MAX_NUM_BITS_IN_READ_REQUEST, READ_DISCRETE_INPUTS>(req, [this](encoder::ReadDiscreteInputsRsp& rsp, uint16_t startAddr, uint16_t numValues) {
        m_backend.getDiscreteInputs(startAddr, numValues, [&rsp](uint16_t pos, bool value) {
            rsp.setValue(pos, value);
        });
    });
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleReadHoldingRegisters(const modbus::tcp::serialized::ReadValuesReq& req) {
    using namespace modbus::tcp;
    return handle_read_values_req<encoder::ReadHoldingRegsRsp, MODBUS_MAX_NUM_REGS_IN_READ_REQUEST, READ_HOLDING_REGISTERS>(req, [this](encoder::ReadHoldingRegsRsp& rsp, uint16_t startAddr, uint16_t numValues) {
        m_backend.getHoldingRegs(startAddr, numValues, [&rsp](uint16_t pos, uint16_t value) {
            rsp.setValue(pos, value);
        });
    });
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleReadInputRegisters(const modbus::tcp::serialized::ReadValuesReq& req) {
    using namespace modbus::tcp;
    return handle_read_values_req<encoder::ReadInputRegsRsp, MODBUS_MAX_NUM_REGS_IN_READ_REQUEST, READ_INPUT_REGISTERS>(req, [this](encoder::ReadInputRegsRsp& rsp, uint16_t startAddr, uint16_t numValues) {
        m_backend.getInputRegs(startAddr, numValues, [&rsp](uint16_t pos, uint16_t value) {
            rsp.setValue(pos, value);
        });
    });
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleWriteCoil(const modbus::tcp::serialized::WriteValueReq& req) {
    using namespace modbus::tcp;

    uint16_t address = htons(req.address);
    uint16_t value = htons(req.value);

    if ((value != 0xFF00) && (value != 0)) {
        return createFailureMessage(req, WRITE_COIL, ILLEGAL_DATA_VALUE);
    }

    auto buf = new(m_tx_buffer) encoder::WriteCoilResponse::Buffer();
    encoder::WriteCoilResponse rsp(*buf);

    try {
        m_backend.writeCoil(address, value == 0xFF00 ? true : false);
        rsp.setAddress(address);
        rsp.setValue(value);
        return rsp.message_size();
    } catch (const IllegalFunction& ex) {
        return createFailureMessage(req, WRITE_COIL, ILLEGAL_FUNCTION);
    } catch (const IllegalDataAddress& ex) {
        return createFailureMessage(req, WRITE_COIL, ILLEGAL_DATA_ADDRESS);
    } catch (const SlaveDeviceFailure& ex) {
        return createFailureMessage(req, WRITE_COIL, SLAVE_DEVICE_FAILURE);
    }
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleWriteRegister(const modbus::tcp::serialized::WriteValueReq& req) {
    using namespace modbus::tcp;

    uint16_t address = htons(req.address);
    uint16_t value = htons(req.value);

    auto buf = new(m_tx_buffer) encoder::WriteRegisterResponse::Buffer();
    encoder::WriteRegisterResponse rsp(*buf);

    try {
        m_backend.writeRegister(address, value);
        rsp.setAddress(address);
        rsp.setValue(value);
        return rsp.message_size();
    } catch (const IllegalFunction& ex) {
        return createFailureMessage(req, WRITE_COIL, ILLEGAL_FUNCTION);
    } catch (const IllegalDataAddress& ex) {
        return createFailureMessage(req, WRITE_COIL, ILLEGAL_DATA_ADDRESS);
    } catch (const SlaveDeviceFailure& ex) {
        return createFailureMessage(req, WRITE_COIL, SLAVE_DEVICE_FAILURE);
    }
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleWriteCoils(const modbus::tcp::serialized::SetBitsRequest& req) {
    return 0;
}


template <typename ModbusDevice>
std::size_t ModbusRequestHandler<ModbusDevice>::handleWriteRegisters(const modbus::tcp::serialized::SetRegsRequest& req) {
    return 0;
}


template <typename ModbusDevice>
template <typename RspType, std::uint16_t MaxNumValues, modbus::tcp::FunctionCode funcCode, typename Callback>
std::size_t ModbusRequestHandler<ModbusDevice>::handle_read_values_req(const modbus::tcp::serialized::ReadValuesReq& req, Callback cb) {
    using namespace modbus::tcp;

    uint16_t startAddr = htons(req.startAddr);
    uint16_t numValues = htons(req.numValues);

    if (numValues > MaxNumValues)
        return createFailureMessage(req, READ_COILS, ILLEGAL_DATA_VALUE);

    typedef typename RspType::Buffer Buffer;
    Buffer *buf = new (m_tx_buffer)Buffer();
    RspType rsp(*buf, numValues);

    try {
        cb(rsp, startAddr, numValues);
        rsp.setUnitId(req.header.unitId);
        rsp.setTransactionId(ntohs(req.header.transactionId));
        return rsp.message_size();
    } catch (const IllegalFunction& ex) {
        return createFailureMessage(req, READ_COILS, ILLEGAL_FUNCTION);
    } catch (const IllegalDataAddress& ex) {
        return createFailureMessage(req, READ_COILS, ILLEGAL_DATA_ADDRESS);
    } catch (const SlaveDeviceFailure& ex) {
        return createFailureMessage(req, READ_COILS, SLAVE_DEVICE_FAILURE);
    }
}

#endif

