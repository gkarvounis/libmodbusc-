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

    void                                encodeReadCoilsReq(const Address& startAddr, const NumBits& numBits, std::vector<uint8_t>& target) const;
    void                                encodeReadDiscreteInputsReq(const Address& startAddr, const NumBits& numBits, std::vector<uint8_t>& target) const;
    void                                encodeReadHoldingRegistersReq(const Address& startAddr, const NumRegs& numRegs, std::vector<uint8_t>& target) const;
    void                                encodeReadInputRegistersReq(const Address& startAddr, const NumRegs& numRegs, std::vector<uint8_t>& target) const;

    void                                encodeWriteSingleCoilReq(const Address& address, bool value, std::vector<uint8_t>& target) const;
    void                                encodeWriteSingleRegisterReq(const Address& address, uint16_t value, std::vector<uint8_t>& target) const;
    template <typename Iterator>
    void                                encodeWriteCoilsReq(const Address& startAddress, Iterator begin, Iterator end, std::vector<uint8_t>& target) const;
    template <typename Iterator>
    void                                encodeWriteRegistersReq(const Address& startAddress, Iterator begin, Iterator end, std::vector<uint8_t>& target) const;

    template<typename Iterator>
    void                                encodeReadCoilsRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const;
    template<typename Iterator>
    void                                encodeReadDiscreteInputsRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const;
    template<typename Iterator>
    void                                encodeReadHoldingRegistersRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const;
    template<typename Iterator>
    void                                encodeReadInputRegistersRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const;

    void                                encodeWriteSingleCoilRsp(const Address& address, bool value, std::vector<uint8_t>& target) const;
    void                                encodeWriteSingleRegisterRsp(const Address& address, uint16_t value, std::vector<uint8_t>& target) const;
    void                                encodeWriteCoilsRsp(const Address& startAddress, const NumBits& numCoils, std::vector<uint8_t>& target) const;
    void                                encodeWriteRegistersRsp(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) const;

    void                                encodeErrorRsp(FunctionCode code, ExceptionCode ex, std::vector<uint8_t>& target) const;

private:
    void                                encodeReadReq(FunctionCode code, uint16_t startAddress, uint16_t numValues, std::vector<uint8_t>& buffer) const;
    void                                encodeWriteSingleValue(FunctionCode code, uint16_t address, uint16_t numValues, std::vector<uint8_t>& buffer) const;

    template <typename Iterator>
    void                                encodeReadBitsRsp(Iterator begin, Iterator end, FunctionCode code, std::vector<uint8_t>& buffer) const;
    template <typename Iterator>
    void                                encodeReadRegsRsp(Iterator begin, Iterator end, FunctionCode code, std::vector<uint8_t>& buffer) const;

    void                                fillHeader(modbus::tcp::Header* header, uint16_t size, FunctionCode code) const;

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


void Encoder::encodeReadCoilsReq(const Address& startAddress, const NumBits& numBits, std::vector<uint8_t>& target) const {
    encodeReadReq(FunctionCode::READ_COILS, startAddress.get(), numBits.get(), target);
}


void Encoder::encodeReadDiscreteInputsReq(const Address& startAddress, const NumBits& numBits, std::vector<uint8_t>& target) const {
    encodeReadReq(FunctionCode::READ_DISCRETE_INPUTS, startAddress.get(), numBits.get(), target);
}


void Encoder::encodeReadHoldingRegistersReq(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) const {
    encodeReadReq(FunctionCode::READ_HOLDING_REGISTERS, startAddress.get(), numRegs.get(), target);
}


void Encoder::encodeReadInputRegistersReq(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) const {
    encodeReadReq(FunctionCode::READ_INPUT_REGISTERS, startAddress.get(), numRegs.get(), target);
}


void Encoder::encodeReadReq(FunctionCode funcCode, uint16_t startAddress, uint16_t numValues, std::vector<uint8_t>& target) const {
    target.resize(sizeof(ReadReq));

    auto* msg = reinterpret_cast<ReadReq*>(target.data());
    fillHeader(&msg->header, sizeof(ReadReq), funcCode);

    msg->startAddress = htons(startAddress);
    msg->numEntries = htons(numValues);
}


