#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include "OutputFormatter.hpp"
#include "VerboseStandardOutputFormatter.hpp"
#include "ModbusClient.hpp"
#include "ModbusCommand.hpp"
#include "ModbusReadCoilsCommand.hpp"

#include <thread>

TEST_CASE("readcoils", "[cli]") {
    ReadCoilsCommand cmd;
    std::unique_ptr<OutputFormatter> fmt(new VerboseStandardOutputFormatter());

    class MyServer {
    public:
        MyServer(boost::asio::io_service& io) :
            m_io(io)
        {}

        void start() {
            boost::asio::ip::tcp::acceptor acceptor(m_io);
            acceptor.open(boost::asio::ip::tcp::v4());
            acceptor.set_option(boost::asio::socket_base::reuse_address(true));
            acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
            acceptor.listen();

            std::cout << "waiting for connection" << std::endl;
            boost::asio::ip::tcp::socket client_socket(m_io);
            acceptor.accept(client_socket);
            std::cout << "received connection... waiting 12 bytes" << std::endl;

            std::vector<uint8_t> rx_buffer;
            rx_buffer.resize(12);
            boost::asio::read(client_socket, boost::asio::buffer(rx_buffer));
            std::cout << "received 12 bytes" << std::endl;
            REQUIRE(rx_buffer == (std::vector<uint8_t>{0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x00, 0x14, 0x00, 0x08}));

            std::vector<uint8_t> tx_buffer{0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0b01001100};
            boost::asio::write(client_socket, boost::asio::buffer(tx_buffer));
        }

    private:
        boost::asio::io_service &m_io;
    };


    boost::asio::io_service io;
    MyServer server(io);
    std::thread t([&server]() { server.start(); });
    usleep(5000);

    ModbusClient client(modbus::tcp::UnitId(0xab), std::move(fmt));
    client.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));

    cmd.exec(client, std::vector<std::string>{"--startAddress", "20", "--numCoils", "8"});

    t.join();
}

