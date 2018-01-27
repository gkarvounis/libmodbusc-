#ifndef EVT_SENT_HPP
#define EVT_SENT_HPP

struct EvtSent {
    inline EvtSent(const boost::system::error_code& ec);
    boost::system::error_code ec;
};


EvtSent::EvtSent(const boost::system::error_code& ec) :
    ec(ec)
{}

#endif

