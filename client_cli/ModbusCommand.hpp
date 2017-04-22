#ifndef MODBUS_COMMAND_HPP
#define MODBUS_COMMAND_HPP

#include "ModbusClient.hpp"

#include <boost/program_options.hpp>
#include <vector>


class ModbusCommand {
public:
    inline                      ModbusCommand(const std::string& cmd);
    inline virtual             ~ModbusCommand();
    inline const std::string&   getCommand() const;

    virtual void                exec(ModbusClient& client, const std::vector<std::string>& args);

protected:
    boost::program_options::options_description m_description;
    
private:
    std::string                 m_command;
    void                        parseArgs(const std::vector<std::string>& args);
};


ModbusCommand::ModbusCommand(const std::string& cmd) :
    m_description(cmd),
    m_command(cmd)
{}


ModbusCommand::~ModbusCommand() {
}


const std::string& ModbusCommand::getCommand() const {
    return m_command;
}


void ModbusCommand::parseArgs(const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_description).run(), vm);
    po::notify(vm);
}


void ModbusCommand::exec(ModbusClient& /*client*/, const std::vector<std::string>& args) {
    parseArgs(args);
}


#endif

