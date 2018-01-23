#ifndef SOCKET_CONNECTOR_FSM_ACTIONS_HPP
#define SOCKET_CONNECTOR_FSM_ACTIONS_HPP

namespace socket_connector_detail {

struct ActInitConnection {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/);
};


struct ActInitRetryTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/);
};


struct ActPostCallback {
public:
    template <typename FSM>
    void operator()(const EvtConnected& evt, FSM& fsm, StConnecting& source, StIdle& target);

    template <typename FSM>
    void operator()(const EvtTimerExpired& evt, FSM& fsm, StWaitingTimerAfterCancel& source, StIdle& target);

    template <typename FSM>
    void operator()(const EvtConnected& evt, FSM& fsm, StWaitingSocketAfterCancel& source, StIdle& target);

private:
    template <typename FSM>
    void post(FSM& fsm, const boost::system::error_code& ec);
};


struct ActCancelSocket {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/);
};


struct ActCancelTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/);
};


struct NoAction {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};



template <typename Event, typename FSM, typename SourceState, typename TargetState>
void ActInitConnection::operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
    std::cout << "Attempting connection to " << fsm.m_endpoint << std::endl;

    fsm.m_socket->async_connect(fsm.m_endpoint, [&fsm](const boost::system::error_code& ec) {
        std::cout << "on connected ec=" << ec.message() << std::endl;
        fsm.process_event(EvtConnected(ec));
    });
}


template <typename Event, typename FSM, typename SourceState, typename TargetState>
void ActInitRetryTimer::operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
    fsm.m_timer.expires_from_now(fsm.m_interval);

    fsm.m_timer.async_wait([&fsm](const boost::system::error_code& ec) {
        fsm.process_event(EvtTimerExpired(ec));
    });
}


template <typename FSM>
void ActPostCallback::operator()(const EvtConnected& /*evt*/, FSM& fsm, StConnecting& /*source*/, StIdle& /*target*/) {
    post(fsm, boost::system::error_code());
}


template <typename FSM>
void ActPostCallback::operator()(const EvtTimerExpired& /*evt*/, FSM& fsm, StWaitingTimerAfterCancel& /*source*/, StIdle& /*target*/) {
    post(fsm, boost::asio::error::operation_aborted);
}


template <typename FSM>
void ActPostCallback::operator()(const EvtConnected& evt, FSM& fsm, StWaitingSocketAfterCancel& /*source*/, StIdle& /*target*/) {
    if (evt.ec)
        post(fsm, boost::asio::error::operation_aborted);
    else
        post(fsm, evt.ec);
}


template <typename FSM>
void ActPostCallback::post(FSM& fsm, const boost::system::error_code& ec) {
    auto cb = std::move(fsm.m_connectedCb);
    fsm.m_connectedCb = nullptr;

    fsm.m_socket->get_io_service().post([cb, ec]() {
        cb(ec);
    });
}


template <typename Event, typename FSM, typename SourceState, typename TargetState>
void ActCancelSocket::operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
    fsm.m_socket->cancel();
}


template <typename Event, typename FSM, typename SourceState, typename TargetState>
void ActCancelTimer::operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
    fsm.m_timer.cancel();
}

} // namespace socket_connector_detail
#endif

