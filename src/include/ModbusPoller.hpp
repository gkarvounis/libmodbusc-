#ifndef MODBUS_POLLER_HPP
#define MODBUS_POLLER_HPP


#include <list>


class ModbusPoller : public std::enable_shared_from_this<ModbusPoller> {
public:
    using Interval                              = boost::posix_time::milliseconds;
    using Endpoint                              = boost::asio::ip::tcp::endpoint;
    using Socket                                = boost::asio::ip::tcp::socket;
    using Vector                                = std::vector<uint8_t>;
    using PollCallback                          = std::function<void(void)>;

                                                ModbusPoller(boost::asio::io_service& io, const Endpoint& ep, const Interval& reconnectInterval);
                                               ~ModbusPoller();

    void                                        addPollTask(const Vector& req, Vector& rsp, const Interval& interval, PollCallback cb);
    void                                        start();
    void                                        cancel();

private:
    using Task                                  = ModbusPollTask<ModbusPoller>;
    using PTask                                 = std::shared_ptr<Task>;
    friend class ModbusPollTask<ModbusPoller>;

    SocketConnector                             m_connector;
    SocketConnector::Socket                     m_socket;
    bool                                        m_connected;
    std::set<PTask>                             m_tasks;
    std::list<PTask>                            m_taskQueue;

    void                                        initFirstConnection();
    void                                        onFirstTimeConnected();

    void                                        initReconnection();
    void                                        onReconnected();

    boost::asio::io_service&                    get_io_service();
    void                                        enque(PTask task);
    void                                        handleModbusDialogDone(const boost::system::error_code& ec);
    void                                        handleModbusDialogDone();
};


boost::asio::io_service& ModbusPoller::get_io_service() {
    return m_socket.get_io_service();
}


ModbusPoller::ModbusPoller(boost::asio::io_service& io, const Endpoint& ep, const Interval& reconnectInterval) :
    m_connector(io, ep, reconnectInterval),
    m_socket(io),
    m_connected(false),
    m_tasks(),
    m_taskQueue()
{
    std::cout << "ModbusPoller: " << this << std::endl;
}


ModbusPoller::~ModbusPoller() {
    std::cout << "~ModbusPoller: " << this << std::endl;
}


void ModbusPoller::addPollTask(const Vector& req, Vector& rsp, const Interval& interval, PollCallback cb) {
    auto task = std::make_shared<Task>(shared_from_this(), req, rsp, interval, cb);
    m_tasks.insert(task);
}


void ModbusPoller::start() {
    initFirstConnection();
}


void ModbusPoller::initFirstConnection() {
    auto self = shared_from_this();

    m_connector.async_connect(m_socket, [self, this](const boost::system::error_code& ec) {
        if (ec)
            return;

        onFirstTimeConnected();
    });
}


void ModbusPoller::onFirstTimeConnected() {
    m_connected = true;

    for (auto& task: m_tasks)
        task->start();
}


void ModbusPoller::enque(PTask task) {
    if (!m_connected)
        return;

    if (m_taskQueue.empty()) {
        m_taskQueue.push_back(task);
        task->asyncModbusDialog(&m_socket);
    } else
        m_taskQueue.push_back(task);
}


void ModbusPoller::handleModbusDialogDone(const boost::system::error_code& ec) {
    m_taskQueue.pop_front();

    if (ec == boost::asio::error::operation_aborted) {
        return;
    } else {
        m_connected = false;
        m_taskQueue.clear();
        initReconnection();
    }
}


void ModbusPoller::initReconnection() {
    auto self = shared_from_this();

    m_connector.async_connect(m_socket, [self, this](const boost::system::error_code& ec) {
        if (ec)
            return;

        onReconnected();
    });
   
}


void ModbusPoller::onReconnected() {
    m_connected = true;

    if (!m_taskQueue.empty())
        m_taskQueue.front()->asyncModbusDialog(&m_socket);
}


void ModbusPoller::handleModbusDialogDone() {
    m_taskQueue.pop_front();

    if (!m_taskQueue.empty())
        m_taskQueue.front()->asyncModbusDialog(&m_socket);
}


void ModbusPoller::cancel() {
    auto self = shared_from_this();

    m_socket.get_io_service().post([self, this] {
        m_connector.cancel();

        for (auto&task: m_tasks)
            task->cancel();

        m_socket.cancel();
        m_connected = false;

        m_tasks.clear();

        if (!m_taskQueue.empty()) {
            auto front = m_taskQueue.front();
            m_taskQueue.clear();
            m_taskQueue.push_back(front);
        } else
            m_taskQueue.clear();
    });
}

#endif

