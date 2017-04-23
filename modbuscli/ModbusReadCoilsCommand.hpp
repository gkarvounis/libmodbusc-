#ifndef MODBUS_READ_COILS_COMMAND_HPP
#define MODBUS_READ_COILS_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <iostream>


class ReadCoilsCommand : public ModbusCommand {
public:
    inline                      ReadCoilsCommand();
    void                        exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                 getShortHelpText() override;
    void                        printHelp() override;

private:
    struct CmdOptions {
        uint16_t startAddress;
        uint16_t numCoils;
    } cmd_options;

    void                        printResult(modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp);
};


ReadCoilsCommand::ReadCoilsCommand() :
    ModbusCommand("getcoils"),
    cmd_options()
{
    namespace po = boost::program_options;

    m_description.add_options()
        ("startAddr,s", po::value<uint16_t>(&cmd_options.startAddress)->required(), "address of first coil to retrieve")
        ("numCoils,n", po::value<uint16_t>(&cmd_options.numCoils)->required(), "number of coils to retrieve (up to 2000 coils)");
}


void ReadCoilsCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    ModbusCommand::exec(client, args);

    modbus::tcp::encoder::ReadCoilsRsp::Buffer buf;

    try {
        client.readCoils(cmd_options.startAddress, cmd_options.numCoils, buf);
    } catch (const ModbusErrorRsp& ex) {
        std::cout << "Device responded with error: " + std::string(ex.what()) << std::endl;
        return;
    } catch (const std::out_of_range& ex) {
        std::cout << ex.what() << std::endl;
        return;
    }

    printResult(buf);
}


void ReadCoilsCommand::printResult(modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp) {
    std::cout << "unitId:" << (unsigned)rsp.header.unitId << std::endl
              << "transactionId: " << ntohs(rsp.header.transactionId) << std::endl;

    std::size_t numRows = cmd_options.numCoils / 8;
    if (cmd_options.numCoils % 8 != 0)
        numRows++;

    uint16_t coil = 0;

    for (std::size_t row = 0; row < numRows; ++row) {
        uint16_t fromCoil = cmd_options.startAddress + row * 8;
        uint16_t toCoil = std::min(fromCoil + 7, cmd_options.numCoils + cmd_options.startAddress - 1);

        std::cout << "coils " << fromCoil << " to " << toCoil << ": ";

        for (unsigned i = 0; i < toCoil-fromCoil+1; ++i) {
            uint8_t c = rsp.values[coil/8];
            uint8_t m = 1 << (coil%8);
            std::cout << (c & m ? 1 : 0) << ' ';
            coil++;
        }

        std::cout << std::endl;
    }
}


std::string ReadCoilsCommand::getShortHelpText() {
    return "Read a list of coils from modbus device";
}


void ReadCoilsCommand::printHelp() {
}


#endif

