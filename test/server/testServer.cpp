#include <vector>
#include <functional>
#include <boost/asio.hpp>
#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusDecoder.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusServerDevice.hpp"
#include "ModbusServer.hpp"

class ServerDevice : public modbus::tcp::ServerDevice {
public:
    ServerDevice(const modbus::tcp::UnitId& unitId) : modbus::tcp::ServerDevice(unitId) {}
};

int main() {
    ServerDevice dev(modbus::tcp::UnitId(0xab));

    boost::asio::io_service io;
    modbus::tcp::Server server(io, dev);
    server.start(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 50000), [](){
    });

    io.run();
}
