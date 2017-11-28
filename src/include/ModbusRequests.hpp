#ifndef MODBUS_REQUESTS_HPP
#define MODBUS_REQUESTS_HPP

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include <arpa/inet.h>


namespace modbus {
namespace tcp {

template <FunctionCode code>
class ReadBitsReq {
public:
    static const FunctionCode functionCode = code;

                            ReadBitsReq(const Address& startAddr, const NumBits& numBits);
    const Address&          getStartAddress() const { return m_startAddress; }
    const NumBits&          getNumValues() const { return m_numBits; }

private:
    Address                 m_startAddress;
    NumBits                 m_numBits;
};


template <FunctionCode code>
class ReadRegistersReq {
public:
    static const FunctionCode functionCode = code;

                            ReadRegistersReq(const Address& startAddr, const NumRegs& numRegs);
    const Address&          getStartAddress() const { return m_startAddress; }
    const NumRegs&          getNumValues() const { return m_numRegs; }

private:
    Address                 m_startAddress;
    NumRegs                 m_numRegs;
};


template <FunctionCode code, typename T, int dummy>
class WriteSingleValue {
public:
    static const FunctionCode functionCode = code;

                            WriteSingleValue(const Address& addr, T value);
    const Address&          getAddress() const { return m_address; }
    uint16_t                getValue() const { return m_value; }
private:
    Address                 m_address;
    uint16_t                m_value;
};


using ReadCoilsReq = ReadBitsReq<FunctionCode::READ_COILS>;
using ReadDiscreteInputsReq = ReadBitsReq<FunctionCode::READ_DISCRETE_INPUTS>;
using ReadHoldingRegistersReq = ReadRegistersReq<FunctionCode::READ_HOLDING_REGISTERS>;
using ReadInputRegistersReq = ReadRegistersReq<FunctionCode::READ_INPUT_REGISTERS>;
using WriteSingleCoilReq = WriteSingleValue<FunctionCode::WRITE_COIL, bool, 0>;
using WriteSingleRegisterReq = WriteSingleValue<FunctionCode::WRITE_REGISTER, uint16_t, 0>;

//

template<>
WriteSingleValue<FunctionCode::WRITE_COIL, bool, 0>::WriteSingleValue(const Address& addr, bool value) :
    m_address(addr),
    m_value(value ? 0xFF00 : 0x0000)
{}


template<>
WriteSingleValue<FunctionCode::WRITE_REGISTER, uint16_t, 0>::WriteSingleValue(const Address& addr, uint16_t value) :
    m_address(addr),
    m_value(value)
{}


template <FunctionCode funcCode>
ReadBitsReq<funcCode>::ReadBitsReq(const Address& startAddr, const NumBits& numBits) :
    m_startAddress(startAddr),
    m_numBits(numBits)
{}


template <FunctionCode funcCode>
ReadRegistersReq<funcCode>::ReadRegistersReq(const Address& startAddr, const NumRegs& numRegs) :
    m_startAddress(startAddr),
    m_numRegs(numRegs)
{}

} // namespace tcp
} // namespace modbus

#endif // MODBUS_REQUESTS_HPP
