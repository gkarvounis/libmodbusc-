#ifndef SOCKET_CONNECTOR_FSM_ACTIONS_HPP
#define SOCKET_CONNECTOR_FSM_ACTIONS_HPP

namespace socket_connector_detail {

struct ActInitConnection {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm&, SourceState&, const Event&, TargetState&);
};


struct ActInitRetryTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm&, SourceState&, const Event&, TargetState&);
};


struct ActPostCallback {
public:
    template <typename Fsm>
    void operator()(Fsm& fsm, StConnecting& source, const EvtConnected& evt, StIdle& target);

    template <typename Fsm>
    void operator()(Fsm& fsm, StWaitingTimerAfterCancel& source, const EvtTimerExpired& evt, StIdle& target);

    template <typename Fsm>
    void operator()(Fsm& fsm, StWaitingSocketAfterCancel& source, const EvtConnected& evt, StIdle& target);

private:
    template <typename FSM>
    void post(FSM& fsm, const boost::system::error_code& ec);
};


struct ActCancelSocket {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm&, SourceState&, const Event&, TargetState&);
};


struct ActCancelTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm&, SourceState&, const Event&, TargetState&);
};


template <typename Fsm, typename SourceState, typename Event, typename TargetState>
void ActInitConnection::operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
    auto& userData = fsm.userData();
    
    std::cout << "Attempting connection to " << userData.m_endpoint << std::endl;

    userData.m_socket->async_connect(userData.m_endpoint, [&fsm](const boost::system::error_code& ec) {
        std::cout << "on connected ec=" << ec.message() << std::endl;
        fsm.process_event(EvtConnected(ec));
    });
}


template <typename Fsm>
void ActPostCallback::operator()(Fsm& fsm, StConnecting& /*source*/, const EvtConnected&, StIdle& /*target*/) {
    post(fsm, boost::system::error_code());
}


template <typename Fsm>
void ActPostCallback::operator()(Fsm& fsm, StWaitingTimerAfterCancel& /*source*/, const EvtTimerExpired&, StIdle& /*target*/) {
    post(fsm, boost::asio::error::operation_aborted);
}


template <typename Fsm>
void ActPostCallback::operator()(Fsm& fsm, StWaitingSocketAfterCancel& /*source*/, const EvtConnected& evt, StIdle& /*target*/) {
    if (evt.ec)
        post(fsm, boost::asio::error::operation_aborted);
    else
        post(fsm, evt.ec);
}


template <typename FSM>
void ActPostCallback::post(FSM& fsm, const boost::system::error_code& ec) {
    auto& userData = fsm.userData();

    auto cb = std::move(userData.m_connectedCb);
    userData.m_connectedCb = nullptr;

    userData.m_socket->get_io_service().post([cb, ec]() {
        cb(ec);
    });
}


template <typename Fsm, typename SourceState, typename Event, typename TargetState>
void ActInitRetryTimer::operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
    auto& userData = fsm.userData();

    userData.m_timer.expires_from_now(userData.m_interval);

    userData.m_timer.async_wait([&fsm](const boost::system::error_code& ec) {
        fsm.process_event(EvtTimerExpired(ec));
    });
}




template <typename Fsm, typename SourceState, typename Event, typename TargetState>
void ActCancelSocket::operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
    auto& userData = fsm.userData();
    userData.m_socket->cancel();
}


template <typename Fsm, typename SourceState, typename Event, typename TargetState>
void ActCancelTimer::operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
    auto& userData = fsm.userData();
    userData.m_timer.cancel();
}

} // namespace socket_connector_detail
#endif

