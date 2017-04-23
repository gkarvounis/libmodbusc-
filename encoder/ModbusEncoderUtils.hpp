#ifndef MODBUS_ENCODER_UTILS_HPP
#define MODBUS_ENCODER_UTILS_HPP


#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <stdexcept>


namespace modbus {
namespace tcp {

class StringBuilder {
public:
    template <typename T>
    StringBuilder& operator<<(const T& rhs) {
        ss << rhs;
        return *this;
    }

    std::string str() const {
        return ss.str();
    }

private:
    std::stringstream ss;
};


namespace serialzed = modbus::tcp::serialized;

template <typename Content>
class Message {
public:
                                            Message(uint8_t funcCode, Content& content);
    void                                    setUnitId(uint8_t unitId);
    void                                    setTransactionId(uint16_t transactionId);
    std::size_t                             message_size() const;

protected:
    Content                                &content;
};


template <modbus::tcp::FunctionCode FuncCode, uint16_t MaxNumValues>
class ReadValuesReq : public Message<serialized::ReadValuesReq> {
public:
    typedef serialized::ReadValuesReq       Buffer;

                                            ReadValuesReq(Buffer& content);
    void                                    setStartAddress(uint16_t startAddr);
    void                                    setNumValues(uint16_t num_coils);
};


template <modbus::tcp::FunctionCode funcCode>
class ReadBitsRsp : public Message<serialized::GetBitsResponse> {
public:
    typedef serialized::GetBitsResponse     Buffer;

                                            ReadBitsRsp(Buffer& content, uint16_t numBits);
    void                                    setValue(uint16_t pos, bool value);
private:
    uint16_t                                m_numBits;
};


template <FunctionCode funcCode>
class ReadRegsRsp : public Message<serialized::GetRegsResponse> {
public:
    typedef serialized::GetRegsResponse     Buffer;

                                            ReadRegsRsp(Buffer& content, uint8_t numRegs);
    void                                    setValue(uint8_t pos, uint16_t value);
private:
    uint8_t                                 m_numRegs;
};


template <FunctionCode funcCode>
class WriteValueResponse : public Message<serialized::WriteValueResponse> {
public:
    typedef serialized::WriteValueResponse  Buffer;
    WriteValueResponse(Buffer& content);
    void                                    setAddress(uint16_t address);
};


template <FunctionCode funcCode, uint16_t MaxNumValues>
class WriteValuesResponse : public Message<serialized::WriteValuesResponse> {
public:
    typedef serialized::WriteValuesResponse Buffer;

