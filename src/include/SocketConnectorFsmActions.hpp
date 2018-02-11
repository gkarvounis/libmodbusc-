#ifndef SOCKET_CONNECTOR_FSM_ACTIONS_HPP
#define SOCKET_CONNECTOR_FSM_ACTIONS_HPP

namespace socket_connector_detail {

struct ActInitConnection {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().initConnection(fsm);
    }
};


struct ActInitRetryTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().initTimer(fsm);
    }
};


struct ActPostCallbackSuccess {
public:
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().postCallback(boost::system::error_code());
    }
};

struct ActPostCallbackAborted {
public:
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().postCallback(boost::system::error_code(boost::asio::error::operation_aborted));
    }
};


struct ActPostCallbackFailed {
public:
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event& evt, TargetState&) {
        fsm.userData().postCallback(evt.ec);
    }

};


struct ActCancelSocket {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().cancelSocket();
    }
};


struct ActCancelTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState&, const Event&, TargetState&) {
        fsm.userData().cancelTimer();
    }
};

} // namespace socket_connector_detail
#endif

