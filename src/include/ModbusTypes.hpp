#ifndef MODBUS_TYPES_HPP
#define MODBUS_TYPES_HPP

#include <stdexcept>


namespace modbus {
namespace tcp {

class UnitId {
public:
    inline explicit                     UnitId(uint8_t unitId) : m_unitId(unitId) {}
    inline uint8_t                      get() const { return m_unitId; }
private:
    uint8_t                             m_unitId;
};


class TransactionId {
public:
    inline explicit                     TransactionId(uint16_t transactionId) : m_transactionId(transactionId) {}
    inline uint16_t                     get() const { return m_transactionId; }
private:
    uint16_t                            m_transactionId;
};


class Address {
public:
    inline explicit                     Address(uint16_t addr) : m_address(addr) {}
    inline uint16_t                     get() const { return m_address; }
private:
    uint16_t                            m_address;
};


class NumBits {
public:
    inline explicit                     NumBits(uint16_t numBits);
    inline uint16_t                     get() const { return m_numBits; }
private:
    uint16_t                            m_numBits;
};


class NumRegs {
public:
    inline explicit                     NumRegs(uint8_t numRegs);
    inline uint16_t                     get() const { return m_numRegs; }
private:
    uint16_t                            m_numRegs;
};


NumBits::NumBits(uint16_t numBits) :
    m_numBits(numBits)
{
    if (numBits == 0)
        throw std::logic_error("Cannot submit request for 0 bits");

    if (numBits > MODBUS_MAX_NUM_BITS_IN_READ_REQUEST)
        throw std::logic_error("max number of bits that can be requested is MODBUS_MAX_NUM_BITS_IN_READ_REQUEST=0x07D0");
}


NumRegs::NumRegs(uint8_t numRegs) :
    m_numRegs(numRegs)
{
    if (numRegs == 0)
        throw std::logic_error("Cannot submit request for 0 registers");

    if (numRegs > MODBUS_MAX_NUM_REGS_IN_READ_REQUEST)
        throw std::logic_error("max number of coils that can be requested is MODBUS_MAX_NUM_REGS_IN_READ_REQUEST=0x007D");
}


} // namespace tcp
} // namespace modbus

#endif
