#ifndef MDOBUS_POLL_TASK_FSM_ACTIONS_HPP
#define MODBUS_POLL_TASK_FSM_ACTIONS_HPP

namespace modbus_poll_task_detail {

struct ActInitSending {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

struct ActInitHeaderReception {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

struct ActInitPayloadReception {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

struct ActInitTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

struct ActCancelTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};


struct ActNotifySocketFailed {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

struct ActNotifyPollStopped {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

} // namespace modbus_poll_task_detail

#endif

