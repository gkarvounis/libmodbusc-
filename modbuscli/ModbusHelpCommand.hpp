#ifndef MODBUS_HELP_COMMAND_HPP
#define MODBUS_HELP_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <stdexcept>


struct ModbusCliHelpForCommand : std::runtime_error {
                                ModbusCliHelpForCommand(const std::string& cmd) : std::runtime_error(cmd) {}
};


struct ModbusCliHelpListCommands : std::runtime_error {
                                ModbusCliHelpListCommands() : std::runtime_error("") {}
};


class ModbusHelpCommand : public ModbusCommand {
public:
    inline                      ModbusHelpCommand();
    void                        exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                 getShortHelpText();

private:
    std::string                 m_cmd;
};


ModbusHelpCommand::ModbusHelpCommand() :
    ModbusCommand("help")
{
    namespace po = boost::program_options;

    m_description.add_options()
        ("cmd,c", po::value<std::string>(&m_cmd)->default_value(""), "show help for a command");
}


void ModbusHelpCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    ModbusCommand::exec(client, args);

    if (m_cmd == "")
        throw ModbusCliHelpListCommands();
    else
        throw ModbusCliHelpForCommand(m_cmd);
}


std::string ModbusHelpCommand::getShortHelpText() {
    return "Get list of available commands and help about each one";
}


#endif

