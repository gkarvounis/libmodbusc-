#ifndef MODBUS_READ_COILS_COMMAND_HPP
#define MODBUS_READ_COILS_COMMAND_HPP

#include "ModbusCommand.hpp"
#include <iostream>


class ReadCoilsCommand : public ModbusCommand {
public:
    inline                      ReadCoilsCommand();
    void                        exec(ModbusClient& client, const std::vector<std::string>& args) override;
    std::string                 getShortHelpText() const override;
    std::string                 getHelpText() const override;

private:
    uint16_t                    m_startAddress;
    uint16_t                    m_numCoils;

    // void                        printResult(modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp);
    boost::program_options::options_description m_options;
};


ReadCoilsCommand::ReadCoilsCommand() :
    ModbusCommand(),
    m_options("Options")
{
    namespace po = boost::program_options;

    m_options.add_options()
        ("startAddress,s", po::value<uint16_t>(&m_startAddress)->required(), "address of first coil to retrieve")
        ("numCoils,n", po::value<uint16_t>(&m_numCoils)->required(), "number of coils to retrieve (up to 2000 coils)");
}


void ReadCoilsCommand::exec(ModbusClient& client, const std::vector<std::string>& args) {
    namespace po = boost::program_options;

    po::variables_map vm;
    //po::store(po::command_line_parser(args).options(m_options).positional(m_positional_options).run(), vm);
    po::store(po::command_line_parser(args).options(m_options).run(), vm);
    po::notify(vm);

    client.readCoils(modbus::tcp::Address(m_startAddress), modbus::tcp::NumBits(m_numCoils));
/*

    printResult(buf);
*/
}


//void ReadCoilsCommand::printResult(modbus::tcp::encoder::ReadCoilsRsp::Buffer& rsp) {
/*
    std::cout << "unitId:" << static_cast<unsigned>(rsp.header.unitId) << std::endl
              << "transactionId: " << ntohs(rsp.header.transactionId) << std::endl;


    std::size_t numRows = cmd_options.numCoils / 8;
    if (cmd_options.numCoils % 8 != 0)
        numRows++;

    uint16_t coil = 0;

    for (std::size_t row = 0; row < numRows; ++row) {
        uint16_t fromCoil = cmd_options.startAddress + row * 8;
        uint16_t toCoil = std::min(fromCoil + 7, cmd_options.numCoils + cmd_options.startAddress - 1);

        std::cout << "coils " << fromCoil << " to " << toCoil << ": ";

        for (unsigned i = fromCoil; i <= toCoil; ++i) {
            uint8_t c = rsp.values[coil/8];
            uint8_t m = 1 << (coil%8);
            std::cout << (c & m ? 1 : 0) << ' ';
            coil++;
        }

        std::cout << std::endl;
    }
*/
//}


std::string ReadCoilsCommand::getShortHelpText() const {
    return "Read a list of coils from modbus device";
}


std::string ReadCoilsCommand::getHelpText() const {
    return "getcoils [options]";
}

#endif
