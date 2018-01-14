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
#include "ModbusServerDevice.hpp"
#include "ModbusServer.hpp"
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

    try {
        while(true) {
            req.resize(expected_request.size());
            std::cout << "Expecting request (" << expected_request.size() << " bytes)" << std::endl;
            boost::asio::read(sock, boost::asio::buffer(req));
            REQUIRE(expected_request == req);

            boost::asio::write(sock, boost::asio::buffer(response));
            std::cout << "Response sent (" << response.size() << " bytes)" << std::endl;
        }
    } catch(const boost::system::system_error& ec) {
        REQUIRE (ec.what() == std::string("read: End of file"));
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

    void asyncAcceptWithDelay(const boost::asio::ip::tcp::endpoint& ep, const boost::posix_time::milliseconds& delay, std::function<void(void)> cb) {
        m_endpoint = ep;
        m_callback = cb;

        m_timer.expires_from_now(delay);
        m_timer.async_wait([this] (const boost::system::error_code& ec) {
            REQUIRE(!ec);
            onTimer();
        });
    }

    void asyncAccept(const boost::asio::ip::tcp::endpoint& ep, std::function<void(void)> cb) {
        m_endpoint = ep;
        m_callback = cb;

        m_timer.get_io_service().post([this]() { onTimer(); });
    }

    boost::asio::ip::tcp::socket& getConnectedClient() {
        return m_client;
    }
    
private:
    boost::asio::deadline_timer      m_timer;
    boost::asio::ip::tcp::acceptor   m_acceptor;
    boost::asio::ip::tcp::socket     m_client;
    boost::asio::ip::tcp::endpoint   m_endpoint;
    bool                             m_connected;
    std::function<void(void)>        m_callback;

    void onTimer() {
        m_acceptor.open(boost::asio::ip::tcp::v4());
        m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
        m_acceptor.bind(make_endpoint("127.0.0.1", 8502));
        m_acceptor.listen();

        m_acceptor.async_accept(m_client, [this](const boost::system::error_code& ec) {
            m_acceptor.close();
            REQUIRE(!ec);
            m_connected = true;
            m_callback();
        });
    }
};


TEST_CASE("ModbusPoller - no polling tasks", "[ModbusPoller]") {
    boost::asio::io_service   io;
    OneTimeTcpAcceptor        acceptor(io);

    std::shared_ptr<ModbusPoller> poller = std::make_shared<ModbusPoller>(io, make_endpoint("127.0.0.1", 8502), make_millisecs(200));
    poller->start();

    acceptor.asyncAcceptWithDelay(make_endpoint("127.0.0.1", 8502), make_millisecs(200), []()  {});

    io.run();
}


TEST_CASE("ModbusPoller - one polling task", "[ModbusPoller]") {
    class MyTest : public modbus::tcp::ServerDevice {
    public:
        MyTest() :
            modbus::tcp::ServerDevice(modbus::tcp::UnitId(0xab)),
            m_io(),
            m_server(m_io, *this),
            m_poller(std::make_shared<ModbusPoller>(m_io, make_endpoint("127.0.0.1", 8502), make_millisecs(100))),
            m_request(),
            m_expectedSample(),
            m_rxSample(),
            m_coils({0,1,0,0,1,1,0,1}),
            m_numReceivedSamples(0)
        {
            modbus::tcp::Encoder encoder(modbus::tcp::UnitId(0xab), modbus::tcp::TransactionId(0x0002));
            encoder.encodeReadCoilsReq(modbus::tcp::Address(0x0123), modbus::tcp::NumBits(8), m_request);
            encoder.encodeReadCoilsRsp(m_coils, m_expectedSample);

            m_poller->addPollTask(m_request, m_rxSample, make_millisecs(500), [this]() {
                std::cout << "Sample!" << std::endl;
                m_numReceivedSamples++;
                REQUIRE(m_rxSample == m_expectedSample);

                if (m_numReceivedSamples == 3) {
                    m_poller->cancel();
                    m_server.stop();
                }
            });
        }

        ~MyTest() {
            REQUIRE(m_numReceivedSamples == 3);
        }

        void start() {
            m_server.start(make_endpoint("127.0.0.1", 8502), []() {});
            m_poller->start();
            m_io.run();
        }

    protected:
        bool getCoil(const modbus::tcp::Address& addr) const override {
            return m_coils.at(addr.get() - 0x0123);
        }
            
    private:
        using PModbusPoller = std::shared_ptr<ModbusPoller>;

        boost::asio::io_service             m_io;
        modbus::tcp::Server                 m_server;
        PModbusPoller                       m_poller;

        std::vector<uint8_t>                m_request;
        std::vector<uint8_t>                m_expectedSample;
        std::vector<uint8_t>                m_rxSample;
        std::vector<uint8_t>                m_coils;

        std::size_t                         m_numReceivedSamples;
    };

    MyTest test;
    test.start();
}
