#ifndef MODBUS_READ_COILS_COMMAND_HPP
#define MODBUS_READ_COILS_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <iostream>


class ReadCoilsCommand : public ModbusCommand {
public:
    inline                      ReadCoilsCommand();
    void                        exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                 getShortHelpText() const override;
    std::string                 getHelpText() const override;

private:
    uint16_t                    m_startAddress;
    uint16_t                    m_numCoils;

    boost::program_options::options_description m_options;
};


ReadCoilsCommand::ReadCoilsCommand() :
    ModbusCommand(),
    m_options("Options")
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("startAddress,s", po::value<uint16_t>(&m_startAddress)->required(), "address of first coil to retrieve")
        ("numCoils,n", po::value<uint16_t>(&m_numCoils)->required(), "number of coils to retrieve (up to 2000 coils)");
}


void ReadCoilsCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    //po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::store(po::command_line_parser(args).options(m_options).run(), vm);
    po::notify(vm);

    client.readCoils(modbus::tcp::Address(m_startAddress), modbus::tcp::NumBits(m_numCoils));
}


std::string ReadCoilsCommand::getShortHelpText() const {
    return "Read a list of coils from modbus device";
}


std::string ReadCoilsCommand::getHelpText() const {
    return "getcoils [options]";
}

#endif
