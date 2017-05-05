#ifndef MODBUS_COMMAND_HPP
#define MODBUS_COMMAND_HPP

#include "ModbusClient.hpp"

#include <boost/program_options.hpp>
#include <vector>
#include <iostream>


class ModbusCommand {
public:
    inline                                                  ModbusCommand(const std::string& cmd);
    inline virtual                                         ~ModbusCommand();
    inline const std::string&                               getCommand() const;

    virtual void                                            exec(ModbusClient& client, const std::vector<std::string>& args);
    virtual std::string                                     getShortHelpText() = 0;
    virtual std::string                                     getUsageText() = 0;
    virtual void                                            printHelp();

protected:
    boost::program_options::options_description             m_visible_options;
    boost::program_options::options_description             m_hidden_options;
    boost::program_options::positional_options_description  m_positional_options;
    void                                                    compose_options();

private:
    boost::program_options::options_description             m_options;
    std::string                                             m_command;
    void                                                    parseArgs(const std::vector<std::string>& args);
};


ModbusCommand::ModbusCommand(const std::string& cmd) :
    m_visible_options("Options"),
    m_hidden_options(""),
    m_positional_options(),
    m_command(cmd)
{}


ModbusCommand::~ModbusCommand() {
}


const std::string& ModbusCommand::getCommand() const {
    return m_command;
}


void ModbusCommand::exec(ModbusClient& /*client*/, const std::vector<std::string>& args) {
    parseArgs(args);
}


void ModbusCommand::compose_options() {
    m_options.add(m_visible_options);
    m_options.add(m_hidden_options);
}


void ModbusCommand::parseArgs(const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);
}


void ModbusCommand::printHelp() {
    std::cout << "Usage: " << getUsageText() << std::endl;

    if (!m_visible_options.options().empty())
        std::cout << m_visible_options << std::endl;
}

#endif

