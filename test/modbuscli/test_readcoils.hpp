#ifndef TEST_READCOILS_HPP
#define TEST_READCOILS_HPP

// Simple modbus server that receives a request and sends a response
void dummyModbusServer(std::vector<uint8_t>& request, const std::vector<uint8_t>& response) {
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acceptor(io);
    acceptor.open(boost::asio::ip::tcp::v4());
    acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
    acceptor.listen();

    std::cout << "waiting for connection" << std::endl;
    boost::asio::ip::tcp::socket client_socket(io);
    acceptor.accept(client_socket);
    std::cout << "received connection... waiting " << sizeof(modbus::tcp::Header) << " bytes" << std::endl;

    request.resize(sizeof(modbus::tcp::Header));
    boost::asio::read(client_socket, boost::asio::buffer(request));

    modbus::tcp::decoder_views::Header req_header_view(request);
    std::size_t payload_size = req_header_view.getLength() - 2;
    std::cout << "received modbus header. Waiting for " << payload_size << " bytes" << std::endl;

    request.resize(sizeof(modbus::tcp::Header) + payload_size);
    boost::asio::read(client_socket, boost::asio::buffer(request.data() + sizeof(modbus::tcp::Header), payload_size));
    std::cout << "received payload" << std::endl;

    boost::asio::write(client_socket, boost::asio::buffer(response));
    std::cout << "sent response" << std::endl;
}



TEST_CASE("readcoils - normal case", "[cli]") {
    // start dummy server
    std::thread t([]() { 
        std::vector<uint8_t> request;
        std::vector<uint8_t> response{0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b01001100};
        dummyModbusServer(request, response);
       
        REQUIRE(request == (std::vector<uint8_t>{0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x00, 0x14, 0x00, 0x08}));
    });
    usleep(5000);


    // Create a ModbusClient that sends verbose output to a stringstream
    std::stringstream out;
    ModbusClient client(modbus::tcp::UnitId(0xab), std::unique_ptr<OutputFormatter>(new VerboseStandardOutputFormatter(out)));

    // Connect to dummy server and execute readcoils command
    client.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
    ReadCoilsCommand cmd;
    cmd.exec(client, std::vector<std::string>{"--startAddress", "20", "--numCoils", "8"});

    // Dummy server is done now
    t.join();

    // Make sure that output printed by the command is correct
    std::stringstream expected_out;
    expected_out <<
           "          req: [00 01 00 00 00 06 ab 01 00 14 00 08 ]" << std::endl
        << "          rsp: [00 01 00 00 00 04 ab 01 01 4c ]" << std::endl
        << "       unitId: 0xab" << std::endl
        << "function code: 0x01" << std::endl
        << "transactionId: 0x0001" << std::endl
        << "         bits: 0 0 1 1 0 0 1 0 " << std::endl;

    REQUIRE(expected_out.str() == out.str());
}


TEST_CASE("readcoils - non existing coils", "[cli]") {
    // start dummy server
    std::thread t([]() { 
        std::vector<uint8_t> request;
        std::vector<uint8_t> response{0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0xab, 0x81, 0x01};
        dummyModbusServer(request, response);
       
        REQUIRE(request == (std::vector<uint8_t>{0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x00, 0x14, 0x00, 0x08}));
    });
    usleep(5000);

    // Create a ModbusClient that sends verbose output to a stringstream
    std::stringstream out;
    ModbusClient client(modbus::tcp::UnitId(0xab), std::unique_ptr<OutputFormatter>(new VerboseStandardOutputFormatter(out)));

    // Connect to dummy server and execute readcoils command
    client.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
    ReadCoilsCommand cmd;
    cmd.exec(client, std::vector<std::string>{"--startAddress", "20", "--numCoils", "8"});

    // Dummy server is done now
    t.join();

    // Make sure that output printed by the command is correct
    std::stringstream expected_out;
    expected_out <<
           "          req: [00 01 00 00 00 06 ab 01 00 14 00 08 ]" << std::endl
        << "          rsp: [00 01 00 00 00 03 ab 81 01 ]" << std::endl
        << "       unitId: 0xab" << std::endl
        << "function code: 0x01" << std::endl
        << "transactionId: 0x0001" << std::endl
        << "        error: 1" << std::endl;

    REQUIRE(expected_out.str() == out.str());
}


#endif

