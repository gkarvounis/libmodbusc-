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
    void                        printHelp() override;
};


ModbusExitCommand::ModbusExitCommand() :
    ModbusCommand("exit")
{}


void ModbusExitCommand::exec(ModbusClient& /*client*/, const std::vector<std::string>& /*args*/) {
    throw ModbusCliExit();
}


std::string ModbusExitCommand::getShortHelpText() {
    return "Exit from the program";
}


void ModbusExitCommand::printHelp() {
    std::cout << "Would print detailed help about exit" << std::endl;
}

#endif

