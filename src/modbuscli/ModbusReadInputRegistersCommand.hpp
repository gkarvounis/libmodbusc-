#ifndef MODBUS_READ_INPUT_REGISTERS_COMMAND_HPP
#define MODBUS_READ_INPUT_REGISTERS_COMMAND_HPP

class ModbusReadInputRegistersCommand : public ModbusCommand {
public:
                                    ModbusReadInputRegistersCommand();
    void                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                     getShortHelpText() const override;
    std::string                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    uint16_t                        m_startAddress;
    uint16_t                        m_numRegs;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;

};


ModbusReadInputRegistersCommand::ModbusReadInputRegistersCommand() {
    namespace po = boost::program_options;

    m_options.add_options()
        ("startAddress,s", po::value<uint16_t>(&m_startAddress)->required(), "address of first coil to retrieve")
        ("numRegs,n", po::value<uint16_t>(&m_numRegs)->required(), "number of coils to retrieve (up to 2000 coils)");

    m_positional_options.add("startAddress", 1);
    m_positional_options.add("numRegs", 1);
}


void ModbusReadInputRegistersCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    client.readInputRegisters(modbus::tcp::Address(m_startAddress), modbus::tcp::NumRegs(m_numRegs));
}


std::string ModbusReadInputRegistersCommand::getShortHelpText() const {
    return "Read a set of input registers";
}


std::string ModbusReadInputRegistersCommand::getHelpText() const {
    std::stringstream ss;

    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    readinputregs <start address> <number of registers>" << std::endl;

    return ss.str();
}

#endif
