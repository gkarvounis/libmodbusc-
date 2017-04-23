#ifndef MODBUS_COMMANDS_HPP
#define MODBUS_COMMANDS_HPP

#include "ModbusCommand.hpp"
#include "ModbusReadCoilsCommand.hpp"
#include "ModbusHelpCommand.hpp"
#include "ModbusExitCommand.hpp"

#include <map>
#include <memory>
#include <iomanip>


struct NoSuchCommand : public std::runtime_error {
    NoSuchCommand(const std::string& msg) : std::runtime_error(msg) {}
};


class ModbusCommands {
public:
    inline                          ModbusCommands();
    inline void                     exec(ModbusClient& client, const std::string& line);

private:
    using  CommandsMap = std::map<std::string, std::shared_ptr<ModbusCommand>>;

    inline void                     show_commands_list();
    inline void                     show_command_help(const std::string& cmd);
    static void                     parse_cmd_line(const std::string& line, std::string& cmd, std::vector<std::string>& args);
    CommandsMap                     m_commands;
};


ModbusCommands::ModbusCommands() :
    m_commands()
{
    auto ptr1 = std::make_shared<ReadCoilsCommand>();
    m_commands[ptr1->getCommand()] = ptr1;

    auto ptr2 = std::make_shared<ModbusHelpCommand>();
    m_commands[ptr2->getCommand()] = ptr2;

    auto ptr3 = std::make_shared<ModbusExitCommand>();
    m_commands[ptr3->getCommand()] = ptr3;
}


void ModbusCommands::exec(ModbusClient& client, const std::string& line) {
    std::string cmd;
    std::vector<std::string> args;
    parse_cmd_line(line, cmd, args);

    auto cmdobj = m_commands.find(cmd);

    if (cmdobj == m_commands.end())
        throw NoSuchCommand(cmd);

    try {
        cmdobj->second->exec(client, args);
    } catch (const NoSuchCommand& ex) {
        std::cout << "No such command" << std::endl;
    } catch (const ModbusCliHelp& ex) {
        std::string cmd(ex.what());

        if (cmd.empty())
           show_commands_list();
        else
            show_command_help(ex.what());
    }
}


void ModbusCommands::show_commands_list() {
    std::cout << "List of available commands:" << std::endl;

    for (const auto& kv: m_commands)
        std::cout << "    " << std::setw(10) << kv.second->getCommand() << " - " << kv.second->getShortHelpText() << std::endl;
}


void ModbusCommands::show_command_help(const std::string& cmd) {
}


void ModbusCommands::parse_cmd_line(const std::string& line, std::string& cmd, std::vector<std::string>& args) {
    namespace po = boost::program_options;

    auto pos = line.find(" ");
    cmd = line.substr(0, pos);
    std::string remainder(line.begin()+pos+1, line.end());
    args = po::split_unix(remainder);
}
#endif

