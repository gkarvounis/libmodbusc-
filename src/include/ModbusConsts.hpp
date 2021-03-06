#ifndef MODBUS_CONSTS_HPP
#define MODBUS_CONSTS_HPP

#include <cstdint>

namespace modbus {
namespace tcp {

static const uint16_t   MODBUS_PROTOCOL_ID = 0;
static const uint16_t   MODBUS_MAX_NUM_BITS_IN_READ_REQUEST = 0x07D0;
static const uint8_t    MODBUS_MAX_NUM_REGS_IN_READ_REQUEST = 0x007D;
static const uint16_t   MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST = 0x07D0;
static const uint8_t    MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST = 0x007D;


enum class FunctionCode : uint8_t {
    READ_COILS = 0x01,
    READ_DISCRETE_INPUTS = 0x02,
    READ_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    WRITE_COIL = 0x05,
    WRITE_REGISTER = 0x06,
    WRITE_COILS = 0x0F,
    WRITE_REGISTERS = 0x10
};


enum class ExceptionCode : uint8_t {
    ILLEGAL_FUNCTION = 0x01,
    ILLEGAL_DATA_ADDRESS = 0x02,
    ILLEGAL_DATA_VALUE = 0x03,
    SLAVE_DEVICE_FAILURE = 0x04,
    ACKNOWLEDGE = 0x05,
    SLAVE_DEVICE_BUSY = 0x06,
    MEMORY_PARITY_ERROR = 0x08,
    GATEWAY_PATH_UNAVAILABLE = 0x0A,
    GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B
};

}; // tcp
}; // modbus

#endif

