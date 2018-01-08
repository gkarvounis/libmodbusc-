#ifndef SOCKET_CONNECTOR_HPP
#define SOCKET_CONNECTOR_HPP


#include <list>

class SocketConnector : public std::enable_shared_from_this<SocketConnector> {
public:
    using PSocket = std::unique_ptr<boost::asio::ip::tcp::socket>;
    using SocketConnectedCb = std::function<void(PSocket)>;
    using Endpoint = boost::asio::ip::tcp::endpoint;
    using Interval = boost::posix_time::milliseconds;

                                                    SocketConnector(boost::asio::io_service& io);
                                                    SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval);
                                                   ~SocketConnector();

    void                                            async_connect(const Endpoint& ep, const Interval& interval, SocketConnectedCb cb);
    void                                            async_connect(SocketConnectedCb cb);

    void                                            cancel();

    boost::asio::io_service&                        get_io_service();

private:
    boost::asio::deadline_timer                     m_timer;
    Interval                                        m_interval;
    SocketConnectedCb                               m_connectedCb;
    Endpoint                                        m_endpoint;
    PSocket                                         m_socket;

    void                                            initConnection();
    void                                            initTimer();
    void                                            postCallback();
};


SocketConnector::SocketConnector(boost::asio::io_service& io) :
    m_timer(io),
    m_interval(0),
    m_connectedCb(),
    m_endpoint(),
    m_socket(nullptr)
{}


SocketConnector::SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval) :
    m_timer(io),
    m_interval(interval),
    m_connectedCb(),
    m_endpoint(ep),
    m_socket(nullptr)
{}


SocketConnector::~SocketConnector() {
    std::cout << "~SocketConnector" << std::endl;
}


boost::asio::io_service& SocketConnector::get_io_service() {
    return m_timer.get_io_service();
}


void SocketConnector::async_connect(const Endpoint& ep, const Interval& interval, SocketConnectedCb cb) {
    auto self = shared_from_this();

    m_timer.get_io_service().post([self, this, ep, interval, cb]() {
        m_endpoint = ep;
        m_connectedCb = cb;
        m_interval = interval;
        m_socket.reset(new boost::asio::ip::tcp::socket(m_timer.get_io_service()));
        initConnection();
    });
}


void SocketConnector::async_connect(SocketConnectedCb cb) {
    auto self = shared_from_this();
    m_timer.get_io_service().post([self, this, cb]() {
        m_connectedCb = cb;
        m_socket.reset(new boost::asio::ip::tcp::socket(m_timer.get_io_service()));
        initConnection();
    });
}


void SocketConnector::initConnection() {
    auto self = shared_from_this();
    std::cout << "attempting connection to " << m_endpoint << std::endl;

    m_socket->async_connect(m_endpoint, [self, this](const boost::system::error_code& ec) {
        std::cout << "on connected ec=" << ec.message() << std::endl;
        if (ec == boost::asio::error::operation_aborted) {
            m_socket.reset(nullptr);
            postCallback();
        } else if (ec) {
            initTimer();
        } else {
            postCallback();
        }
    });
}


void SocketConnector::initTimer() {
    m_timer.expires_from_now(m_interval);
    m_timer.async_wait([this](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            m_socket.reset(nullptr);
            postCallback();
        } else
            initConnection();
    });
}


void SocketConnector::cancel() {
    m_timer.get_io_service().post([this]() {
        m_socket->cancel();
        m_timer.cancel();
    });
}


void SocketConnector::postCallback() {
    auto cb = m_connectedCb;
    m_connectedCb = nullptr;

    auto raw_socket = m_socket.release();
    m_timer.get_io_service().post([cb, raw_socket]() {
        cb(PSocket(raw_socket));
    });
}

#endif

