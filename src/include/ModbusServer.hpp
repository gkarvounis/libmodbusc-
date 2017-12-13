#ifndef MODBUS_SERVER_HPP
#define MODBUS_SERVER_HPP

#include "ModbusServerSession.hpp"

#include <set>
#include <stdexcept>


namespace modbus {
namespace tcp {

class Server {
public:
                                    Server(boost::asio::io_service& io, ServerDevice& device);

    void                            start(const boost::asio::ip::tcp::endpoint& ep, std::function<void(void)> done_cb);
    void                            stop();

private:
    using PModbusSession = std::shared_ptr<ServerSession>;

    ServerDevice                   &m_device;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    std::set<PModbusSession>        m_sessions;
    std::function<void(void)>       m_done_cb;

    void                            on_start(const boost::asio::ip::tcp::endpoint& ep, std::function<void(void)> done_cb);
    void                            init_accepting();
    void                            on_client_connected(PModbusSession session, const boost::system::error_code& ec);
    void                            on_session_done(PModbusSession session);

    void                            trigger_done_cb();
    void                            init_shutdown_sessions();
};


Server::Server(boost::asio::io_service& io, ServerDevice& device) :
    m_device(device),
    m_acceptor(io),
    m_sessions()
{}


void Server::start(const boost::asio::ip::tcp::endpoint& ep, std::function<void(void)> cb) {
    m_acceptor.get_io_service().post([this, ep, cb]() {
        m_acceptor.open(ep.protocol());
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(ep);
        m_acceptor.listen();

        m_done_cb = cb;
        init_accepting();
    });
}


void Server::init_accepting() {
    PModbusSession session = std::make_shared<ServerSession>(m_acceptor.get_io_service(), m_device);

    m_acceptor.async_accept(
        session->socket(),
        [this, session](const boost::system::error_code& ec) {
            on_client_connected(session, ec);
        });
}


void Server::on_client_connected(PModbusSession session, const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) {
        if (m_sessions.empty())
            trigger_done_cb();
        else
            init_shutdown_sessions();
    } else {
        m_sessions.insert(session);
        session->start([this, session]() {
            m_acceptor.get_io_service().post([this, session]() {
                on_session_done(session);
            });
        });
        init_accepting();
    }
}


void Server::on_session_done(PModbusSession session) {
    m_sessions.erase(session);
    std::cout << "session done " << session.get() << std::endl;

    if ((!m_acceptor.is_open()) && m_sessions.empty())
        trigger_done_cb();
}


void Server::stop() {
    m_acceptor.get_io_service().post([this]() {
        m_acceptor.close();
    });
}


void Server::trigger_done_cb() {
    m_acceptor.get_io_service().post([this]() {
        m_done_cb();
        m_done_cb = nullptr;
    });
}


void Server::init_shutdown_sessions() {
    for (auto &s: m_sessions) {
        s->stop();
    }
}

} // namespace tcp
} // namespace modbus

#endif
