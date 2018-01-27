#ifndef STATE_NO_ACTION_HPP
#define STATE_NO_ACTION_HPP

struct State_NoExitAction {
    template <class Event, class FSM, class State>
    void operator()(const Event&, FSM&, State&) {
    }
};

#endif

