#ifndef MODBUS_POLL_TASK_FSM_STATES_HPP
#define MODBUS_POLL_TASK_FSM_STATES_HPP

namespace modbus_poll_task_detail {
    struct StIdle: public boost::msm::front::state<> {};



    struct StInQueue_EntryAction {
        template <class Event, class FSM, class State>
        void operator()(const Event&, FSM&, State&) {
        }
    };

    struct StInQueue_tag{};
    struct StInQueue : public boost::msm::front::euml::func_state<StInQueue_tag, StInQueue_EntryAction, State_NoExitAction> {};



    struct StSending: public boost::msm::front::state<> {};
    struct StWaitingHeader: public boost::msm::front::state<> {};
    struct StWaitingPayload: public boost::msm::front::state<> {};
    struct StWaitingTimer: public boost::msm::front::state<> {};
    struct StStopped: public boost::msm::front::state<> {};



    struct StCancelingSocket_EntryAction {
        template <class Event, class FSM, class State>
        void operator()(const Event&, FSM&, State&) {
        }
    };

    struct StCancelingSocket_tag{};
    struct StCancelingSocket : public boost::msm::front::euml::func_state<StCancelingSocket_tag, StCancelingSocket_EntryAction, State_NoExitAction> {};


    struct StCancelingTimer: public boost::msm::front::state<> {};
} // namespace modbus_poll_task_detail

#endif

