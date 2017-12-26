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
    inline                                          ModbusHelpCommand(const ModbusCommandsMap& commands);
    void                                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                                     getShortHelpText() const override;
    std::string                                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    void                                            show_commands_list() const;
    void                                            show_command_help() const;

    const ModbusCommandsMap                        &m_commands;
    std::string                                     m_cmd;
    OptionsDescription                              m_options;
    PositionalOptionsDescription                    m_positional_options;
};


ModbusHelpCommand::ModbusHelpCommand(const ModbusCommandsMap& commands) :
    ModbusCommand(),
    m_commands(commands),
    m_cmd(),
    m_options("options"),
    m_positional_options()
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("cmd", po::value<std::string>(&m_cmd)->default_value(""), "show help for a command");

    m_positional_options.add("cmd", 1);
}


void ModbusHelpCommand::exec(ModbusClient& /*client*/, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    if (m_cmd == "")
        show_commands_list();
    else
        show_command_help();
}


void ModbusHelpCommand::show_commands_list() const {
    std::cout << "List of available commands:" << std::endl;

    for (const auto& kv: m_commands)
        std::cout << "    " << std::setw(15) << kv.first << " - " << kv.second->getShortHelpText() << std::endl;

    std::cout << "Type 'help <cmd>' to see details for each command." << std::endl;
}


void ModbusHelpCommand::show_command_help() const {
    auto cmdobj = m_commands.find(m_cmd);

    if (cmdobj == m_commands.end())
        std::cout << "No such command: " << m_cmd << std::endl;
    else
        std::cout << cmdobj->second->getHelpText() << std::endl;
}


std::string ModbusHelpCommand::getShortHelpText() const {
    return "Get list of available commands and help about each one";
}


std::string ModbusHelpCommand::getHelpText() const {
    std::stringstream ss;

    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    help [<cmd>]" << std::endl
       << "If no command is given, a list of available commands is printed." << std::endl;

    return ss.str();
}

#endif

