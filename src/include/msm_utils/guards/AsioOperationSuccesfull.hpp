#ifndef ASIO_OPERATION_SUCCESFULL_HPP
#define ASIO_OPERATION_SUCCESFULL_HPP

struct AsioOperationSuccesfull {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    bool operator()(const Event& evt, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {
        if (!evt.ec)
            return true;
        else 
            return false;
    }
};


#endif

