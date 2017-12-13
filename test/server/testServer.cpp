#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COUNTER
#include "Catch.hpp"


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

TEST_CASE("1", "[]") {
    ServerDevice dev(modbus::tcp::UnitId(0xab));

    boost::asio::io_service io;
    modbus::tcp::Server server(io, dev);

    const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 50000);
    server.start(ep, [](){
    });
    io.run_one();

    boost::asio::ip::tcp::socket client(io);
    client.connect(ep);
    io.run_one();

    std::vector<uint8_t> req;
    modbus::tcp::Encoder encoder(modbus::tcp::UnitId(0xab), modbus::tcp::TransactionId(0x02));
    encoder.encodeReadCoilsReq(modbus::tcp::Address(0x1020), modbus::tcp::NumBits(4), req);

    boost::asio::write(client, boost::asio::buffer(req));

    io.run_one();
    io.run_one();

    std::vector<uint8_t> rsp;
    rsp.resize(sizeof(modbus::tcp::ExceptionRsp));
    boost::asio::read(client, boost::asio::buffer(rsp));

    server.stop();
    io.run();

    REQUIRE(rsp == (std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0xab, 0x81, 0x01}));
}
