#ifndef MODBUS_READ_DISCRETE_INPUTS_COMMAND_HPP
#define MODBUS_READ_DISCRETE_INPUTS_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <iostream>


class ReadDiscreteInputs : public ModbusCommand {
public:
    inline                          ReadDiscreteInputs();
    void                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                     getShortHelpText() const override;
    std::string                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    uint16_t                        m_startAddress;
    uint16_t                        m_numCoils;
    std::size_t                     m_interval;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;
};


ReadDiscreteInputs::ReadDiscreteInputs() :
    ModbusCommand(),
    m_options("Options")
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("startAddress,s", po::value<uint16_t>(&m_startAddress)->required(), "address of first coil to retrieve")
        ("numInputs,n", po::value<uint16_t>(&m_numCoils)->required(), "number of inputs to retrieve (up to 2000 coils)")
        ("interval,i", po::value<std::size_t>(&m_interval)->default_value(0), "polling interval in msecs. Set to 0 turns off polling");

    m_positional_options.add("startAddress", 1);
    m_positional_options.add("numInputs", 1);
    m_positional_options.add("interval", 1);
}


void ReadDiscreteInputs::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    if (m_interval == 0)
        client.readDiscreteInputs(modbus::tcp::Address(m_startAddress), modbus::tcp::NumBits(m_numCoils));
    else {
        while (true) {
            client.readDiscreteInputs(modbus::tcp::Address(m_startAddress), modbus::tcp::NumBits(m_numCoils));
            std::cout << std::endl;
            usleep(m_interval*1000);
        }
    }
}


std::string ReadDiscreteInputs::getShortHelpText() const {
    return "Read a list of discrete inputs from modbus device";
}


std::string ReadDiscreteInputs::getHelpText() const {
    std::stringstream ss;
    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    readcoils <start_address> <num_inputs> [<interval>]" << std::endl
       << "Last parameter, if present and non-zero, is interpreted as polling interval in msecs" << std::endl;
    return ss.str();
}

#endif
