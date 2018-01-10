#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include <boost/asio.hpp>

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include "SocketConnector.hpp"
#include "ModbusPollTask.hpp"
#include "ModbusPoller.hpp"
#include <thread>


boost::asio::ip::tcp::endpoint make_endpoint(const std::string& ip, uint16_t port) {
    return boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
}


boost::posix_time::milliseconds make_millisecs(std::size_t msecs) {
    return boost::posix_time::milliseconds(msecs);
}



void modbusServer(const std::vector<uint8_t>& expected_request, const::std::vector<uint8_t>& response) {
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8502));
    acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    boost::asio::ip::tcp::socket sock(io);

    acceptor.accept(sock);
    std::vector<uint8_t> req;

    while(true) {
        req.resize(expected_request.size());
        std::cout << "Expecting request (" << expected_request.size() << " bytes)" << std::endl;
        boost::asio::read(sock, boost::asio::buffer(req));
        REQUIRE(expected_request == req);

        boost::asio::write(sock, boost::asio::buffer(response));
        std::cout << "Response sent (" << response.size() << " bytes)" << std::endl;
    }
}


class OneTimeTcpAcceptor {
public:
    OneTimeTcpAcceptor(boost::asio::io_service& io) :
        m_timer(io),
        m_acceptor(io),
        m_client(io),
        m_connected(false)
    {}

    ~OneTimeTcpAcceptor() {
        REQUIRE(m_connected == true);
    }

    void async_accept(const boost::asio::ip::tcp::endpoint& ep, const boost::posix_time::milliseconds& delay) {
        m_endpoint = ep;

        m_timer.expires_from_now(delay);
        m_timer.async_wait([this] (const boost::system::error_code& ec) {
            REQUIRE(!ec);
            onTimer();
        });
    }
    
private:
    boost::asio::deadline_timer      m_timer;
    boost::asio::ip::tcp::acceptor   m_acceptor;
    boost::asio::ip::tcp::socket     m_client;
    boost::asio::ip::tcp::endpoint   m_endpoint;
    bool                             m_connected;

    void onTimer() {
        m_acceptor.open(boost::asio::ip::tcp::v4());
        m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
        m_acceptor.bind(make_endpoint("127.0.0.1", 8502));
        m_acceptor.listen();

        m_acceptor.async_accept(m_client, [this](const boost::system::error_code& ec) {
            m_acceptor.close();
            REQUIRE(!ec);
            m_connected = true;
        });
    }
};


TEST_CASE("ModbusPoller - no polling tasks", "[ModbusPoller]") {
    boost::asio::io_service   io;
    OneTimeTcpAcceptor        acceptor(io);

    std::shared_ptr<ModbusPoller> poller = std::make_shared<ModbusPoller>(io, make_endpoint("127.0.0.1", 8502), make_millisecs(200));
    poller->start();

    acceptor.async_accept(make_endpoint("127.0.0.1", 8502), make_millisecs(200));

    io.run();
}


TEST_CASE("ModbusPoller - one polling task", "[ModbusPoller]") {
    /*
    std::vector<uint8_t> req;
    std::vector<uint8_t> rsp;
    modbus::tcp::Encoder encoder(modbus::tcp::UnitId(0xab), modbus::tcp::TransactionId(0x0002));
    encoder.encodeReadCoilsReq(modbus::tcp::Address(0x0123), modbus::tcp::NumBits(8), req);
    encoder.encodeReadCoilsRsp(std::vector<bool>{0, 1, 0, 0, 1, 1, 0, 1}, rsp);

    std::thread t([&req, &rsp]() {
        modbusServer(req, rsp);
    });

    boost::asio::io_service io;

    std::shared_ptr<ModbusPoller> poller = std::make_shared<ModbusPoller>(io, make_endpoint("127.0.0.1", 8502), make_millisecs(100));

    std::vector<uint8_t> rx_buffer;
    poller->addPollTask(req, rx_buffer, make_millisecs(1000), []() {
        std::cout << "Sample!" << std::endl;
    });

    poller->start();
    io.run();
    t.join();
    */
}

