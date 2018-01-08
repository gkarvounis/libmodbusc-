#ifndef MODBUS_POLLER_HPP
#define MODBUS_POLLER_HPP


#include "SocketConnector.hpp"

class ModbusPoller {
public:
    using Interval = boost::posix_time::milliseconds;
    using Vector = std::vector<uint8_t>;
    using PollCallback = std::function<void(void)>;

                                                ModbusPoller(boost::asio::io_service& io, const SocketConnector::Endpoint& ep, const Interval& reconnectInterval);
    void                                        addPollTask(const Vector& req, Vector& rsp, const Interval& interval, PollCallback cb);
    void                                        start();
    void                                        cancel();

private:
    friend class PollTask;

    class PollTask : public std::enable_shared_from_this<PollTask> {
    public:
                                                PollTask(ModbusPoller* owner, const Vector& req, Vector& rsp, const Interval& interval, PollCallback pollCb);
        void                                    start();
        void                                    cancel();
        void                                    asyncModbusDialog(boost::asio::ip::tcp::socket* sock);

    private:
        ModbusPoller                           *m_owner;
        Vector                                  m_req;
        Vector                                 &m_rsp;
        Interval                                m_interval;
        PollCallback                            m_pollCb;
        boost::asio::deadline_timer             m_timer;
        boost::asio::ip::tcp::socket           *m_socket;

        void                                    initTimer();

        void                                    initHeaderReception();
        void                                    initPayloadReception();
        void                                    onPayloadReceived();

    };

    using PTask = std::shared_ptr<PollTask>;

    boost::asio::io_service                    &m_io;
    std::shared_ptr<SocketConnector>            m_connector;
    SocketConnector::PSocket                    m_socket;
    std::set<PTask>                             m_tasks;
    std::list<PTask>                            m_taskQueue;

    void                                        initFirstConnection();
    void                                        onFirstTimeConnected(SocketConnector::PSocket sock);

    void                                        initReconnection();
    void                                        onReconnected(SocketConnector::PSocket sock);

    boost::asio::io_service&                    get_io_service();
    void                                        enque(PTask task);
    void                                        handleModbusDialogDone(const boost::system::error_code& ec);
    void                                        handleModbusDialogDone();
};


ModbusPoller::PollTask::PollTask(ModbusPoller* owner, const Vector& req, Vector& rsp, const Interval& interval, PollCallback pollCb) :
    m_req(req),
    m_rsp(rsp),
    m_interval(interval),
    m_pollCb(pollCb),
    m_timer(owner->get_io_service())
{}


void ModbusPoller::PollTask::start() {
    m_owner->enque(shared_from_this());
    initTimer();
}


void ModbusPoller::PollTask::initTimer() {
    auto self = shared_from_this();

    m_timer.expires_from_now(m_interval);
    m_timer.async_wait([self, this](const boost::system::error_code& ec) {
        if (ec)
            return;

        m_owner->enque(self);
        initTimer();
    });
}


void ModbusPoller::PollTask::asyncModbusDialog(boost::asio::ip::tcp::socket* sock) {
    auto self = shared_from_this();
    m_socket = sock;

    boost::asio::async_write(*m_socket, boost::asio::buffer(m_req), [self, this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec)
            m_owner->handleModbusDialogDone(ec);
        else
            initHeaderReception();
    });
}


void ModbusPoller::PollTask::initHeaderReception() {
    m_rsp.resize(sizeof(modbus::tcp::Header));

    boost::asio::async_read(*m_socket, boost::asio::buffer(m_rsp), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec)
            m_owner->handleModbusDialogDone(ec);
        else
            initPayloadReception();        
    });
}


void ModbusPoller::PollTask::initPayloadReception() {
    modbus::tcp::decoder_views::Header rsp_header_view(m_rsp);
    std::size_t payloadSize = rsp_header_view.getLength() - 2;
    
    m_rsp.resize(sizeof(modbus::tcp::Header) + payloadSize);

    uint8_t* payload = m_rsp.data() + sizeof(modbus::tcp::Header);

    boost::asio::async_read(*m_socket, boost::asio::buffer(payload, payloadSize), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec)
            m_owner->handleModbusDialogDone(ec);
        else
            onPayloadReceived();
    });
}


void ModbusPoller::PollTask::onPayloadReceived() {
    m_pollCb();
    m_owner->handleModbusDialogDone();
}


void ModbusPoller::PollTask::cancel() {
    m_timer.cancel();
    m_pollCb = nullptr;
}


boost::asio::io_service& ModbusPoller::get_io_service() {
    return m_io;
}


ModbusPoller::ModbusPoller(boost::asio::io_service& io, const SocketConnector::Endpoint& ep, const Interval& reconnectInterval) :
    m_io(io),
    m_connector(std::make_shared<SocketConnector>(io, ep, reconnectInterval)),
    m_socket(nullptr),
    m_tasks()
{}


void ModbusPoller::addPollTask(const Vector& req, Vector& rsp, const Interval& interval, PollCallback cb) {
    auto task = std::make_shared<PollTask>(this, req, rsp, interval, cb);  
    m_tasks.insert(task);
}


void ModbusPoller::start() {
    initFirstConnection();
}


void ModbusPoller::initFirstConnection() {
    m_connector->async_connect([this](SocketConnector::PSocket sock) {
        onFirstTimeConnected(std::move(sock));
    });
}


void ModbusPoller::onFirstTimeConnected(SocketConnector::PSocket sock) {
    if (sock.get() == nullptr)
        return;

    m_socket.swap(sock);

    for (auto& task: m_tasks) {
        task->start();
    }
}


void ModbusPoller::enque(std::shared_ptr<PollTask> task) {
    if (m_socket.get() == nullptr)
        return;

    if (m_taskQueue.empty()) {
        m_taskQueue.push_back(task);
        task->asyncModbusDialog(m_socket.get());
    } else
        m_taskQueue.push_back(task);
}


void ModbusPoller::initReconnection() {
     m_connector->async_connect([this](SocketConnector::PSocket sock) {
        onReconnected(std::move(sock));
    });
   
}


void ModbusPoller::onReconnected(SocketConnector::PSocket sock) {
    m_socket.swap(sock);

    if (!m_taskQueue.empty())
        m_taskQueue.front()->asyncModbusDialog(m_socket.get());
}


void ModbusPoller::handleModbusDialogDone(const boost::system::error_code& ec) {
    m_taskQueue.pop_front();

    if (ec == boost::asio::error::operation_aborted) {
        return;
    } else if (ec) {
        m_socket.reset(nullptr);
        m_taskQueue.clear();
        initReconnection();
    } else {
        if (!m_taskQueue.empty())
            m_taskQueue.front()->asyncModbusDialog(m_socket.get());
    }

}


void ModbusPoller::handleModbusDialogDone() {
    if (!m_taskQueue.empty())
        m_taskQueue.front()->asyncModbusDialog(m_socket.get());
}


void ModbusPoller::cancel() {
    m_io.post([this] {
        m_connector->cancel();

        for (auto&task: m_tasks)
            task->cancel();

        m_tasks.clear();
        m_taskQueue.clear();

        if (m_socket.get() != nullptr) {
            m_socket->cancel();
            m_socket.reset();
        }
    });
}

#endif

