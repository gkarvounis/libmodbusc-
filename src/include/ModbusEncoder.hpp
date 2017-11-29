#ifndef MODBUS_ENCODER_HPP
#define MODBUS_ENCODER_HPP

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include <arpa/inet.h>

namespace modbus {
namespace tcp {

class Encoder {
public:
                                        Encoder();
                                        Encoder(const modbus::tcp::UnitId& unitId, const modbus::tcp::TransactionId& transactionId);

    void                                setUnitId(const UnitId& unitId);
    void                                setTransactionId(const TransactionId& transactionId);
    void                                setTargetBuffer(uint8_t* buffer, std::size_t capacity);

    void                                encodeReadCoilsReq(const Address& startAddr, const NumBits& numBits, std::vector<uint8_t>& target);
    void                                encodeReadDiscreteInputsReq(const Address& startAddr, const NumBits& numBits, std::vector<uint8_t>& target);
    void                                encodeReadHoldingRegistersReq(const Address& startAddr, const NumRegs& numRegs, std::vector<uint8_t>& target);
    void                                encodeReadInputRegistersReq(const Address& startAddr, const NumRegs& numRegs, std::vector<uint8_t>& target);
    void                                encodeWriteSingleCoilReq(const Address& address, bool value, std::vector<uint8_t>& target);
    void                                encodeWriteSingleRegisterReq(const Address& address, uint16_t value, std::vector<uint8_t>& target);

    template<typename Iterator>
    std::size_t                         encodeReadCoilsRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity);

    template<typename Iterator>
    std::size_t                         encodeReadDiscreteInputsRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity);

    template<typename Iterator>
    std::size_t                         encodeReadHoldingRegistersRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity);

    template<typename Iterator>
    std::size_t                         encodeReadInputRegistersRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity);

    void                                encodeWriteSingleCoilRsp(const Address& address, bool value, std::vector<uint8_t>& target);
    void                                encodeWriteSingleRegisterRsp(const Address& address, uint16_t value, std::vector<uint8_t>& target);

private:
    struct Header {
        uint16_t                        transactionId;
        uint16_t                        protocolId;
        uint16_t                        length;
        uint8_t                         unitId;
        uint8_t                         functionCode;
    } __attribute__((packed));

    struct ReadReq {
        Header                          header;
        uint16_t                        startAddress;
        uint16_t                        numEntries;
    } __attribute__((packed));

    struct WriteSingleValue { // no need to differentiate request / response
        Header                          header;
        uint16_t                        address;
        uint16_t                        value;
    } __attribute__((packed));

    struct ReadCoilsRsp {
        Header                          header;
        uint8_t                         numBytes;
        uint8_t                         coils[];
    } __attribute__((packed));

    struct ReadRegsRsp {
        Header                          header;
        uint8_t                         numBytes;
        uint16_t                        regs[];
    } __attribute__((packed));


    void                                encodeReadReq(FunctionCode code, uint16_t startAddress, uint16_t numValues, std::vector<uint8_t>& buffer);
    void                                encodeWriteSingleValue(FunctionCode code, uint16_t address, uint16_t numValues, std::vector<uint8_t>& buffer);

    template <typename Iterator>
    std::size_t                         encodeReadBitsRsp(Iterator begin, Iterator end, FunctionCode code, uint8_t* buffer, std::size_t capacity);

    template <typename Iterator>
    std::size_t                         encodeReadRegsRsp(Iterator begin, Iterator end, FunctionCode code, uint8_t* buffer, std::size_t capacity);

    TransactionId                       m_transactionId;
    UnitId                              m_unitId;
};


Encoder::Encoder() :
    m_transactionId(0),
    m_unitId(1)
{}


Encoder::Encoder(const UnitId& unitId, const TransactionId& transactionId) :
    m_transactionId(transactionId),
    m_unitId(unitId)
{}


void Encoder::setUnitId(const UnitId& unitId) {
    m_unitId = unitId;
}


void Encoder::setTransactionId(const TransactionId& transactionId) {
    m_transactionId = transactionId;
}


void Encoder::encodeReadCoilsReq(const Address& startAddress, const NumBits& numBits, std::vector<uint8_t>& target) {
    encodeReadReq(FunctionCode::READ_COILS, startAddress.get(), numBits.get(), target);
}


void Encoder::encodeReadDiscreteInputsReq(const Address& startAddress, const NumBits& numBits, std::vector<uint8_t>& target) {
    encodeReadReq(FunctionCode::READ_DISCRETE_INPUTS, startAddress.get(), numBits.get(), target);
}


void Encoder::encodeReadHoldingRegistersReq(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) {
    encodeReadReq(FunctionCode::READ_HOLDING_REGISTERS, startAddress.get(), numRegs.get(), target);
}


void Encoder::encodeReadInputRegistersReq(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) {
    encodeReadReq(FunctionCode::READ_INPUT_REGISTERS, startAddress.get(), numRegs.get(), target);
}


void Encoder::encodeReadReq(FunctionCode funcCode, uint16_t startAddress, uint16_t numValues, std::vector<uint8_t>& target) {
    target.resize(sizeof(ReadReq));

    auto* msg = reinterpret_cast<ReadReq*>(target.data());

    msg->header.transactionId = htons(m_transactionId.get());
    msg->header.protocolId = htons(MODBUS_PROTOCOL_ID);
    msg->header.length = htons(sizeof(ReadReq) - 6);
    msg->header.unitId = m_unitId.get();
    msg->header.functionCode = static_cast<uint8_t>(funcCode);
    msg->startAddress = htons(startAddress);
    msg->numEntries = htons(numValues);
}


