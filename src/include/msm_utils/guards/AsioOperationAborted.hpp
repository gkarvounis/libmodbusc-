#ifndef ASIO_OPERATION_ABORTED_HPP
#define ASIO_OPERATION_ABORTED_HPP

struct AsioOperationAborted {
    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    bool operator()(Fsm&, FromStateType&, const EventType& evt, TargetStateType&) {
        return evt.ec == boost::asio::error::operation_aborted;
    }
};

#endif