void Encoder::encodeWriteSingleCoilReq(const Address& address, bool value, std::vector<uint8_t>& target) const {
    encodeWriteSingleValue(FunctionCode::WRITE_COIL, address.get(), value ? 0xFF00 : 0x0000, target);
}


void Encoder::encodeWriteSingleRegisterReq(const Address& address, uint16_t value, std::vector<uint8_t>& target) const {
    encodeWriteSingleValue(FunctionCode::WRITE_REGISTER, address.get(), value, target);
}


template <typename Iterator>
void Encoder::encodeWriteCoilsReq(const Address& startAddress, Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    std::size_t num_bits = 0;
    for (auto it = begin; it != end; ++it)
        num_bits++;

    std::size_t numBytes = num_bits/8;
    if (num_bits%8 != 0)
        numBytes += 1;

    target.resize(sizeof(WriteCoilsReq) + numBytes);
    std::fill(target.begin(), target.end(), 0);

    auto* msg = reinterpret_cast<WriteCoilsReq*>(target.data());
    fillHeader(&msg->header, sizeof(WriteCoilsReq) + numBytes, FunctionCode::WRITE_COILS);

    msg->startAddress = htons(startAddress.get());
    msg->numBits = htons(num_bits);
    msg->numBytes = numBytes;

    std::size_t pos = 0;

    for (auto it = begin; it != end; ++it) {
        std::size_t byte_idx = pos / 8;
        std::size_t bit_idx = pos % 8;

        if (*it)
            msg->coils[byte_idx] |= (1 << bit_idx);

        ++pos;
    }
}


template <typename Iterator>
void Encoder::encodeWriteRegistersReq(const Address& startAddress, Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    std::size_t numRegs = 0;
    for (auto it = begin; it != end; ++it)
        numRegs++;

    target.resize(sizeof(WriteRegsReq) + 2*numRegs);

    auto* msg = reinterpret_cast<WriteRegsReq*>(target.data());

    msg->header.transactionId = htons(m_transactionId.get());
    msg->header.protocolId = htons(MODBUS_PROTOCOL_ID);
    msg->header.length = htons(sizeof(WriteRegsReq) + 2*numRegs - 6);
    msg->header.unitId = m_unitId.get();
    msg->header.functionCode = static_cast<uint8_t>(FunctionCode::WRITE_REGISTERS);
    msg->startAddress = htons(startAddress.get());
    msg->numRegs = htons(numRegs);
    msg->numBytes = 2*numRegs;

    std::size_t pos = 0;
    for (auto it = begin; it != end; ++it)
        msg->regs[pos++] = htons(*it);
}


void Encoder::encodeWriteSingleValue(FunctionCode funcCode, uint16_t address, uint16_t value, std::vector<uint8_t>& target) const {
    target.resize(sizeof(WriteSingleValue));

    auto* msg = reinterpret_cast<WriteSingleValue*>(target.data());
    fillHeader(&msg->header, sizeof(WriteSingleValue), funcCode);

    msg->address = htons(address);
    msg->value = htons(value);
}


void Encoder::encodeWriteSingleCoilRsp(const Address& address, bool value, std::vector<uint8_t>& target) const {
    encodeWriteSingleValue(FunctionCode::WRITE_COIL, address.get(), value ? 0xFF00 : 0x0000, target);
}


void Encoder::encodeWriteSingleRegisterRsp(const Address& address, uint16_t value, std::vector<uint8_t>& target) const {
    encodeWriteSingleValue(FunctionCode::WRITE_REGISTER, address.get(), value, target);
}


template<typename Iterator>
void Encoder::encodeReadCoilsRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    encodeReadBitsRsp(begin, end, FunctionCode::READ_COILS, target);
}


template<typename Iterator>
void Encoder::encodeReadDiscreteInputsRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    encodeReadBitsRsp(begin, end, FunctionCode::READ_DISCRETE_INPUTS, target);
}


