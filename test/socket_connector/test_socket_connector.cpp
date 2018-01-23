#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include <boost/asio.hpp>
#include "SocketConnector.hpp"



boost::asio::ip::tcp::endpoint make_endpoint(const std::string& addr, uint16_t port) {
    return boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(addr), port);
}


boost::posix_time::milliseconds millisecs(std::size_t n) {
    return boost::posix_time::milliseconds(n);
}


TEST_CASE("socket connector - canceling connection attempt", "[SocketConnector]") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);
    SocketConnector connector(io, make_endpoint("127.0.0.1", 8080), millisecs(500));

    connector.async_connect(sock, [](const boost::system::error_code& ec) {
        REQUIRE(ec == boost::asio::error::operation_aborted);
    });

    boost::asio::deadline_timer tmr(io);
    tmr.expires_from_now(millisecs(1100));
    tmr.async_wait([&connector](const boost::system::error_code& /*ec*/) {
        connector.cancel();
    });

    io.run();
}


TEST_CASE("socket connector - connection succeeds immediately", "[SocketConnector]") {
    boost::asio::io_service io;
    SocketConnector connector(io);

    bool connected = false;
    boost::asio::ip::tcp::acceptor acceptor(io, make_endpoint("127.0.0.1", 8080));
    boost::asio::ip::tcp::socket client(io);

    acceptor.async_accept(client, [&connected](const boost::system::error_code& ec) {
        REQUIRE( !ec );
        connected = true;
    });

    bool connected_callback_called = false;
    boost::asio::ip::tcp::socket sock(io);
    connector.async_connect(sock, make_endpoint("127.0.0.1", 8080), SocketConnector::Interval(2000), [&connected_callback_called](const boost::system::error_code& ec) {
        REQUIRE( !ec );
        connected_callback_called = true;
    });

    io.run();

    REQUIRE(connected == true);
    REQUIRE(connected_callback_called == true);
}


TEST_CASE("socket connector - connection succeeds after some attempts", "[SocketConnector]") {
    boost::asio::io_service io;
    SocketConnector connector(io);

    bool connected = false;
    boost::asio::deadline_timer tmr(io);
    boost::asio::ip::tcp::acceptor acceptor(io, make_endpoint("127.0.0.1", 8080));
    boost::asio::ip::tcp::socket client(io);

    tmr.expires_from_now(millisecs(1500));
    tmr.async_wait([&acceptor, &connected, &client](const boost::system::error_code& ec) {
        REQUIRE( !ec );

        acceptor.async_accept(client, [&connected](const boost::system::error_code& ec) {
            REQUIRE( !ec );
            connected = true;
        });
    });

    bool connected_callback_called = false;
    SocketConnector::Socket sock(io);
    connector.async_connect(sock, make_endpoint("127.0.0.1", 8080), SocketConnector::Interval(200), [&connected_callback_called](const boost::system::error_code& ec) {
        REQUIRE( !ec );
        connected_callback_called = true;
    });

    io.run();

    REQUIRE(connected == true);
    REQUIRE(connected_callback_called == true);
}

