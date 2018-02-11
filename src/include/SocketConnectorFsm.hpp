#ifndef SOCKET_CONNECTOR_FSM_HPP
#define SOCKET_CONNECTOR_FSM_HPP

namespace socket_connector_detail {

struct UserData {
    using Socket                                = boost::asio::ip::tcp::socket;
    using Endpoint                              = boost::asio::ip::tcp::endpoint;
    using Interval                              = boost::posix_time::milliseconds;
    using SocketConnectedCb                     = std::function<void(const boost::system::error_code& ec)>;

    inline                                      UserData(boost::asio::io_service& io);
    inline                                      UserData(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval);

    void                                        set_connect_params(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb);
    void                                        set_connect_params(Socket& sock, SocketConnectedCb cb);

    template <typename Fsm>
    void                                        initConnection(Fsm& fsm);

    template <typename Fsm>
    void                                        initTimer(Fsm& fsm);

    void                                        postCallback(const boost::system::error_code& ec);

    void                                        cancelSocket();
    void                                        cancelTimer();

    boost::asio::deadline_timer                 m_timer;
    Interval                                    m_interval;
    SocketConnectedCb                           m_connectedCb;
    Endpoint                                    m_endpoint;
    Socket                                     *m_socket;
};


template <typename Fsm>
void UserData::initConnection(Fsm& fsm) {
    std::cout << "Attempting connection to " << m_endpoint << std::endl;

    m_socket->async_connect(m_endpoint, [&fsm](const boost::system::error_code& ec) {
        std::cout << "on connected ec=" << ec.message() << std::endl;
        fsm.process_event(EvtConnected(ec));
    });
}


void UserData::postCallback(const boost::system::error_code& ec) {
    auto cb = std::move(m_connectedCb);
    m_connectedCb = nullptr;

    m_socket->get_io_service().post([cb, ec]() {
        cb(ec);
    });
}


template <typename Fsm>
void UserData::initTimer(Fsm& fsm) {
    m_timer.expires_from_now(m_interval);

    m_timer.async_wait([&fsm](const boost::system::error_code& ec) {
        fsm.process_event(EvtTimerExpired(ec));
    });
}


void UserData::cancelTimer() {
    m_timer.cancel();
}


void UserData::cancelSocket() {
    m_socket->cancel();
}


struct FsmDef {
    using States = std::tuple<
        StIdle,
        StConnecting,
        StWaitingRetryTimer,
        StWaitingSocketAfterCancel,
        StWaitingTimerAfterCancel
    >;

    using InitialState = StIdle;

    using Transitions = std::tuple<
        std::tuple< StIdle,                     EvtCancel,           fsm::NoGuard,               StIdle,                         fsm::NoAction             >,
        std::tuple< StIdle,                     EvtInitConnection,   fsm::NoGuard,               StConnecting,                   ActInitConnection         >,
        std::tuple< StConnecting,               EvtConnected,        AsioOperationSuccesfull,    StIdle,                         ActPostCallbackSuccess    >,
        std::tuple< StConnecting,               EvtConnected,        AsioOperationFailed,        StWaitingRetryTimer,            ActInitRetryTimer         >,
        std::tuple< StConnecting,               EvtCancel,           fsm::NoGuard,               StWaitingSocketAfterCancel,     ActCancelSocket           >,
        std::tuple< StWaitingRetryTimer,        EvtTimerExpired,     AsioOperationSuccesfull,    StConnecting,                   ActInitConnection         >,
        std::tuple< StWaitingRetryTimer,        EvtCancel,           fsm::NoGuard,               StWaitingTimerAfterCancel,      ActCancelTimer            >,
        std::tuple< StWaitingTimerAfterCancel,  EvtTimerExpired,     fsm::NoGuard,               StIdle,                         ActPostCallbackAborted    >,
        std::tuple< StWaitingSocketAfterCancel, EvtConnected,        AsioOperationSuccesfull,    StIdle,                         ActPostCallbackSuccess    >,
        std::tuple< StWaitingSocketAfterCancel, EvtConnected,        AsioOperationFailed,        StIdle,                         ActPostCallbackFailed     >,
        std::tuple< StWaitingSocketAfterCancel, EvtConnected,        AsioOperationAborted,       StIdle,                         ActPostCallbackAborted    >
    >;

    template <typename Fsm, typename State>
    static void entryAction(Fsm&, State&) {
    }


    template <typename Fsm, typename State>
    static void exitAction(Fsm&, State&) {
    }
};


UserData::UserData(boost::asio::io_service& io) :
    m_timer(io),
    m_interval(0),
    m_connectedCb(),
    m_endpoint(),
    m_socket(nullptr)
{}


UserData::UserData(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval) :
    m_timer(io),
    m_interval(interval),
    m_connectedCb(),
    m_endpoint(ep),
    m_socket(nullptr)
{}


void UserData::set_connect_params(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb) {
    m_socket = &sock;
    m_endpoint = ep;
    m_interval = interval;
    m_connectedCb = cb;
}


void UserData::set_connect_params(Socket& sock, SocketConnectedCb cb) {
    m_socket = &sock;
    m_connectedCb = cb;
}

} // namespace socket_connector_detail
#endif

