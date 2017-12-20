#include "OutputFormatter.hpp"
#include "JsonOutputFormatter.hpp"
#include "StandardOutputFormatter.hpp"
#include "ModbusClient.hpp"
#include "ModbusCommands.hpp"
#include "CommandLineOptions.hpp"

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>



int main(int argc, char** argv) {
    CommandLineOptions options;
    options.read(argc, argv);
    std::cout << "** Welcome to modbus cli **" << std::endl
              << "  Type 'help' to see the list of available commands" << std::endl
              << "  Type 'help <cmd>' to obtain more info about each command" << std::endl
              << std::endl;

    std::unique_ptr<OutputFormatter> out(nullptr);

    if (options.format == "std")
        out.reset(new StandardOutputFormatter());
    else if (options.format == "json")
        out.reset(new JsonOutputFormatter());
    else {
        std::cout << "Invalid value for format. Please specify 'std' or 'json'" << std::endl;
        exit(1);
    }

    ModbusClient client(modbus::tcp::UnitId(options.unitId), *out);
    ModbusCommands commands;

    if (!options.server_ip.empty()) {
        try {
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(options.server_ip), options.server_port);
            client.connect(ep);
            std::cout << "connected to " << options.server_ip << ":" << options.server_port << std::endl;
        } catch (const boost::system::system_error& ec) {
            std::cout << ec.what() << std::endl;
            exit(1);
        }
    }

    while (true) {
        std::cout << "> ";

        std::string line;
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
