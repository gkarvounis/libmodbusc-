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

    void                        decodeHeader(const std::vector<uint8_t>& rx_buffer, Header& header);
    void                        decodeReadCoilsReq(const std::vector<uint8_t>& rx_buffer, Address& address, NumBits& numCoils);
    void                        decodeReadDiscreteInputsReq(const std::vector<uint8_t>& rx_buffer, Address& address, NumBits& numCoils);
private:
};


void Decoder::decodeHeader(const std::vector<uint8_t>& rx_buffer, Decoder::Header& header) {
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


void Decoder::decodeReadCoilsReq(const std::vector<uint8_t>& rx_buffer, Address& address, NumBits& numCoils) {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    address.set(ntohs(msg->startAddress));
    numCoils.set(ntohs(msg->numEntries));
}


void Decoder::decodeReadDiscreteInputsReq(const std::vector<uint8_t>& rx_buffer, Address& address, NumBits& numCoils) {
    const auto* msg = reinterpret_cast<const modbus::tcp::ReadReq*>(rx_buffer.data());
    address.set(ntohs(msg->startAddress));
    numCoils.set(ntohs(msg->numEntries));
}

} // namespace tcp
} // namespace modbus

#endif
