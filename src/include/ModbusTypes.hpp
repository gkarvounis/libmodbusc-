#ifndef MODBUS_TYPES_HPP
#define MODBUS_TYPES_HPP

#include <stdexcept>


namespace modbus {
namespace tcp {

struct error : public std::runtime_error {
    error(const std::string& msg) : std::runtime_error(msg) {}
};


struct NumBitsOutOfRange : public error {
    NumBitsOutOfRange(const std::string& msg) : error(msg) {}
};


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

struct WriteCoilsReq {
    Header                          header;
    uint16_t                        startAddress;
    uint16_t                        numBits;
    uint8_t                         numBytes;
    uint8_t                         coils[];
} __attribute__((packed));

struct WriteRegistersReq {
    Header                          header;
    uint16_t                        startAddress;
    uint16_t                        numRegs;
    uint8_t                         numBytes;
    uint16_t                        regs[];
} __attribute__((packed));

struct WriteValuesRsp {
    Header                          header;
    uint16_t                        startAddress;
    uint16_t                        numValues;
} __attribute__((packed));

struct ExceptionRsp {
    Header                          header;
    uint8_t                         code;
} __attribute__((packed));


class UnitId {
public:
    inline explicit                     UnitId(uint8_t unitId=0) : m_unitId(unitId) {}
    inline uint8_t                      get() const { return m_unitId; }
    inline void                         set(uint8_t unitId) { m_unitId = unitId; }
    inline bool                         operator==(const UnitId& other) { return m_unitId == other.m_unitId; }
    inline bool                         operator!=(const UnitId& other) { return !(m_unitId == other.m_unitId); }
private:
    uint8_t                             m_unitId;
};


class TransactionId {
public:
    inline explicit                     TransactionId(uint16_t transactionId=0) : m_transactionId(transactionId) {}
    inline uint16_t                     get() const { return m_transactionId; }
    inline void                         set(uint16_t transactionId) { m_transactionId = transactionId; }
    inline bool                         operator==(const TransactionId& other) { return m_transactionId == other.m_transactionId; }
private:
    uint16_t                            m_transactionId;
};


class Address {
public:
    inline explicit                     Address(uint16_t addr=0) : m_address(addr) {}
    inline uint16_t                     get() const { return m_address; }
    inline void                         set(uint16_t address) { m_address = address; }
    inline bool                         operator==(const Address& other) const { return m_address == other.m_address; }
private:
    uint16_t                            m_address;
};


class NumBits {
public:
    inline explicit                     NumBits(uint16_t numBits=1);
    inline uint16_t                     get() const { return m_numBits; }
    inline void                         set(uint16_t numBits);
    inline bool                         operator==(const NumBits& other) { return m_numBits == other.m_numBits; }
private:
    uint16_t                            m_numBits;
    inline static void                  check(uint16_t numBits);
};


class NumRegs {
public:
    inline explicit                     NumRegs(uint8_t numRegs=1);
    inline uint16_t                     get() const { return m_numRegs; }
    inline void                         set(uint16_t numRegs);
    inline bool                         operator==(const NumRegs& other) { return m_numRegs == other.m_numRegs; }
private:
    uint16_t                            m_numRegs;
    inline static void                  check(uint16_t numRegs);
};


NumBits::NumBits(uint16_t numBits) :
    m_numBits(numBits)
{
    check(numBits);
}

void NumBits::set(uint16_t numBits) {
    check(numBits);
    m_numBits = numBits;
}

void NumBits::check(uint16_t numBits) {
    if (numBits == 0)
        throw NumBitsOutOfRange("Cannot submit request for 0 bits");

    if (numBits > MODBUS_MAX_NUM_BITS_IN_READ_REQUEST)
        throw NumBitsOutOfRange("max number of bits that can be requested is MODBUS_MAX_NUM_BITS_IN_READ_REQUEST=0x07D0");
}


NumRegs::NumRegs(uint8_t numRegs) :
    m_numRegs(numRegs)
{
    check(numRegs);
}


void NumRegs::set(uint16_t numRegs) {
    check(numRegs);
    m_numRegs = numRegs;
}


void NumRegs::check(uint16_t numRegs) {
    if (numRegs == 0)
        throw std::logic_error("Cannot submit request for 0 registers");

    if (numRegs > MODBUS_MAX_NUM_REGS_IN_READ_REQUEST)
        throw std::logic_error("max number of coils that can be requested is MODBUS_MAX_NUM_REGS_IN_READ_REQUEST=0x007D");
}

} // namespace tcp
} // namespace modbus

#endif
