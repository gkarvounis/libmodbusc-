#ifndef ASIO_OPERATION_FAILED_HPP
#define ASIO_OPERATION_FAILED_HPP

struct AsioOperationFailed {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    bool operator()(const Event& evt, FSM& /*fsm*/, SourceState& /*source*/, TargetState& /*target*/) {
        return evt.ec && (evt.ec != boost::asio::error::operation_aborted);
    }
};


#endif

