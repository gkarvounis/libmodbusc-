#ifndef MODBUS_EXIT_COMMAND_HPP
#define MODBUS_EXIT_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <stdexcept>


struct ModbusCliExit : public std::runtime_error {
    ModbusCliExit() : std::runtime_error("") {}
};


class ModbusExitCommand : public ModbusCommand {
public:
    inline                      ModbusExitCommand();
    inline void                 exec(ModbusClient& client, const std::vector<std::string>& args);

    std::string                 getShortHelpText() override;
    std::string                 getUsageText() override;
};


ModbusExitCommand::ModbusExitCommand() :
    ModbusCommand("exit")
{
    compose_options();
}


void ModbusExitCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    ModbusCommand::exec(client, args);
    throw ModbusCliExit();
}


std::string ModbusExitCommand::getShortHelpText() {
    return "Exit from the program";
}


std::string ModbusExitCommand::getUsageText() {
    return "exit";
}

#endif

