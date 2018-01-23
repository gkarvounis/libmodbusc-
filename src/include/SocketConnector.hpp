#ifndef SOCKET_CONNECTOR_HPP
#define SOCKET_CONNECTOR_HPP

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
#include "SocketConnectorFsmActions.hpp"
#include "SocketConnectorFsm.hpp"

class SocketConnector {
public:
    using Socket                                 = boost::asio::ip::tcp::socket;
    using Endpoint                               = boost::asio::ip::tcp::endpoint;
    using Interval                               = boost::posix_time::milliseconds;
    using SocketConnectedCb                      = std::function<void(const boost::system::error_code& ec)>;

    inline                                       SocketConnector(boost::asio::io_service& io);
    inline                                       SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval);
    inline                                      ~SocketConnector();

    inline void                                  async_connect(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb);
    inline void                                  async_connect(Socket& sock, SocketConnectedCb cb);

    inline void                                  cancel();

    inline boost::asio::io_service&              get_io_service();

private:
    using SocketConnectorFsm = boost::msm::back::state_machine<socket_connector_detail::FsmDef>;

    SocketConnectorFsm                           m_fsm;
};


SocketConnector::SocketConnector(boost::asio::io_service& io) : 
    m_fsm(&io)
{
    m_fsm.start();
}


SocketConnector::SocketConnector(boost::asio::io_service& io, const Endpoint& ep, const Interval& interval) :
    m_fsm(&io, ep, interval)
{}


SocketConnector::~SocketConnector() {
    std::cout << "~SocketConnector" << std::endl;
}


boost::asio::io_service& SocketConnector::get_io_service() {
    return m_fsm.get_io_service();
}


void SocketConnector::async_connect(Socket& sock, const Endpoint& ep, const Interval& interval, SocketConnectedCb cb) {
    m_fsm.set_connect_params(sock, ep, interval, cb);
    m_fsm.process_event(EvtInitConnection());
}


void SocketConnector::async_connect(Socket& sock, SocketConnectedCb cb) {
    m_fsm.set_connect_params(sock, cb);
    m_fsm.process_event(EvtInitConnection());
}


void SocketConnector::cancel() {
    m_fsm.process_event(EvtCancel());
}

#endif

