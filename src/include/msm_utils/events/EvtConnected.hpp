#ifndef EVT_CONNECTED_HPP
#define EVT_CONNECTED_HPP

struct EvtConnected {
    inline EvtConnected(const boost::system::error_code& ec);
    boost::system::error_code ec;
};


EvtConnected::EvtConnected(const boost::system::error_code& ec) :
    ec(ec)
{}

#endif

