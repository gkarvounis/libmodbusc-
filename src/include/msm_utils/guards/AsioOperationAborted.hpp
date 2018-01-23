#ifndef ASIO_OPERATION_ABORTED_HPP
#define ASIO_OPERATION_ABORTED_HPP

struct AsioOperationAborted {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    bool operator()(const Event& evt, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {
        return evt.ec == boost::asio::error::operation_aborted;
    }
};

#endif

