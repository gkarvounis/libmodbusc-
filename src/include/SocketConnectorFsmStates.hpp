#ifndef SOCKET_CONNECTOR_FSM_STATES_HPP
#define SOCKET_CONNECTOR_FSM_STATES_HPP

namespace socket_connector_detail {

struct StIdle :                             public boost::msm::front::state<> {};
struct StConnecting:                        public boost::msm::front::state<> {};
struct StWaitingRetryTimer:                 public boost::msm::front::state<> {};
struct StWaitingSocketAfterCancel:          public boost::msm::front::state<> {};
struct StWaitingTimerAfterCancel:           public boost::msm::front::state<> {};


} // namespace socket_connector_detail

#endif

