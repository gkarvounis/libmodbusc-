#ifndef NO_ACTION_HPP
#define NO_ACTION_HPP

struct NoAction {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {}
};

#endif