template <typename Iterator>
void Encoder::encodeReadBitsRsp(Iterator begin, Iterator end, FunctionCode code, std::vector<uint8_t>& target) const {
    std::size_t num_bits = 0;
    for (auto it = begin; it != end; ++it)
        num_bits++;

    std::size_t numBytes = num_bits/8;
    if (num_bits%8 != 0)
        numBytes += 1;

    target.resize(sizeof(ReadCoilsRsp) + numBytes);
    std::fill(target.begin(), target.end(), 0);

    auto* msg = reinterpret_cast<ReadCoilsRsp*>(target.data());
    fillHeader(&msg->header, sizeof(ReadCoilsRsp) + numBytes, code);
    msg->numBytes = numBytes;

    std::size_t pos = 0;

    for (auto it = begin; it != end; ++it) {
        std::size_t byte_idx = pos / 8;
        std::size_t bit_idx = pos % 8;

        if (*it)
            msg->coils[byte_idx] |= (1 << bit_idx);

        ++pos;
    }
}


template<typename Iterator>
void Encoder::encodeReadHoldingRegistersRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    encodeReadRegsRsp(begin, end, FunctionCode::READ_HOLDING_REGISTERS, target);
}


template<typename Iterator>
void Encoder::encodeReadInputRegistersRsp(Iterator begin, Iterator end, std::vector<uint8_t>& target) const {
    encodeReadRegsRsp(begin, end, FunctionCode::READ_INPUT_REGISTERS, target);
}


template <typename Iterator>
void Encoder::encodeReadRegsRsp(Iterator begin, Iterator end, FunctionCode code, std::vector<uint8_t>& target) const {
    std::size_t numBytes = 0;
    for (auto it = begin; it != end; ++it)
        numBytes += 2;

    target.resize(sizeof(ReadRegsRsp) + numBytes);

    auto* msg = reinterpret_cast<ReadRegsRsp*>(target.data());

    fillHeader(&msg->header, sizeof(ReadRegsRsp) + numBytes, code);
    msg->numBytes = numBytes;

    std::size_t pos = 0;
    for (auto it = begin; it != end; ++it)
        msg->regs[pos++] = htons(*it);
}


void Encoder::encodeWriteCoilsRsp(const Address& startAddress, const NumBits& numCoils, std::vector<uint8_t>& target) const {
    target.resize(sizeof(WriteValuesRsp));
    auto* msg = reinterpret_cast<WriteValuesRsp*>(target.data());

    fillHeader(&msg->header, sizeof(WriteValuesRsp), FunctionCode::WRITE_COILS);
    msg->startAddr = htons(startAddress.get());
    msg->numValues = htons(numCoils.get());
}


void Encoder::encodeWriteRegistersRsp(const Address& startAddress, const NumRegs& numRegs, std::vector<uint8_t>& target) const {
    target.resize(sizeof(WriteValuesRsp));

    auto* msg = reinterpret_cast<WriteValuesRsp*>(target.data());
    fillHeader(&msg->header, sizeof(WriteValuesRsp), FunctionCode::WRITE_REGISTERS);

    msg->startAddr = htons(startAddress.get());
    msg->numValues = htons(numRegs.get());
}


void Encoder::encodeErrorRsp(FunctionCode code, ExceptionCode ex, std::vector<uint8_t>& target) const {
    target.resize(sizeof(ExceptionRsp));

    auto* msg = reinterpret_cast<ExceptionRsp*>(target.data());
    fillHeader(&msg->header, sizeof(ExceptionRsp), code);

    msg->header.functionCode = static_cast<uint8_t>(code) | 0x80;
    msg->code = static_cast<uint8_t>(ex);
}


void Encoder::fillHeader(modbus::tcp::Header *header, uint16_t size, FunctionCode code) const {
    header->transactionId = htons(m_transactionId.get());
    header->protocolId = htons(MODBUS_PROTOCOL_ID);
    header->length = htons(size - 6);
    header->unitId = m_unitId.get();
    header->functionCode = static_cast<uint8_t>(code);
}


} // namespace tcp
} // namespace modbus

#endif
