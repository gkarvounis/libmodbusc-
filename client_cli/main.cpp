#include "ModbusClient.hpp"
#include "ModbusCommands.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>


struct Options {
    std::string server_ip;
    uint16_t    server_port;
    uint16_t    unitId;
} options;


void read_options(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description descr("Options");
    descr.add_options()
        ("help", "help")
        ("host,h", po::value<std::string>(&options.server_ip)->default_value("127.0.0.1"), "device ip address")
        ("port,p", po::value<uint16_t>(&options.server_port)->default_value(502), "device port")
        ("unitId,u", po::value<uint16_t>(&options.unitId)->default_value(1), "modbus device id");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, descr), vm);
    po::notify(vm);

    if (vm.count("help")) {
       std::cout << descr << std::endl;
       exit(0);
    }
}


int main(int argc, char** argv) {
    read_options(argc, argv);
    std::cout << std::endl << std::endl
              << "Welcome to modbus cli" << std::endl
              << "  Type 'help' to see the list of available commands" << std::endl
              << "  Type 'help <cmd>' to see the usage of each command" << std::endl
              << std::endl;

    ModbusClient client(options.unitId);
    ModbusCommands commands;

    try {
        client.connect(options.server_ip, options.server_port);
    } catch (const boost::system::system_error& ec) {
        std::cout << ec.what() << std::endl;
        exit(1);
    }

    while (true) {
        std::string line;
        std::string cmd;
        std::cout << "> ";
        std::getline(std::cin, line);

        boost::trim(line);
        if (line == "")
            continue;
        
        try {
            commands.exec(client, line);
        } catch (const ModbusCliExit& ex) {
            break;
        } 
    }
}