                                            WriteValuesResponse(Buffer& content);
    void                                    setStartAddress(uint16_t startAddress);
    void                                    setNumValues(uint16_t numValues);
};


template <typename Content>
Message<Content>::Message(uint8_t funcCode, Content& content) :
        content(content)
{
    memset(&content, 0, sizeof(content));
    this->content.header.protocolId = MODBUS_PROTOCOL_ID;
    this->content.header.functionCode = funcCode;
}


template <typename Content>
void Message<Content>::setUnitId(uint8_t unitId) {
    content.header.unitId = unitId;
}


template <typename Content>
void Message<Content>::setTransactionId(uint16_t transactionId) {
    content.header.transactionId = htons(transactionId);
}


template <typename  Content>
std::size_t Message<Content>::message_size() const {
    return ntohs(content.header.length) + 6;
}


template <modbus::tcp::FunctionCode FuncCode, uint16_t MaxNumValues>
ReadValuesReq<FuncCode, MaxNumValues>::ReadValuesReq(Buffer& content) :
        Message<serialized::ReadValuesReq>(FuncCode, content)
{
    content.header.length = htons(6);
}


template <modbus::tcp::FunctionCode FuncCode, uint16_t MaxNumValues>
void ReadValuesReq<FuncCode, MaxNumValues>::setStartAddress(uint16_t startAddr) {
    content.startAddr = htons(startAddr);
}


template <modbus::tcp::FunctionCode FuncCode, uint16_t MaxNumValues>
void ReadValuesReq<FuncCode, MaxNumValues>::setNumValues(uint16_t numValues) {
    if (numValues > MaxNumValues)
        throw std::out_of_range((StringBuilder()
                << "According to modbus specs you can request up to "
                << MaxNumValues
                << " values but you requested "
                << numValues)
                                        .str());

    content.numValues = htons(numValues);
}


template <modbus::tcp::FunctionCode funcCode>
ReadBitsRsp<funcCode>::ReadBitsRsp(Buffer& content, uint16_t numBits) :
        Message<serialized::GetBitsResponse>(funcCode, content),
        m_numBits(numBits)
{
    if (numBits > MODBUS_MAX_NUM_BITS_IN_READ_REQUEST)
        throw std::out_of_range((StringBuilder() << "Number of bits in response (" << numBits << " bits) exceeds message capacity (" << MODBUS_MAX_NUM_BITS_IN_READ_REQUEST << " bits)").str());

    this->content.header.length = htons(uint8_t(2 + 1 + NUM_BITS_TO_BYTES(numBits)));
    this->content.numBytes = uint8_t(NUM_BITS_TO_BYTES(numBits));
}


template <modbus::tcp::FunctionCode funcCode>
void ReadBitsRsp<funcCode>::setValue(uint16_t pos, bool value) {
    if (pos >= m_numBits)
        throw std::out_of_range((StringBuilder() << "Attempt to set bit at position " << pos << " which exceeds max position for this message (=" << m_numBits-1 <<")").str());

    uint8_t byte_num = uint8_t(pos/8);
    uint8_t bit_num = uint8_t(pos%8);

    if (value)
        this->content.values[byte_num] |= (1<<bit_num);
    else
        this->content.values[byte_num] &= ~(1<<bit_num);
}


template <modbus::tcp::FunctionCode funcCode>
ReadRegsRsp<funcCode>::ReadRegsRsp(Buffer& content, uint8_t numRegs) :
        Message<serialized::GetRegsResponse>(funcCode, content),
        m_numRegs(numRegs)
{
    if (numRegs > MODBUS_MAX_NUM_REGS_IN_READ_REQUEST)
        throw std::out_of_range((StringBuilder() << "Number of registers in response (" << numRegs << " ) exceeds message capacity (" << MODBUS_MAX_NUM_REGS_IN_READ_REQUEST << ")").str());

    this->content.header.length = htons(uint8_t(2 + 1 + 2*numRegs));
    this->content.numBytes = uint8_t(2*numRegs);
}


template <modbus::tcp::FunctionCode funcCode>
void ReadRegsRsp<funcCode>::setValue(uint8_t pos, uint16_t value) {
    if (pos >= m_numRegs)
        throw std::out_of_range((StringBuilder() << "Attempt to set register at position " << pos << " which exceeds limit for this message (" << m_numRegs-1 << ")").str());

    this->content.values[pos] = htons(value);
}


template <FunctionCode funcCode>
WriteValueResponse<funcCode>::WriteValueResponse(Buffer& content) :
        Message<serialized::WriteValueResponse>(funcCode, content)
{
    this->content.header.length = htons(6);
}


template <FunctionCode funcCode>
void WriteValueResponse<funcCode>::setAddress(uint16_t address) {
    this->content.address = htons(address);
}


template <FunctionCode funcCode, uint16_t MaxNumValues>
WriteValuesResponse<funcCode, MaxNumValues>::WriteValuesResponse(serialized::WriteValuesResponse& content) :
        Message<serialized::WriteValuesResponse>(funcCode, content)
{
    this->content.header.length = htons(6);
}


template <FunctionCode funcCode, uint16_t MaxNumValues>
void  WriteValuesResponse<funcCode, MaxNumValues>::setStartAddress(uint16_t startAddress) {
    this->content.startAddr = htons(startAddress);
}


template <FunctionCode funcCode, uint16_t MaxNumValues>
void WriteValuesResponse<funcCode, MaxNumValues>::setNumValues(uint16_t numValues) {
    if (numValues > MaxNumValues)
        throw std::out_of_range((StringBuilder() << "Max allowed number of values for this response is " << MaxNumValues << "(you are trying to set to " << numValues << ")").str());

    this->content.numValues = htons(numValues);
}
} // namespace tcp
} // namespace modbus

#endif
