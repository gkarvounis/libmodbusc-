#ifndef TEST_SOCKET_CONNECTOR_HPP
#define TEST_SOCKET_CONNECTOR_HPP


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

#endif

