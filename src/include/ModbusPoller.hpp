#ifndef MODBUS_POLLER_HPP
#define MODBUS_POLLER_HPP

#include <list>

class ModbusQueue {
public:
    using Callback =                            std::function<void(const std::vector<uint8_t>& rsp)>;

                                                ModbusQueue(boost::asio::io_service& io, const boost::asio::ip::tcp::endpoint& ep);

    void                                        notify_start();
    void                                        notify_request(const std::vector<uint8_t>& req, Callback cb);

private:
    struct Request {
                                     Request(const std::vector<uint8_t>& req, Callback cb) : req(req), cb(cb) {}
        const std::vector<uint8_t>  &req;
        Callback                     cb;
    };

    void                                        initConnection();
    void                                        initRetryTimer();
    void                                        initSending();
    void                                        initHeaderReception();
    void                                        initPayloadReception();

    boost::asio::ip::tcp::socket                m_socket;
    boost::asio::deadline_timer                 m_timer;
    boost::asio::ip::tcp::endpoint              m_endpoint;
    std::list<std::unique_ptr<Request>>         m_queue;
    bool                                        m_connected;
    std::vector<uint8_t>                        m_rx_buffer;

    std::unique_ptr<boost::asio::io_service::work>   m_work;
};


ModbusQueue::ModbusQueue(boost::asio::io_service& io, const boost::asio::ip::tcp::endpoint& ep) :
    m_socket(io),
    m_timer(io),
    m_endpoint(ep),
    m_queue(),
    m_connected(false),
    m_rx_buffer(),
    m_work(nullptr)
{}


void ModbusQueue::notify_start() {
    m_socket.get_io_service().post([this]() {
        initConnection();
    });
}


void ModbusQueue::initConnection() {
    std::cout << "trying connection to " << m_endpoint << std::endl;

    m_socket.async_connect(m_endpoint, [this](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        } else if (ec) {
            std::cout << "connection attempt failed (" << ec.message() << ") - starting retry timer" << std::endl;
            initRetryTimer();
        } else {
            std::cout << "connected!" << std::endl;
            m_connected = true;

            if (m_queue.empty())
                m_work.reset(new boost::asio::io_service::work(m_socket.get_io_service()));
            else
                initSending();
        }
    });
}


void ModbusQueue::initRetryTimer() {
    m_timer.expires_from_now(boost::posix_time::milliseconds(2000));
    m_timer.async_wait([this](const boost::system::error_code& ec) {
        if (ec)
            return;

        initConnection();
    });
}


void ModbusQueue::initSending() {
    boost::asio::async_write(m_socket, boost::asio::buffer(m_queue.front()->req), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec == boost::asio::error::operation_aborted)
            return;
        else if (ec) {
            m_connected = false;
            m_socket.close();
            initRetryTimer();
        } else {
            initHeaderReception();
        }
    });
}


void ModbusQueue::initHeaderReception() {
    m_rx_buffer.resize(sizeof(modbus::tcp::Header));
    boost::asio::async_read(m_socket, boost::asio::buffer(m_rx_buffer), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec == boost::asio::error::operation_aborted)
            return;
        else if (ec) {
            m_connected = false;
            m_socket.close();
            initRetryTimer();
        } else {
            initPayloadReception();
        }
    });
}


void ModbusQueue::initPayloadReception() {
    modbus::tcp::decoder_views::Header rsp_header_view(m_rx_buffer);
    std::size_t payload_size = rsp_header_view.getLength() - 2;

    m_rx_buffer.resize(sizeof(modbus::tcp::Header) + payload_size);

    uint8_t* payload = m_rx_buffer.data() + sizeof(modbus::tcp::Header);

    boost::asio::async_read(m_socket, boost::asio::buffer(payload, payload_size), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec == boost::asio::error::operation_aborted)
            return;
        else if (ec) {
            m_connected = false;
            m_socket.close();
            initRetryTimer();
        } else {
            m_queue.front()->cb(m_rx_buffer);
            m_queue.pop_front();

            if (m_queue.empty()) {
                m_work.reset(new boost::asio::io_service::work(m_socket.get_io_service()));
            } else {
                initSending();
            }
        }
    });
}


void ModbusQueue::notify_request(const std::vector<uint8_t>& req, Callback cb) {
    auto tmp = new Request(req, cb);

    m_socket.get_io_service().post([this, tmp]() {
        bool isFirst = m_queue.empty();

        m_queue.push_back(std::unique_ptr<Request>(tmp));

        if (isFirst && m_connected) {
            m_work.reset(nullptr);
            initSending();
        }
    });
}



class ModbusPoller {
public:
                                                ModbusPoller(boost::asio::io_service& io);
    void                                        addModbusRequest(const std::vector<uint8_t>& req, const boost::posix_time::milliseconds& interval);
    void                                        start();

private:
    struct ModbusPoll {
                                                ModbusPoll(boost::asio::io_service& io) : timer(io), request(), interval(0) {}
        boost::asio::deadline_timer             timer;
        std::vector<uint8_t>                    request;
        boost::posix_time::milliseconds         interval;
    };

    boost::asio::io_service                    &m_io;
    boost::asio::deadline_timer                 m_timer;
    std::vector<std::unique_ptr<ModbusPoll>>    m_pollObjects;
};


ModbusPoller::ModbusPoller(boost::asio::io_service& io) :
    m_io(io),
    m_timer(io)
{
}


void ModbusPoller::addModbusRequest(const std::vector<uint8_t>& req, const boost::posix_time::milliseconds& interval) {
    auto p = new ModbusPoll(m_io);
    p->request = req;
    p->interval = interval;

    m_pollObjects.push_back(std::unique_ptr<ModbusPoll>(p));
}


void ModbusPoller::start() {

}

#endif

