#ifndef MODBUS_POLL_TASK_HPP
#define MODBUS_POLL_TASK_HPP


template <typename ModbusPoller>
class ModbusPollTask : public std::enable_shared_from_this<ModbusPollTask<ModbusPoller>> {
public:
    using Interval                          = boost::posix_time::milliseconds;
    using Vector                            = std::vector<uint8_t>;
    using PollCallback                      = std::function<void(void)>;

                                            ModbusPollTask(ModbusPoller* owner, const Vector& req, Vector& rsp, const Interval& interval, PollCallback pollCb);
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


template <typename ModbusPoller>
ModbusPollTask<ModbusPoller>::ModbusPollTask(ModbusPoller* owner, const Vector& req, Vector& rsp, const Interval& interval, PollCallback pollCb) :
    m_req(req),
    m_rsp(rsp),
    m_interval(interval),
    m_pollCb(pollCb),
    m_timer(owner->get_io_service())
{}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::start() {
    m_owner->enque(this->shared_from_this());
    initTimer();
}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::initTimer() {
    auto self = this->shared_from_this();

    m_timer.expires_from_now(m_interval);
    m_timer.async_wait([self, this](const boost::system::error_code& ec) {
        if (ec)
            return;

        m_owner->enque(self);
        initTimer();
    });
}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::asyncModbusDialog(boost::asio::ip::tcp::socket* sock) {
    auto self = this->shared_from_this();
    m_socket = sock;

    boost::asio::async_write(*m_socket, boost::asio::buffer(m_req), [self, this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec)
            m_owner->handleModbusDialogDone(ec);
        else
            initHeaderReception();
    });
}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::initHeaderReception() {
    m_rsp.resize(sizeof(modbus::tcp::Header));

    boost::asio::async_read(*m_socket, boost::asio::buffer(m_rsp), [this](const boost::system::error_code& ec, std::size_t /* bytes_transferred */) {
        if (ec)
            m_owner->handleModbusDialogDone(ec);
        else
            initPayloadReception();        
    });
}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::initPayloadReception() {
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


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::onPayloadReceived() {
    m_pollCb();
    m_owner->handleModbusDialogDone();
}


template <typename ModbusPoller>
void ModbusPollTask<ModbusPoller>::cancel() {
    m_timer.cancel();
    m_pollCb = nullptr;
}



#endif

