#ifndef COMMAND_LINE_OPTIONS_HPP
#define COMMAND_LINE_OPTIONS_HPP

#include <boost/program_options.hpp>

struct CommandLineOptions {
    std::string     server_ip;
    uint16_t        server_port;
    uint16_t        unitId;

    inline void     read(int argc, char** argv);
};


void CommandLineOptions::read(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description descr("Options");
    descr.add_options()
        ("help", "help")
        ("host,h", po::value<std::string>(&server_ip)->default_value(""), "device ip address")
        ("port,p", po::value<uint16_t>(&server_port)->default_value(502), "device port")
        ("unitId,u", po::value<uint16_t>(&unitId)->default_value(1), "modbus device id");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, descr), vm);
    po::notify(vm);

    if (vm.count("help")) {
       std::cout << descr << std::endl;
       exit(0);
    }
}

#endif
