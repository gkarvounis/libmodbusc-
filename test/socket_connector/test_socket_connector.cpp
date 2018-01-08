#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include <boost/asio.hpp>
#include "SocketConnector.hpp"


/*
void modbusServer(const std::vector<uint8_t>& expected_request, const::std::vector<uint8_t>& response) {
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
    boost::asio::ip::tcp::socket sock(io);

    acceptor.accept(sock);
    std::vector<uint8_t> req;

    while(true) {
        req.resize(expected_request.size());
        boost::asio::read(sock, boost::asio::buffer(req));
        REQUIRE(expected_request == req);

        boost::asio::write(sock, boost::asio::buffer(response));
    }
    
}
*/

TEST_CASE("modbus_poller", "[poller]") {
/*
    std::thread t([]() {
        sleep(5);
        modbusServer(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02}, std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01});
    });

    boost::asio::io_service io;
//    ModbusQueue q(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
//    q.notify_start();
//    q.notify_request(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02}, [](const std::vector<uint8_t>&) {
//    });

    ModbusPoller poller(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502), modbus::tcp::UnitId(0xab));
    poller.addReadCoilsPoll(modbus::tcp::Address(0x1000), modbus::tcp::NumBits(8), modbus::tcp::TransactionId(0x00cd), boost::posix_time::milliseconds(2000));
    poller.start();
    io.run();
*/
}


TEST_CASE("socket connector - canceling connection attempt", "[SocketConnector]") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);
    std::shared_ptr<SocketConnector> connector = std::make_shared<SocketConnector>(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080), boost::posix_time::milliseconds(500));
    connector->async_connect(sock, [](const boost::system::error_code& ec) {
        REQUIRE(ec == boost::asio::error::operation_aborted);
    });

    boost::asio::deadline_timer tmr(io);
    tmr.expires_from_now(boost::posix_time::milliseconds(1100));
    tmr.async_wait([connector](const boost::system::error_code& /*ec*/) {
        connector->cancel();
    });

    io.run();
}


TEST_CASE("socket connector - connection succeeds immediately", "[SocketConnector]") {
    boost::asio::io_service io;
    std::shared_ptr<SocketConnector> connector = std::make_shared<SocketConnector>(io);

    bool connected = false;
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));
    boost::asio::ip::tcp::socket client(io);
    acceptor.async_accept(client, [&connected](const boost::system::error_code& ec) {
        REQUIRE( !ec );
        connected = true;
    });

    SocketConnector::Socket sock(io);
    connector->async_connect(sock, SocketConnector::Endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080), SocketConnector::Interval(2000), [](const boost::system::error_code& ec) {
        REQUIRE( !ec );
    });

    io.run();

    REQUIRE(connected == true);
}


TEST_CASE("socket connector - connection succeeds after some attempts", "[SocketConnector]") {
    boost::asio::io_service io;

    std::shared_ptr<SocketConnector> connector = std::make_shared<SocketConnector>(io);

    bool connected = false;
    boost::asio::deadline_timer tmr(io);
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));
    SocketConnector::Socket client(io);

    tmr.expires_from_now(boost::posix_time::milliseconds(1500));
    tmr.async_wait([&acceptor, &connected, &client](const boost::system::error_code& ec) {
        REQUIRE( !ec );

        acceptor.async_accept(client, [&connected](const boost::system::error_code& ec) {
            REQUIRE( !ec );
            connected = true;
        });
    });

    SocketConnector::Socket sock(io);
    connector->async_connect(sock, SocketConnector::Endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080), SocketConnector::Interval(200), [](const boost::system::error_code& ec) {
        REQUIRE( !ec );
    });

    io.run();

    REQUIRE(connected == true);
}

