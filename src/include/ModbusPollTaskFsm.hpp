#ifndef MODBUS_POLL_TASK_FSM_HPP
#define MODBUS_POLL_TASK_FSM_HPP

namespace modbus_poll_task_detail {

struct FsmDef : public boost::msm::front::state_machine_def<FsmDef> {
    using Socket                                = boost::asio::ip::tcp::socket;
    using Interval                              = boost::posix_time::milliseconds;

    typedef StIdle initial_state;

    struct transition_table : public boost::mpl::vector<
        boost::msm::front::Row< StIdle,             EvtStart,           StInQueue,          NoAction,                   NoGuard>,
        boost::msm::front::Row< StInQueue,          EvtExecOnePoll,     StSending,          ActInitSending,             NoGuard>,
        boost::msm::front::Row< StSending,          EvtSent,            StWaitingHeader,    ActInitHeaderReception,     AsioOperationSuccesfull>,
        boost::msm::front::Row< StWaitingHeader,    EvtReceived,        StWaitingPayload,   ActInitPayloadReception,    AsioOperationSuccesfull>,
        boost::msm::front::Row< StWaitingPayload,   EvtReceived,        StWaitingTimer,     ActInitTimer,               AsioOperationSuccesfull>,
        boost::msm::front::Row< StWaitingTimer,     EvtTimerExpired,    StInQueue,          NoAction,                   AsioOperationSuccesfull>,

        boost::msm::front::Row< StSending,          EvtSent,            StStopped,          ActNotifySocketFailed,      AsioOperationFailed>,
        boost::msm::front::Row< StWaitingHeader,    EvtReceived,        StStopped,          ActNotifySocketFailed,      AsioOperationFailed>,
        boost::msm::front::Row< StWaitingPayload,   EvtReceived,        StStopped,          ActNotifySocketFailed,      AsioOperationFailed>,

        boost::msm::front::Row< StStopped,          EvtCancel,          StStopped,          NoAction,                   NoGuard>,
        boost::msm::front::Row< StSending,          EvtCancel,          StCancelingSocket,  NoAction,                   NoGuard>,
        boost::msm::front::Row< StWaitingHeader,    EvtCancel,          StCancelingSocket,  NoAction,                   NoGuard>,
        boost::msm::front::Row< StWaitingPayload,   EvtCancel,          StCancelingSocket,  NoAction,                   NoGuard>,
        boost::msm::front::Row< StWaitingTimer,     EvtCancel,          StCancelingTimer,   ActCancelTimer,             NoGuard>,

        boost::msm::front::Row< StCancelingSocket,  EvtSent,            StStopped,          ActNotifyPollStopped,       NoGuard>,
        boost::msm::front::Row< StCancelingSocket,  EvtReceived,        StStopped,          ActNotifyPollStopped,       NoGuard>,
        boost::msm::front::Row< StCancelingTimer,   EvtTimerExpired,    StStopped,          ActNotifyPollStopped,       NoGuard>
        
    > {};
};


} // namespace modbus_poll_task_detail

#endif

