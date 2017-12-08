#define CATCH_CONFIG_MAIN

#include "Catch.hpp"

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include "ModbusServerDevice.hpp"

class TestServerDevice : public modbus::tcp::ServerDevice {
public:
                            TestServerDevice(const modbus::tcp::UnitId& id) : modbus::tcp::ServerDevice(id) {}
protected:
    virtual bool            getCoil(const modbus::tcp::Address& address) const {
        static const std::vector<bool> coils{1,0,1,0,0,0,0,0};
        return coils.at(address.get()-0x1020);
    }

    virtual bool            getDiscreteInput(const modbus::tcp::Address& address) const {}
    virtual uint16_t        getHoldingRegister(const modbus::tcp::Address& addr) const {}
    virtual uint16_t        getInputRegister(const modbus::tcp::Address& addr) const {};

    virtual void            setCoil(const modbus::tcp::Address& address, bool value) {}
    virtual void            setRegister(const modbus::tcp::Address& address, uint16_t value) {}
};


TEST_CASE("handle read coils", "[server device]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> rsp;
    TestServerDevice dev(mt::UnitId(0xab));

    dev.handleMessage({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x00, 0x04}, rsp);

    REQUIRE(rsp == (std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b00000101}));
}
