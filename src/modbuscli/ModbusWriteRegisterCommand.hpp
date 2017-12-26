#ifndef MODBUS_WRITE_REGISTER_COMMAND_HPP
#define MODBUS_WRITE_REGISTER_COMMAND_HPP

class ModbusWriteRegisterCommand : public ModbusCommand {
public:
                                    ModbusWriteRegisterCommand();
    void                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                     getShortHelpText() const override;
    std::string                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    uint16_t                        m_address;
    uint16_t                        m_value;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;
};


ModbusWriteRegisterCommand::ModbusWriteRegisterCommand() {
    namespace po = boost::program_options;

    m_options.add_options()
        ("address,a", po::value<uint16_t>(&m_address)->required(), "address of target register");
        ("value,v", po::value<uint16_t>(&m_value)->required(), "new value for target register");

    m_positional_options.add("address", 1);
    m_positional_options.add("value", 1);
}


void ModbusWriteRegisterCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    client.writeRegister(modbus::tcp::Address(m_address), m_value);
}


std::string ModbusWriteRegisterCommand::getShortHelpText() const {
    return "Write a register of the modbus device";
}


std::string ModbusWriteRegisterCommand::getHelpText() const {
    std::stringstream ss;
    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    writeregister <address> <value>" << std::endl;
    return ss.str();
}


#endif
