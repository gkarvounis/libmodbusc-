#ifndef ASIO_OPERATION_SUCCESFULL_HPP
#define ASIO_OPERATION_SUCCESFULL_HPP

struct AsioOperationSuccesfull {
    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    bool operator()(Fsm&, FromStateType&, const EventType& evt, TargetStateType&) {
        if (!evt.ec)
            return true;
        else 
            return false;
    }
};


#endif

