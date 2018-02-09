#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include <boost/asio.hpp>

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


namespace socket_connector_detail {

struct ActInitConnection {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
        fsm.userData().push_back("ActInitConnection");
    }
};


struct ActInitRetryTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
        fsm.userData().push_back("ActInitRetryTimer");
    }
};

struct ActPostCallback {
public:
    template <typename Fsm>
    void operator()(Fsm& fsm, StConnecting& /*fromState*/, const EvtConnected&, StIdle& /*toState*/) {
        fsm.userData().push_back("ActPostCallback - StConnecting->StIdle");
    }

    template <typename Fsm>
    void operator()(Fsm& fsm, StWaitingTimerAfterCancel& /*fromState*/, const EvtTimerExpired&, StIdle& /*toState*/) {
        fsm.userData().push_back("ActPostCallback - StWaitingTimerAfterCancel->StIdle");
    }

    template <typename Fsm>
    void operator()(Fsm& fsm, StWaitingSocketAfterCancel& /*fromState*/, const EvtConnected&, StIdle& /*toState*/) {
        fsm.userData().push_back("ActPostCallback - StWaitingSocketAfterCancel->StIdle");
    }
};


struct ActCancelSocket {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
        fsm.userData().push_back("ActCancelSocket");
    }
};


struct ActCancelTimer {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
        fsm.userData().push_back("ActCancelTimer");
    }
};


struct NoAction {
    template <typename Fsm, typename SourceState, typename Event, typename TargetState>
    void operator()(Fsm& fsm, SourceState& /*source*/, const Event& /*evt*/, TargetState& /*target*/) {
        fsm.userData.push_back("NoAction");
    }
};

} // namespace socket_connector_detail


#include "fsm.hpp"
#include "SocketConnectorFsm.hpp"


using SocketConnectorFsm = fsm::Fsm<socket_connector_detail::FsmDef, std::vector<std::string>>;


TEST_CASE("Two consecutive connection requests that complete succesfully", "SocketConnectorFsmDef") {
    std::vector<std::string> userData;
    SocketConnectorFsm fsm(userData);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));
    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));

    REQUIRE(userData == (std::vector<std::string>{
        "ActInitConnection", 
        "ActPostCallback - StConnecting->StIdle",
        "ActInitConnection", 
        "ActPostCallback - StConnecting->StIdle"
    }));
}

#if 0
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
#endif