void Encoder::encodeWriteSingleCoilReq(const Address& address, bool value, std::vector<uint8_t>& target) {
    encodeWriteSingleValue(FunctionCode::WRITE_COIL, address.get(), value ? 0xFF00 : 0x0000, target);
}


void Encoder::encodeWriteSingleRegisterReq(const Address& address, uint16_t value, std::vector<uint8_t>& target) {
    encodeWriteSingleValue(FunctionCode::WRITE_REGISTER, address.get(), value, target);
}


void Encoder::encodeWriteSingleValue(FunctionCode funcCode, uint16_t address, uint16_t value, std::vector<uint8_t>& target) {
    target.resize(sizeof(WriteSingleValue));

    auto* msg = reinterpret_cast<Encoder::WriteSingleValue*>(target.data());

    msg->header.transactionId = htons(m_transactionId.get());
    msg->header.protocolId = htons(MODBUS_PROTOCOL_ID);
    msg->header.length = htons(sizeof(WriteSingleValue) - 6);
    msg->header.unitId = m_unitId.get();
    msg->header.functionCode = static_cast<uint8_t>(funcCode);

    msg->address = htons(address);
    msg->value = htons(value);
}


void Encoder::encodeWriteSingleCoilRsp(const Address& address, bool value, std::vector<uint8_t>& target) {
    encodeWriteSingleValue(FunctionCode::WRITE_COIL, address.get(), value ? 0xFF00 : 0x0000, target);
}


void Encoder::encodeWriteSingleRegisterRsp(const Address& address, uint16_t value, std::vector<uint8_t>& target) {
    encodeWriteSingleValue(FunctionCode::WRITE_REGISTER, address.get(), value, target);
}


template<typename Iterator>
std::size_t Encoder::encodeReadCoilsRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity) {
    return encodeReadBitsRsp(begin, end, FunctionCode::READ_COILS, buffer, capacity);
}


template<typename Iterator>
std::size_t Encoder::encodeReadDiscreteInputsRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity) {
    return encodeReadBitsRsp(begin, end, FunctionCode::READ_DISCRETE_INPUTS, buffer, capacity);
}


template <typename Iterator>
std::size_t Encoder::encodeReadBitsRsp(Iterator begin, Iterator end, FunctionCode code, uint8_t* buffer, std::size_t capacity) {
    std::size_t num_bits = 0;
    for (auto it = begin; it != end; ++it)
        num_bits++;

    std::size_t numBytes = num_bits/8;
    if (num_bits%8 != 0)
        numBytes += 1;

    if (capacity < sizeof(ReadCoilsRsp) + numBytes)
        throw std::logic_error("Too small buffer. Cannot encode read coils response");

    memset(buffer, 0, capacity);

    auto* msg = reinterpret_cast<Encoder::ReadCoilsRsp*>(buffer);

    msg->header.transactionId = htons(m_transactionId.get());
    msg->header.protocolId = htons(MODBUS_PROTOCOL_ID);
    msg->header.length = htons(sizeof(ReadCoilsRsp) + numBytes - 6);
    msg->header.unitId = m_unitId.get();
    msg->header.functionCode = static_cast<uint8_t>(code);
    msg->numBytes = numBytes;

    std::size_t pos = 0;

    for (auto it = begin; it != end; ++it) {
        std::size_t byte_idx = pos / 8;
        std::size_t bit_idx = pos % 8;

        if (*it)
            msg->coils[byte_idx] |= (1 << bit_idx);

        ++pos;
    }

    return sizeof(ReadCoilsRsp) + numBytes;
}


template<typename Iterator>
std::size_t Encoder::encodeReadHoldingRegistersRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity) {
    return encodeReadRegsRsp(begin, end, FunctionCode::READ_HOLDING_REGISTERS, buffer, capacity);
}


template<typename Iterator>
std::size_t Encoder::encodeReadInputRegistersRsp(Iterator begin, Iterator end, uint8_t* buffer, std::size_t capacity) {
    return encodeReadRegsRsp(begin, end, FunctionCode::READ_INPUT_REGISTERS, buffer, capacity);
}


template <typename Iterator>
std::size_t Encoder::encodeReadRegsRsp(Iterator begin, Iterator end, FunctionCode code, uint8_t* buffer, std::size_t capacity) {
    std::size_t numBytes = 0;
    for (auto it = begin; it != end; ++it)
        numBytes += 2;

    if (capacity < sizeof(ReadRegsRsp) + numBytes)
        throw std::logic_error("Too small buffer. Cannot encode read registers response");

    auto* msg = reinterpret_cast<Encoder::ReadRegsRsp*>(buffer);

    msg->header.transactionId = htons(m_transactionId.get());
    msg->header.protocolId = htons(MODBUS_PROTOCOL_ID);
    msg->header.length = htons(sizeof(ReadRegsRsp) + numBytes - 6);
    msg->header.unitId = m_unitId.get();
    msg->header.functionCode = static_cast<uint8_t>(code);
    msg->numBytes = numBytes;

    std::size_t pos = 0;
    for (auto it = begin; it != end; ++it)
        msg->regs[pos++] = htons(*it);

    return sizeof(ReadRegsRsp) + numBytes;
}

} // namespace tcp
} // namespace modbus

#endif

