#ifndef MODBUS_ENCODER_HPP
#define MODBUS_ENCODER_HPP

#include "ModbusConsts.hpp"
#include "ModbusSerialized.hpp"
#include "ModbusEncoderUtils.hpp"

namespace modbus {
namespace tcp {
namespace encoder {

typedef ReadValuesReq<modbus::tcp::READ_COILS, MODBUS_MAX_NUM_BITS_IN_READ_REQUEST>             ReadCoilsReq;
typedef ReadValuesReq<modbus::tcp::READ_DISCRETE_INPUTS, MODBUS_MAX_NUM_BITS_IN_READ_REQUEST>   ReadDiscreteInputsReq;
typedef ReadValuesReq<modbus::tcp::READ_HOLDING_REGISTERS, 125>  ReadHoldingRegistersReq;
typedef ReadValuesReq<modbus::tcp::READ_INPUT_REGISTERS, 125>    ReadInputRegistersReq;


class WriteCoilReq : public Message<serialized::WriteValueReq> {
public:
    typedef serialized::WriteValueReq       Buffer;

    inline                                  WriteCoilReq(Buffer& content);
    inline void                             setAddress(uint16_t addr);
    inline void                             setValue(bool value);
};


class WriteRegisterReq : public Message<serialized::WriteValueReq> {
public:
    typedef serialized::WriteValueReq       Buffer;

    inline                                  WriteRegisterReq(Buffer& content);
    inline void                             setAddress(uint16_t addr);
    inline void                             setValue(uint16_t value);
};


class WriteCoilsReq : public Message<serialized::SetBitsRequest> {
public:
    typedef serialized::SetBitsRequest      Buffer;

    inline                                  WriteCoilsReq(Buffer& content, uint16_t numBits);
    inline void                             setStartAddress(uint16_t startAddr);
    inline void                             setValue(uint16_t pos, bool value);

private:
    uint16_t                                m_numBits;
};


class WriteRegistersReq : public Message<serialized::SetRegsRequest> {
public:
    typedef serialized::SetRegsRequest      Buffer;

                                            WriteRegistersReq(Buffer& content, uint8_t numRegs);
    void                                    setStartAddress(uint16_t startAddr);
    void                                    setValue(uint16_t pos, uint16_t value);

private:
    uint16_t                                m_numRegs;
};


class ErrorResponse : public Message<serialized::ErrorResponse> {
public:
    typedef serialized::ErrorResponse Buffer;

    inline                                  ErrorResponse(Buffer& content, modbus::tcp::FunctionCode funcCode);
    inline void                             setExceptionCode(modbus::tcp::ExceptionCode ex);
};


class ReadCoilsRsp : public ReadBitsRsp<READ_COILS> {
public:
    typedef serialized::GetBitsResponse     Buffer;

    inline                                  ReadCoilsRsp(Buffer& content, uint16_t numBits) :
                                                ReadBitsRsp(content, numBits)
                                            {}
};


class ReadDiscreteInputsRsp : public ReadBitsRsp<READ_DISCRETE_INPUTS> {
public:
    typedef serialized::GetBitsResponse     Buffer;

    inline                                  ReadDiscreteInputsRsp(Buffer& content, uint16_t numBits) :
                                                ReadBitsRsp(content, numBits)
                                            {}
};


typedef ReadRegsRsp<READ_HOLDING_REGISTERS> ReadHoldingRegsRsp;
typedef ReadRegsRsp<READ_INPUT_REGISTERS>   ReadInputRegsRsp;


class WriteCoilResponse : public WriteValueResponse<WRITE_COIL> {
public:
    typedef serialized::WriteValueResponse  Buffer;

    inline                                  WriteCoilResponse(Buffer& content) :
                                                WriteValueResponse<WRITE_COIL>(content) {}
    inline void                             setValue(bool value);
};


class WriteRegisterResponse : public WriteValueResponse<WRITE_REGISTER> {
public:
    typedef serialized::WriteValueResponse  Buffer;

