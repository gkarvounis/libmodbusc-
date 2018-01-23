#ifndef SOCKET_CONNECTOR_FSM_HPP
#define SOCKET_CONNECTOR_FSM_HPP

namespace socket_connector_detail {

struct FsmDef : public boost::msm::front::state_machine_def<FsmDef> {
    using Socket                                = boost::asio::ip::tcp::socket;
    using Endpoint                              = boost::asio::ip::tcp::endpoint;
    using Interval                              = boost::posix_time::milliseconds;
    using SocketConnectedCb                     = std::function<void(const boost::system::error_code& ec)>;

                                                FsmDef(boost::asio::io_service* io);
                                                FsmDef(boost::asio::io_service* io, const Endpoint& ep, const Interval& interval);
    boost::asio::io_service&                    get_io_service();
    void                                        set_connect_params(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb);
    void                                        set_connect_params(Socket& sock, SocketConnectedCb cb);


    typedef StIdle initial_state;

    struct transition_table : public boost::mpl::vector<
        boost::msm::front::Row< StIdle,                     EvtCancel,              StIdle,                         NoAction,               NoGuard>,
        boost::msm::front::Row< StIdle,                     EvtInitConnection,      StConnecting,                   ActInitConnection,      NoGuard>,
        boost::msm::front::Row< StConnecting,               EvtConnected,           StIdle,                         ActPostCallback,        AsioOperationSuccesfull>,
        boost::msm::front::Row< StConnecting,               EvtConnected,           StWaitingRetryTimer,            ActInitRetryTimer,      AsioOperationFailed>,
        boost::msm::front::Row< StConnecting,               EvtCancel,              StWaitingSocketAfterCancel,     ActCancelSocket,        NoGuard>,
        boost::msm::front::Row< StWaitingSocketAfterCancel, EvtConnected,           StIdle,                         ActPostCallback,        NoGuard>,
        boost::msm::front::Row< StWaitingRetryTimer,        EvtTimerExpired,        StConnecting,                   ActInitConnection,      AsioOperationSuccesfull>,
        boost::msm::front::Row< StWaitingRetryTimer,        EvtCancel,              StWaitingTimerAfterCancel,      ActCancelTimer,         NoGuard>,
        boost::msm::front::Row< StWaitingTimerAfterCancel,  EvtTimerExpired,        StIdle,                         ActPostCallback,        NoGuard>
    > {};


    // Replaces the default no-transition response.
    template <typename FSM, typename Event>
    void no_transition(Event const& e, FSM&,int state);


    boost::asio::deadline_timer                 m_timer;
    Interval                                    m_interval;
    SocketConnectedCb                           m_connectedCb;
    Endpoint                                    m_endpoint;
    Socket                                     *m_socket;
};


FsmDef::FsmDef(boost::asio::io_service* io) :
    boost::msm::front::state_machine_def<FsmDef>(),
    m_timer(*io),
    m_interval(0),
    m_connectedCb(),
    m_endpoint(),
    m_socket(nullptr)
{}


FsmDef::FsmDef(boost::asio::io_service* io, const Endpoint& ep, const Interval& interval) :
    boost::msm::front::state_machine_def<FsmDef>(),
    m_timer(*io),
    m_interval(interval),
    m_connectedCb(),
    m_endpoint(ep),
    m_socket(nullptr)
{}


boost::asio::io_service& FsmDef::get_io_service() {
    return m_timer.get_io_service();
}


void FsmDef::set_connect_params(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb) {
    m_socket = &sock;
    m_endpoint = ep;
    m_interval = interval;
    m_connectedCb = cb;
}


void FsmDef::set_connect_params(Socket& sock, SocketConnectedCb cb) {
    m_socket = &sock;
    m_connectedCb = cb;
}


template <typename FSM, typename Event>
void FsmDef::no_transition(Event const& e, FSM& fsm, int state) {
    static char const* const state_names[] = { "StIdle", "StConnecting", "StWaitingSocketAterCancel", "StWaitingRetryTimer", "StWaitingTimerAfterCancel" };

    std::cout << "no transition from state " << state << "(" << state_names[fsm.current_state()[0]] << ")"
        << " on event " << typeid(e).name() << std::endl;

    exit(1);
}

} // namespace socket_connector_detail

#endif

