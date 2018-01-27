#ifndef EVT_RECEIVED_HPP
#define EVT_RECEIVED_HPP

struct EvtReceived {
    inline EvtReceived(const boost::system::error_code& ec);
    boost::system::error_code ec;
};


EvtReceived::EvtReceived(const boost::system::error_code& ec) :
    ec(ec)
{}

#endif

