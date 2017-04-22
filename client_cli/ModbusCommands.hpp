#ifndef MODBUS_COMMANDS_HPP
#define MODBUS_COMMANDS_HPP

#include "ModbusCommand.hpp"
#include <map>
#include <memory>

#include "ModbusReadCoilsCommand.hpp"


struct NoSuchCommand : public std::runtime_error {
    NoSuchCommand(const std::string& msg) : std::runtime_error(msg) {}
};


class ModbusCommands {
public:
    inline                          ModbusCommands();
    inline void                     exec(ModbusClient& client, const std::string& line);
private:
    using  CommandsMap = std::map<std::string, std::shared_ptr<ModbusCommand>>;

    static void                     parse_cmd_line(const std::string& line, std::string& cmd, std::vector<std::string>& args);
    CommandsMap                     m_commands;
};


ModbusCommands::ModbusCommands() :
    m_commands()
{
    auto ptr = std::make_shared<ReadCoilsCommand>();
    m_commands[ptr->getCommand()] = ptr;
}


void ModbusCommands::exec(ModbusClient& client, const std::string& line) {
    std::string cmd;
    std::vector<std::string> args;
    parse_cmd_line(line, cmd, args);

    auto cmdobj = m_commands.find(cmd);

    if (cmdobj == m_commands.end())
        throw NoSuchCommand(cmd);

    cmdobj->second->exec(client, args);
}


void ModbusCommands::parse_cmd_line(const std::string& line, std::string& cmd, std::vector<std::string>& args) {
    namespace po = boost::program_options;

    auto pos = line.find(" ");
    cmd = line.substr(0, pos);
    std::string remainder(line.begin()+pos+1, line.end());
    args = po::split_unix(remainder);
}
#endif

