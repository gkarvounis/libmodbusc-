#include "ModbusEncoder.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>

struct Options {
    std::string server_ip;
    uint16_t    server_port;
} options;


void read_options(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description descr("Options");
    descr.add_options()
        ("help", "help")
        ("host,h", po::value<std::string>(&options.server_ip)->default_value("127.0.0.1"), "device ip address")
        ("port,p", po::value<uint16_t>(&options.server_port)->default_value(502), "device port");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, descr), vm);
    po::notify(vm);

    if (vm.count("help")) {
       std::cout << descr << std::endl;
       exit(0);
    }
}


struct NoSuchCommand : public std::runtime_error {
    NoSuchCommand(const std::string& msg) : std::runtime_error(msg) {}
};


void handle_get_coils(const std::vector<std::string>& args) {
    struct CmdOptions {
        uint16_t startAddress;
        uint16_t numCoils;
    } cmd_options;

    namespace po = boost::program_options;
    po::options_description descr("get coils");
    descr.add_options()
        ("startAddr,s", po::value<uint16_t>(&cmd_options.startAddress)->required(), "address of first coil to retrieve")
        ("numCoils,n", po::value<uint16_t>(&cmd_options.numCoils)->required(), "number of coils to retrieve (up to 2000 coils)");

    std::cout << cmd_options.startAddress << " " << cmd_options.numCoils << std::endl;

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(descr).run(), vm);
    po::notify(vm);
}


void handle_cmd(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "getcoils") {
        handle_get_coils(args);
    } else
        throw NoSuchCommand(cmd);
}


void parse_cmd_line(const std::string& line, std::string& cmd, std::vector<std::string>& args) {
    namespace po = boost::program_options;

    auto pos = line.find(" ");
    cmd = line.substr(0, pos);
    std::string remainder(line.begin()+pos+1, line.end());
    args = po::split_unix(remainder);
}


int main(int argc, char** argv) {
    read_options(argc, argv);

    while (true) {
        std::string line;
        std::string cmd;
        std::vector<std::string> args;
        std::cout << "> ";
        std::getline(std::cin, line);

        boost::trim(line);
        if (line == "")
            continue;

        parse_cmd_line(line, cmd, args);

        try {
            handle_cmd(cmd, args);
        } catch (const NoSuchCommand& ex) {
            std::cout << "No such command" << std::endl;
        }
    }
}
