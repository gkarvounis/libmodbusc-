#ifndef EVT_TIMER_HPP
#define EVT_TIMER_HPP

struct EvtTimerExpired {
    inline EvtTimerExpired(const boost::system::error_code& ec);
    boost::system::error_code ec;
};


EvtTimerExpired::EvtTimerExpired(const boost::system::error_code& ec) :
    ec(ec)
{}

#endif
