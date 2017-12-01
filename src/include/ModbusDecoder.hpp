#ifndef MODBUS_DECODER_HPP
#define MODBUS_DECODER_HPP


namespace modbus {
namespace tcp {

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include <arpa/inet.h>

class Decoder {
public:
    struct Header {
        TransactionId           transactionId;
        std::size_t             payloadSize;
        UnitId                  unitId;
        FunctionCode            functionCode;
        bool                    isError;
    };

    void                        decodeHeader(const std::vector<uint8_t>& rx_buffer, Header& header) const;

    void                        decodeReadCoilsReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumBits& numCoils) const;
    void                        decodeReadDiscreteInputsReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumBits& numCoils) const;
    void                        decodeReadHoldingRegistersReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumRegs& numRegs) const;
    void                        decodeReadInputRegistersReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumRegs& numRegs) const;
    void                        decodeWriteSingleCoilReq(const std::vector<uint8_t>& rx_buffer, Address& address, bool& value) const;
    void                        decodeWriteSingleRegisterReq(const std::vector<uint8_t>& rx_buffer, Address& address, uint16_t& value) const;

    void                        decodeReadCoilsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const;
    void                        decodeReadDiscreteInputsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const;
    void                        decodeReadHoldingRegistersRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const;
    void                        decodeReadInputRegistersRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const;
    void                        decodeWriteSingleCoilRsp(const std::vector<uint8_t>& rx_buffer, Address& address, bool& value) const;
    void                        decodeWriteSingleRegisterRsp(const std::vector<uint8_t>& rx_buffer, Address& address, uint16_t& value) const;

    void                        decodeErrorResponse(const std::vector<uint8_t>& rx_buffer, ExceptionCode& code) const;

private:
    void                        decodeReadBitsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const;
    void                        decodeReadRegsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const;
};


void Decoder::decodeHeader(const std::vector<uint8_t>& rx_buffer, Decoder::Header& header) const {
    const auto* h = reinterpret_cast<const modbus::tcp::Header*>(rx_buffer.data());

    header.transactionId.set(ntohs(h->transactionId));
    header.payloadSize = ntohs(h->length);
    header.unitId.set(h->unitId);

    switch (h->functionCode & 0x7F) {
        case static_cast<uint8_t>(FunctionCode::READ_COILS):
        case static_cast<uint8_t>(FunctionCode::READ_DISCRETE_INPUTS):
        case static_cast<uint8_t>(FunctionCode::READ_HOLDING_REGISTERS):
        case static_cast<uint8_t>(FunctionCode::READ_INPUT_REGISTERS):
        case static_cast<uint8_t>(FunctionCode::WRITE_COIL):
        case static_cast<uint8_t>(FunctionCode::WRITE_REGISTER):
        case static_cast<uint8_t>(FunctionCode::WRITE_COILS):
        case static_cast<uint8_t>(FunctionCode::WRITE_REGISTERS):
            header.functionCode = static_cast<FunctionCode>(h->functionCode & 0x7F);
            break;

        default:
            throw std::runtime_error("Invalid function code received");
    }

    header.isError = h->functionCode & 0x80;
}


void Decoder::decodeReadCoilsReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumBits& numCoils) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    startAddress.set(ntohs(msg->startAddress));
    numCoils.set(ntohs(msg->numEntries));
}


void Decoder::decodeReadDiscreteInputsReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumBits& numCoils) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    startAddress.set(ntohs(msg->startAddress));
    numCoils.set(ntohs(msg->numEntries));
}


void Decoder::decodeReadHoldingRegistersReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumRegs& numRegs) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    startAddress.set(ntohs(msg->startAddress));
    numRegs.set(ntohs(msg->numEntries));
}


void Decoder::decodeReadInputRegistersReq(const std::vector<uint8_t>& rx_buffer, Address& startAddress, NumRegs& numRegs) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    startAddress.set(ntohs(msg->startAddress));
    numRegs.set(ntohs(msg->numEntries));
}


void Decoder::decodeWriteSingleCoilReq(const std::vector<uint8_t>& rx_buffer, Address& address, bool& value) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::WriteSingleValue*>(rx_buffer.data());
    address.set(ntohs(msg->address));
    value = msg->value;
}


void Decoder::decodeWriteSingleRegisterReq(const std::vector<uint8_t>& rx_buffer, Address& address, uint16_t& value) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::WriteSingleValue*>(rx_buffer.data());
    address.set(ntohs(msg->address));
    value = ntohs(msg->value);
}


void Decoder::decodeReadCoilsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const {
    decodeReadBitsRsp(rx_buffer, coils);
}


void Decoder::decodeReadDiscreteInputsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const {
    decodeReadBitsRsp(rx_buffer, coils);
}


void Decoder::decodeReadBitsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<bool>& coils) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadCoilsRsp*>(rx_buffer.data());

    coils.resize(msg->numBytes*8);
    coils.clear();

    for (std::size_t i = 0; i < msg->numBytes; ++i)
        for (std::size_t j = 0; j < 8; ++j)
            coils.push_back(msg->coils[i] & (1 << j));
}


void Decoder::decodeReadHoldingRegistersRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const {
    decodeReadRegsRsp(rx_buffer, regs);
}


void Decoder::decodeReadInputRegistersRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const {
    decodeReadRegsRsp(rx_buffer, regs);
}


void Decoder::decodeReadRegsRsp(const std::vector<uint8_t>& rx_buffer, std::vector<uint16_t>& regs) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadRegsRsp*>(rx_buffer.data());

    regs.resize(msg->numBytes/2);
    regs.clear();

    for (std::size_t i = 0; i < msg->numBytes/2; ++i)
        regs.push_back(ntohs(msg->regs[i]));
}


void Decoder::decodeWriteSingleCoilRsp(const std::vector<uint8_t>& rx_buffer, Address& address, bool& value) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::WriteSingleValue*>(rx_buffer.data());
    address.set(ntohs(msg->address));
    value = msg->value;
}


void Decoder::decodeWriteSingleRegisterRsp(const std::vector<uint8_t>& rx_buffer, Address& address, uint16_t& value) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::WriteSingleValue*>(rx_buffer.data());
    address.set(ntohs(msg->address));
    value = ntohs(msg->value);
}


void Decoder::decodeErrorResponse(const std::vector<uint8_t>& rx_buffer, ExceptionCode& code) const {
    const auto* msg = reinterpret_cast<const modbus::tcp::ExceptionRsp*>(rx_buffer.data());

    switch (msg->code) {
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_FUNCTION):
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_DATA_ADDRESS):
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_DATA_VALUE):
        case static_cast<uint8_t>(ExceptionCode::SLAVE_DEVICE_FAILURE):
        case static_cast<uint8_t>(ExceptionCode::ACKNOWLEDGE):
        case static_cast<uint8_t>(ExceptionCode::SLAVE_DEVICE_BUSY):
        case static_cast<uint8_t>(ExceptionCode::MEMORY_PARITY_ERROR):
        case static_cast<uint8_t>(ExceptionCode::GATEWAY_PATH_UNAVAILABLE):
        case static_cast<uint8_t>(ExceptionCode::GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND):
            code = static_cast<ExceptionCode>(msg->code);
            break;

        default:
            throw std::runtime_error("Invalid exception code");
    }
}

} // namespace tcp
} // namespace modbus

#endif
