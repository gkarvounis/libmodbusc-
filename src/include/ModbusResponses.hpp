#ifndef MODBUS_RESPONSES_HPP
#define MODBUS_RESPONSES_HPP

namespace modbus {
namespace tcp {

using WriteSingleCoilRsp = WriteSingleValue<FunctionCode::WRITE_COIL, bool, 1>;
using WriteSingleRegisterRsp = WriteSingleValue<FunctionCode::WRITE_REGISTER, uint16_t, 1>;

//

template<>
WriteSingleValue<FunctionCode::WRITE_COIL, bool, 1>::WriteSingleValue(const Address& addr, bool value) :
    m_address(addr),
    m_value(value ? 0xFF00 : 0x0000)
{}


template<>
WriteSingleValue<FunctionCode::WRITE_REGISTER, uint16_t, 1>::WriteSingleValue(const Address& addr, uint16_t value) :
    m_address(addr),
    m_value(value)
{}

} // namespace tcp
} // namespace modbus

#endif
