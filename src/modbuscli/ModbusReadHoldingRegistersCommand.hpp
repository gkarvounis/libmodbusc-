#ifndef MODBUS_READ_HOLDING_REGISTERS_COMMAND_HPP
#define MODBUS_READ_HOLDING_REGISTERS_COMMAND_HPP

class ModbusReadHoldingRegistersCommand : public ModbusCommand {
public:
                                    ModbusReadHoldingRegistersCommand();
    void                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                     getShortHelpText() const override;
    std::string                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    uint16_t                        m_startAddress;
    uint16_t                        m_numRegs;
    std::size_t                     m_interval;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;

};


ModbusReadHoldingRegistersCommand::ModbusReadHoldingRegistersCommand() {
    namespace po = boost::program_options;

    m_options.add_options()
        ("startAddress,s", po::value<uint16_t>(&m_startAddress)->required(), "address of first coil to retrieve")
        ("numRegs,n", po::value<uint16_t>(&m_numRegs)->required(), "number of coils to retrieve (up to 2000 coils)")
        ("interval,i", po::value<std::size_t>(&m_interval)->default_value(0), "polling interval in msecs");

    m_positional_options.add("startAddress", 1);
    m_positional_options.add("numRegs", 1);
    m_positional_options.add("interval", 1);
}


void ModbusReadHoldingRegistersCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    if (m_interval == 0)
        client.readHoldingRegisters(modbus::tcp::Address(m_startAddress), modbus::tcp::NumRegs(m_numRegs));
    else {
        while (true) {
            client.readHoldingRegisters(modbus::tcp::Address(m_startAddress), modbus::tcp::NumRegs(m_numRegs));
            std::cout << std::endl;
            usleep(m_interval * 1000);
        }
    }
}


std::string ModbusReadHoldingRegistersCommand::getShortHelpText() const {
    return "Read a set of holding registers";
}


std::string ModbusReadHoldingRegistersCommand::getHelpText() const {
    std::stringstream ss;

    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    readholdingregs <start address> <number of registers> [<interval>]" << std::endl
       << "Last parameter, if present and non-zero, is interpreted as polling interval in msecs" << std::endl;

    return ss.str();
}

#endif
