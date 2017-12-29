#ifndef MODBUS_FORMAT_COMMAND_HPP
#define MODBUS_FORMAT_COMMAND_HPP

class ModbusFormatCommand : public ModbusCommand {
public:
                                   ModbusFormatCommand();

    void                           exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                    getShortHelpText() const override;
    std::string                    getHelpText() const override;

private:
    using OptionsDescription = boost::program_options::options_description;
    using PositionalOptionsDescription = boost::program_options::positional_options_description;

    std::string                     m_format;

    OptionsDescription              m_options;
    PositionalOptionsDescription    m_positional_options;
};


ModbusFormatCommand::ModbusFormatCommand()
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("format,f", po::value<std::string>(&m_format)->default_value("std"), "possible values are std|json|std_verbose");

    m_positional_options.add("format", 1);
}


void ModbusFormatCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::notify(vm);

    if (m_format == "std") {
        client.setFormatter(std::unique_ptr<OutputFormatter>(new StandardOutputFormatter(std::cout)));
    } else if (m_format == "json") {
        client.setFormatter(std::unique_ptr<OutputFormatter>(new JsonOutputFormatter(std::cout)));
    } else if (m_format == "std_verbose") {
        client.setFormatter(std::unique_ptr<OutputFormatter>(new VerboseStandardOutputFormatter(std::cout)));
    } else {
        std::cout << "Invalid format '" << m_format <<"' specified. Valid options are std, json or std_verbose" << std::endl;
    }
}


std::string ModbusFormatCommand::getShortHelpText() const {
    return "set output format";
}


std::string ModbusFormatCommand::getHelpText() const {
    std::stringstream ss;
    ss << getShortHelpText() << std::endl
       << "Usage:" 
       << "    format std | json | std_verbose";

    return ss.str();
}

#endif

