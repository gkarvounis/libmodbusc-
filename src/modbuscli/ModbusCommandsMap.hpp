#ifndef MODBUS_COMMANDS_MAP_HPP
#define MODBUS_COMMANDS_MAP_HPP

#include <map>
#include <memory>
#include "ModbusCommand.hpp"

using ModbusCommandsMap = std::map<std::string, std::shared_ptr<ModbusCommand>>;

#endif

