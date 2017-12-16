#ifndef MODBUS_CONNECT_COMMAND_HPP
#define MODBUS_CONNECT_COMMAND_HPP

class ModbusConnectCommand : public ModbusCommand {
public:
                                ModbusConnectCommand();

    void                        exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                 getShortHelpText() const override;
    std::string                 getHelpText() const override;
private:
    std::string                 m_address;
    uint16_t                    m_port;
    boost::program_options::options_description m_options;
};


ModbusConnectCommand::ModbusConnectCommand() :
    ModbusCommand()
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("address,a", po::value<std::string>(&m_address)->default_value("127.0.0.1"), "ip address of target device")
        ("port,p", po::value<uint16_t>(&m_port)->default_value(502), "tcp connection port");
}

void ModbusConnectCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    //po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::store(po::command_line_parser(args).options(m_options).run(), vm);
    po::notify(vm);

    client.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(m_address), m_port));
}


std::string ModbusConnectCommand::getShortHelpText() const {
    return "connect to a modbus device";
}


std::string ModbusConnectCommand::getHelpText() const {
    std::stringstream ss;
    ss << "Open connection to a modbus device." << std::endl
    << "Usage:" << std::endl
    << "    connect -a <ip address> -p <port>" << std::endl
    << m_options;
    return ss.str();
}



#endif
