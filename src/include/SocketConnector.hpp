#ifndef SOCKET_CONNECTOR_HPP
#define SOCKET_CONNECTOR_HPP


class SocketConnector : public std::enable_shared_from_this<SocketConnector> {
public:
    using Socket                                    = boost::asio::ip::tcp::socket;
    using Endpoint                                  = boost::asio::ip::tcp::endpoint;
    using Interval                                  = boost::posix_time::milliseconds;
    using SocketConnectedCb                         = std::function<void(const boost::system::error_code& ec)>;

                                                    SocketConnector(boost::asio::io_service& io);
                                                    SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval);
                                                   ~SocketConnector();

    void                                            async_connect(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb);
    void                                            async_connect(Socket& sock, SocketConnectedCb cb);

    void                                            cancel();

    boost::asio::io_service&                        get_io_service();

private:
    boost::asio::deadline_timer                     m_timer;
    Interval                                        m_interval;
    SocketConnectedCb                               m_connectedCb;
    Endpoint                                        m_endpoint;
    std::function<void(void)>                       m_cancel;

    void                                            initConnection(Socket& sock);
    void                                            initTimer(Socket& sock);
    void                                            postCallback();
    void                                            postCallback(const boost::system::error_code& ec);

    void                                            cancelConnection(Socket& sock);
};


SocketConnector::SocketConnector(boost::asio::io_service& io) :
    m_timer(io),
    m_interval(0),
    m_connectedCb(),
    m_endpoint(),
    m_cancel()
{}


SocketConnector::SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval) :
    m_timer(io),
    m_interval(interval),
    m_connectedCb(),
    m_endpoint(ep)
{}


SocketConnector::~SocketConnector() {
    std::cout << "~SocketConnector" << std::endl;
}


boost::asio::io_service& SocketConnector::get_io_service() {
    return m_timer.get_io_service();
}


void SocketConnector::async_connect(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb) {
    m_endpoint = ep;
    m_interval = interval;
    m_connectedCb = cb;
    
    auto self = shared_from_this();

    m_cancel = [self, this, &sock]() {
        cancelConnection(sock);
    };

    initConnection(sock);
}


void SocketConnector::async_connect(Socket& sock, SocketConnectedCb cb) {
    m_connectedCb = cb;

    auto self = shared_from_this();

    m_cancel = [self, this, &sock]() {
        cancelConnection(sock);
    };

    initConnection(sock);
}


void SocketConnector::initConnection(Socket& sock) {
    auto self = shared_from_this();
    std::cout << "Attempting connection to " << m_endpoint << std::endl;

    sock.async_connect(m_endpoint, [self, this, &sock](const boost::system::error_code& ec) {
        std::cout << "on connected ec=" << ec.message() << std::endl;

        if (ec == boost::asio::error::operation_aborted) {
            postCallback(ec);
        } else if (ec) {
            initTimer(sock);
        } else {
            postCallback();
        }
    });
}


void SocketConnector::initTimer(Socket& sock) {
    auto self = shared_from_this();

    m_timer.expires_from_now(m_interval);

    m_timer.async_wait([self, this, &sock](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            postCallback(ec);
        } else
            initConnection(sock);
    });
}


void SocketConnector::postCallback(const boost::system::error_code& ec) {
    auto cb = std::move(m_connectedCb);
    m_cancel = nullptr;

    m_timer.get_io_service().post([cb, ec]() {
        cb(ec);
    });
}


void SocketConnector::postCallback() {
    auto cb = std::move(m_connectedCb);

    m_cancel = nullptr;
    m_connectedCb = nullptr;

    m_timer.get_io_service().post([cb]() {
        cb(boost::system::error_code());
    });

}


void SocketConnector::cancel() {
    if (m_cancel)
        m_cancel();
}


void SocketConnector::cancelConnection(Socket& sock) {
    m_timer.cancel();
    sock.cancel();
}

#endif

