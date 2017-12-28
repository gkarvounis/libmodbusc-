#ifndef MODBUS_COMMAND_HPP
#define MODBUS_COMMAND_HPP


#include <boost/program_options.hpp>
#include <vector>
#include <iostream>
#include <iomanip>


class ModbusCommand {
public:
    inline                                                  ModbusCommand();
    inline virtual                                         ~ModbusCommand();

    virtual void                                            exec(ModbusClient& client, const std::vector<std::string>& args) = 0;
    virtual std::string                                     getShortHelpText() const = 0;
    virtual std::string                                     getHelpText() const = 0;
};


ModbusCommand::ModbusCommand() {
}


ModbusCommand::~ModbusCommand() {
}

#endif