    inline                                  WriteRegisterResponse(Buffer& content) :
                                                WriteValueResponse<WRITE_REGISTER>(content) {}
    inline void                             setValue(uint16_t value);
};


typedef WriteValuesResponse<WRITE_COILS, MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST>        WriteCoilsRsp;
typedef WriteValuesResponse<WRITE_REGISTERS, MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST>    WriteRegsRsp;


WriteCoilReq::WriteCoilReq(serialized::WriteValueReq& content) :
    Message<serialized::WriteValueReq>(modbus::tcp::WRITE_COIL, content)
{
    content.header.length = htons(6);
}


void WriteCoilReq::setAddress(uint16_t addr) {
    content.address = htons(addr);
}


void WriteCoilReq::setValue(bool value) {
    content.value = htons(uint16_t(value ? 0xFF00 : 0x0000));
}


WriteRegisterReq::WriteRegisterReq(Buffer& content) :
    Message<serialized::WriteValueReq>(modbus::tcp::WRITE_REGISTER, content)
{
    content.header.length = htons(6);
}


void WriteRegisterReq::setAddress(uint16_t addr) {
    content.address = htons(addr);
}


void WriteRegisterReq::setValue(uint16_t value) {
    content.value = htons(value);
}


WriteCoilsReq::WriteCoilsReq(Buffer& content, uint16_t numBits) :
    Message<serialized::SetBitsRequest>(modbus::tcp::WRITE_COILS, content),
    m_numBits(numBits)
{
    if (numBits > MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST)
        throw std::out_of_range((StringBuilder()
                << "Number of coils ("
                << numBits
                << ") exceeds message capacity ("
                << MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST
                << ")").str());

    this->content.header.length = htons(uint16_t(2 + 5 + NUM_BITS_TO_BYTES(numBits)));
    this->content.numValues = htons(numBits);
    this->content.numBytes = uint8_t(NUM_BITS_TO_BYTES(numBits));
}


void WriteCoilsReq::setStartAddress(uint16_t startAddr) {
    this->content.startAddr = htons(startAddr);
}


void WriteCoilsReq::setValue(uint16_t pos, bool value) {
    if (pos >= m_numBits)
        throw std::out_of_range((StringBuilder()
                << "Attempt to set coil at position " << pos
                << " which exceeds message size (" << m_numBits << ")").str());

    uint8_t byte_num = uint8_t(pos/8);
    uint8_t bit_num = uint8_t(pos%8);

    if (value)
        this->content.values[byte_num] |= (1<<bit_num);
    else
        this->content.values[byte_num] &= ~(1<<bit_num);
}


WriteRegistersReq::WriteRegistersReq(Buffer& content, uint8_t numRegs) :
    Message<serialized::SetRegsRequest>(modbus::tcp::WRITE_REGISTERS, content),
    m_numRegs(numRegs)

{
    if (numRegs > MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST)
        throw std::out_of_range((StringBuilder()
                << "Number of registers ("
                << numRegs
                << ") exceeds message capacity ("
                << MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST
                << ")").str());

    this->content.header.length = htons(uint16_t(2 + 5 + 2*numRegs));
    this->content.numValues = htons(numRegs);
    this->content.numBytes = uint8_t(2*numRegs);
}


void WriteRegistersReq::setStartAddress(uint16_t startAddr) {
    this->content.startAddr = htons(startAddr);
}



void WriteRegistersReq::setValue(uint16_t pos, uint16_t value) {
    if (pos >= m_numRegs)
        throw std::out_of_range((StringBuilder()
                << "Attempt to set register at position " << pos
                << " which exceeds message size (" << m_numRegs << ")").str());

    this->content.values[pos] = htons(value);
}


ErrorResponse::ErrorResponse(Buffer& content, modbus::tcp::FunctionCode funcCode) :
    Message<serialized::ErrorResponse>(funcCode | 0x80, content)
{
    content.header.length = htons(3);
}


void ErrorResponse::setExceptionCode(modbus::tcp::ExceptionCode ex) {
    content.errCode = ex;
}


void WriteCoilResponse::setValue(bool value) {
    content.value = htons(uint16_t(value?0xFF00:0));
}


void WriteRegisterResponse::setValue(uint16_t value) {
    content.value = htons(value);
}


}; // namespace encoder
}; // namespace tcp
}; // namespace modbus

#endif
