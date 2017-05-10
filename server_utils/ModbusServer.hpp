#ifndef MODBUS_DEVICE_HPP
#define MODBUS_DEVICE_HPP

#include "ModbusSession.hpp"
#include <set>
#include <stdexcept>


template <typename ModbusDevice>
class ModbusServer {
public:
                                    ModbusServer(boost::asio::io_service& io, ModbusDevice& backend);

    template <typename Callback>
    void                            start(const std::string& ip, uint16_t port, Callback done_cb);
    void                            stop();

private:
    using PModbusSession = std::shared_ptr<ModbusSession<ModbusDevice>>;


    ModbusDevice            &m_backend;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    std::set<PModbusSession>        m_sessions;
    std::function<void(void)>       m_done_cb;

    template <typename Callback>
    void                            on_start(const std::string& ip, uint16_t port, Callback done_cb);
    void                            init_accepting();
    void                            on_client_connected(PModbusSession session, const boost::system::error_code& ec);
    void                            on_session_done(PModbusSession session);

    void                            on_stop();
    void                            trigger_done_cb();
    void                            init_shutdown_sessions();
};


template <typename ModbusDevice>
ModbusServer<ModbusDevice>::ModbusServer(boost::asio::io_service& io, ModbusDevice& backend) :
    m_backend(backend),
    m_acceptor(io),
    m_sessions()
{}


template <typename ModbusDevice>
template <typename Callback>
void ModbusServer<ModbusDevice>::start(const std::string& ip, uint16_t port, Callback cb) {
    m_acceptor.get_io_service().post([this, ip, port, cb]() {
        on_start(ip, port, cb);
    });
}


template <typename ModbusDevice>
template <typename Callback>
void ModbusServer<ModbusDevice>::on_start(const std::string& ip, uint16_t port, Callback cb) {
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
    m_acceptor.open(ep.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(ep);
    m_acceptor.listen();

    m_done_cb = cb;
    init_accepting();
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::init_accepting() {
    PModbusSession session = std::make_shared<ModbusSession<ModbusDevice>>(m_acceptor.get_io_service(), m_backend);

    m_acceptor.async_accept(
        session->socket(),
        [this, session](const boost::system::error_code& ec) {
            on_client_connected(session, ec);
        });
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::on_client_connected(PModbusSession session, const boost::system::error_code& ec) {
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


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::on_session_done(PModbusSession session) {
    m_sessions.erase(session);
    std::cout << "session done " << session.get() << std::endl;

    if ((!m_acceptor.is_open()) && m_sessions.empty())
        trigger_done_cb();
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::stop() {
    m_acceptor.get_io_service().post([this]() {
        on_stop();
    });
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::on_stop() {
    m_acceptor.close();
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::trigger_done_cb() {
    m_acceptor.get_io_service().post([this]() {
        m_done_cb();
        m_done_cb = nullptr;
    });
}


template <typename ModbusDevice>
void ModbusServer<ModbusDevice>::init_shutdown_sessions() {
    for (auto &s: m_sessions) {
        s->stop();
    }
}

#endif
