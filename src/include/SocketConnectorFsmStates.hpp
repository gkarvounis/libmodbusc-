#ifndef SOCKET_CONNECTOR_FSM_STATES_HPP
#define SOCKET_CONNECTOR_FSM_STATES_HPP

namespace socket_connector_detail {

struct StIdle {};
struct StConnecting {};
struct StWaitingRetryTimer {};
struct StWaitingSocketAfterCancel {};
struct StWaitingTimerAfterCancel {};


} // namespace socket_connector_detail

#endif

