#ifndef MODBUS_WRITE_COIL_COMMAND_HPP
#define MODBUS_WRITE_COIL_COMMAND_HPP

class ModbusWriteCoilCommand : public ModbusCommand {
public:
                                    ModbusWriteCoilCommand();
    void                            exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                     getShortHelpText() const override;
    std::string                     getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    uint16_t                        m_address;
    bool                            m_value;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;
};


ModbusWriteCoilCommand::ModbusWriteCoilCommand() {
    namespace po = boost::program_options;

    m_options.add_options()
        ("address,a", po::value<uint16_t>(&m_address)->required(), "address of target coil")
        ("value,v", po::value<bool>(&m_value)->required(), "new value for target coil");

    m_positional_options.add("address", 1);
    m_positional_options.add("value", 1);
}


void ModbusWriteCoilCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    client.writeCoil(modbus::tcp::Address(m_address), m_value);
}


std::string ModbusWriteCoilCommand::getShortHelpText() const {
    return "Write a coil of the modbus device";
}


std::string ModbusWriteCoilCommand::getHelpText() const {
    std::stringstream ss;
    ss << getShortHelpText() << std::endl
       << "Usage:" << std::endl
       << "    writecoil <address> <value>" << std::endl
       << "Valid choices for <value> field are 'on|off', 'yes|no', '1|0' and 'true|false'" << std::endl;
    return ss.str();
}


#endif
