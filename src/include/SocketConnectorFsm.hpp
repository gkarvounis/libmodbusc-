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

    boost::asio::deadline_timer                 m_timer;
    Interval                                    m_interval;
    SocketConnectedCb                           m_connectedCb;
    Endpoint                                    m_endpoint;
    Socket                                     *m_socket;
};


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
        std::tuple< StIdle,                     EvtCancel,           fsm::NoGuard,               StIdle,                         fsm::NoAction      >,
        std::tuple< StIdle,                     EvtInitConnection,   fsm::NoGuard,               StConnecting,                   ActInitConnection  >,
        std::tuple< StConnecting,               EvtConnected,        AsioOperationSuccesfull,    StIdle,                         ActPostCallback    >,
        std::tuple< StConnecting,               EvtConnected,        AsioOperationFailed,        StWaitingRetryTimer,            ActInitRetryTimer  >,
        std::tuple< StConnecting,               EvtCancel,           fsm::NoGuard,               StWaitingSocketAfterCancel,     ActCancelSocket    >,
        std::tuple< StWaitingSocketAfterCancel, EvtConnected,        fsm::NoGuard,               StIdle,                         ActPostCallback    >,
        std::tuple< StWaitingRetryTimer,        EvtTimerExpired,     AsioOperationSuccesfull,    StConnecting,                   ActInitConnection  >,
        std::tuple< StWaitingRetryTimer,        EvtCancel,           fsm::NoGuard,               StWaitingTimerAfterCancel,      ActCancelTimer     >,
        std::tuple< StWaitingTimerAfterCancel,  EvtTimerExpired,     fsm::NoGuard,               StIdle,                         ActPostCallback    >
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

