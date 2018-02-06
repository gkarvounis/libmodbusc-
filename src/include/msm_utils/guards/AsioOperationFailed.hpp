#ifndef ASIO_OPERATION_FAILED_HPP
#define ASIO_OPERATION_FAILED_HPP

struct AsioOperationFailed {
    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    bool operator()(Fsm&, FromStateType&, const EventType& evt, TargetStateType&) {
        return evt.ec && (evt.ec != boost::asio::error::operation_aborted);
    }
};


#endif

