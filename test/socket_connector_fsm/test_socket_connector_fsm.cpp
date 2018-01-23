#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include <boost/asio.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

#include "msm_utils/guards/NoGuard.hpp"
#include "msm_utils/guards/AsioOperationSuccesfull.hpp"
#include "msm_utils/guards/AsioOperationFailed.hpp"
#include "msm_utils/guards/AsioOperationAborted.hpp"

#include "msm_utils/events/EvtInitConnection.hpp"
#include "msm_utils/events/EvtConnected.hpp"
#include "msm_utils/events/EvtTimerExpired.hpp"
#include "msm_utils/events/EvtCancel.hpp"

#include "SocketConnectorFsmStates.hpp"


boost::asio::ip::tcp::endpoint make_endpoint(const std::string& addr, uint16_t port) {
    return boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(addr), port);
}

//#include "SocketConnectorFsmActions.hpp"
namespace socket_connector_detail {

struct ActInitConnection {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
        fsm.push_executed_action("ActInitConnection");
    }
};


struct ActInitRetryTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
        fsm.push_executed_action("ActInitRetryTimer");
    }
};

struct ActPostCallback {
public:
    template <typename FSM>
    void operator()(const EvtConnected& /*evt*/, FSM& fsm, StConnecting& /*source*/, StIdle& /*target*/) {
        fsm.push_executed_action("ActPostCallback - StConnecting->StIdle");
    }

    template <typename FSM>
    void operator()(const EvtTimerExpired& /*evt*/, FSM& fsm, StWaitingTimerAfterCancel& /*source*/, StIdle& /*target*/) {
        fsm.push_executed_action("ActPostCallback - StWaitingTimerAfterCancel->StIdle");
    }

    template <typename FSM>
    void operator()(const EvtConnected& /*evt*/, FSM& fsm, StWaitingSocketAfterCancel& /*source*/, StIdle& /*target*/) {
        fsm.push_executed_action("ActPostCallback - StWaitingSocketAfterCancel->StIdle");
    }
};


struct ActCancelSocket {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
        fsm.push_executed_action("ActCancelSocket");
    }
};


struct ActCancelTimer {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
        fsm.push_executed_action("ActCancelTimer");
    }
};

struct NoAction {
    template <typename Event, typename FSM, typename SourceState, typename TargetState>
    void operator()(const Event& /*evt*/, FSM& fsm, SourceState& /*source*/, TargetState& /*target*/) {
        fsm.push_executed_action("NoAction");
    }
};

} // namespace socket_connector_detail


#include "SocketConnectorFsm.hpp"

class FsmDefEx : public socket_connector_detail::FsmDef {
public:
    FsmDefEx(boost::asio::io_service* io) : socket_connector_detail::FsmDef(io) {}

    void push_executed_action(const std::string& action) {
        executed_actions.push_back(action);
    }

    std::vector<std::string> executed_actions;
};

using SocketConnectorFsm = boost::msm::back::state_machine<FsmDefEx>;


TEST_CASE("Two consecutive connection requests that complete succesfully", "SocketConnectorFsmDef") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);

    SocketConnectorFsm fsm(&io);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));
    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));

    REQUIRE(fsm.executed_actions == (std::vector<std::string>{
        "ActInitConnection", 
        "ActPostCallback - StConnecting->StIdle",
        "ActInitConnection", 
        "ActPostCallback - StConnecting->StIdle"
    }));
}


TEST_CASE("One connection request that is canceled by user before completing", "SocketConnectorFsmDef") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);

    SocketConnectorFsm fsm(&io);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtConnected(boost::asio::error::operation_aborted));

    REQUIRE(fsm.executed_actions == (std::vector<std::string>{
        "ActInitConnection",
        "ActCancelSocket",
        "ActPostCallback - StWaitingSocketAfterCancel->StIdle"
    }));
}


TEST_CASE("One connection request that is canceled right after completing completing", "SocketConnectorFsmDef") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);

    SocketConnectorFsm fsm(&io);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtConnected(boost::system::error_code()));

    REQUIRE(fsm.executed_actions == (std::vector<std::string>{
        "ActInitConnection",
        "ActCancelSocket",
        "ActPostCallback - StWaitingSocketAfterCancel->StIdle"
    }));
}

TEST_CASE("One connection request that fails and user cancels before timer expiration", "SocketConnectorFsmDef") {
    boost::asio::io_service io;
    boost::asio::ip::tcp::socket sock(io);

    SocketConnectorFsm fsm(&io);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::asio::error::connection_refused));
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtTimerExpired(boost::asio::error::operation_aborted));

    REQUIRE(fsm.executed_actions == (std::vector<std::string>{
        "ActInitConnection",
        "ActInitRetryTimer",
        "ActCancelTimer",
        "ActPostCallback - StWaitingTimerAfterCancel->StIdle"
    }));
}
