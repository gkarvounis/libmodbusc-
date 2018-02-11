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

#include "fsm.hpp"
#include "SocketConnectorFsmStates.hpp"
#include "SocketConnectorFsmActions.hpp"
#include "SocketConnectorFsm.hpp"

struct UserData {
    UserData(std::vector<std::string>& log) :
        log(log)
    {}

    template <typename Fsm>
    void initConnection(Fsm&) {
        log.push_back("ActInitConnection");
    }

    template <typename Fsm>
    void initTimer(Fsm&) {
        log.push_back("ActInitTimer");
    }

    void postCallback(const boost::system::error_code& ec) {
        log.push_back("ActPostCallback - " + ec.message());
    }

    void cancelSocket() {
        log.push_back("ActCancelSocket");
    }

    void cancelTimer() {
        log.push_back("ActCancelTimer");
    }

    std::vector<std::string>& log;
};


using SocketConnectorFsm = fsm::Fsm<socket_connector_detail::FsmDef, UserData>;


TEST_CASE("Two consecutive connection requests that complete succesfully", "SocketConnectorFsmDef") {
    std::vector<std::string> log;
    UserData userData(log);
    SocketConnectorFsm fsm(userData);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));
    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::system::error_code()));

    REQUIRE(log == (std::vector<std::string>{
        "ActInitConnection", 
        "ActPostCallback - Success",
        "ActInitConnection", 
        "ActPostCallback - Success"
    }));
}


TEST_CASE("One connection request that is canceled by user before completing", "SocketConnectorFsmDef") {
    std::vector<std::string> log;
    UserData userData(log);

    SocketConnectorFsm fsm(userData);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtConnected(boost::asio::error::operation_aborted));

    REQUIRE(log == (std::vector<std::string>{
        "ActInitConnection",
        "ActCancelSocket",
        "ActPostCallback - Operation canceled"
    }));
}


TEST_CASE("One connection request that is canceled right after completing", "SocketConnectorFsmDef") {
    std::vector<std::string> log;
    UserData userData(log);

    SocketConnectorFsm fsm(userData);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtConnected(boost::system::error_code()));

    REQUIRE(log == (std::vector<std::string>{
        "ActInitConnection",
        "ActCancelSocket",
        "ActPostCallback - Success"
    }));
}


TEST_CASE("One connection request that fails and user cancels before timer expiration", "SocketConnectorFsmDef") {
    std::vector<std::string> log;
    UserData userData(log);

    SocketConnectorFsm fsm(userData);
    fsm.start();

    fsm.process_event(EvtInitConnection());
    fsm.process_event(EvtConnected(boost::asio::error::connection_refused));
    fsm.process_event(EvtCancel());
    fsm.process_event(EvtTimerExpired(boost::asio::error::operation_aborted));

    REQUIRE(log == (std::vector<std::string>{
        "ActInitConnection",
        "ActInitTimer",
        "ActCancelTimer",
        "ActPostCallback - Operation canceled"
    }));
}

