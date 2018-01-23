#ifndef NO_GUARD_HPP
#define NO_GUARD_HPP

struct NoGuard {
    template <typename Evt, typename FSM, typename FromState, typename ToState>
    bool operator()(const Evt& /*evt*/, FSM& /*fsm*/, FromState& /*source*/, ToState& /*target*/) {
        return true;
    }
};

#endif

