#define CATCH_CONFIG_MAIN

#include "Catch.hpp"

#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include "ModbusServerDevice.hpp"

class TestServerDevice : public modbus::tcp::ServerDevice {
public:
                            TestServerDevice(const modbus::tcp::UnitId& id, std::function<bool(uint16_t address)> getCoilCb);
protected:
    virtual bool            getCoil(const modbus::tcp::Address& address) const override;

    std::function<bool(uint16_t address)> m_getCoilCb;
};


TestServerDevice::TestServerDevice(const modbus::tcp::UnitId& id, std::function<bool(uint16_t address)> getCoilCb) :
    modbus::tcp::ServerDevice(id),
    m_getCoilCb(getCoilCb)
{
}


bool TestServerDevice::getCoil(const modbus::tcp::Address& address) const {
    return m_getCoilCb(address.get());
}


TEST_CASE("handle read coils", "[server device]") {
    namespace mt = modbus::tcp;

    TestServerDevice dev(mt::UnitId(0xab), [](uint16_t address)->bool {
        static const std::vector<bool> coils{1,0,1,0,0,0,0,0};
        return coils.at(address-0x1020);
    });

    std::vector<uint8_t> req;
    std::vector<uint8_t> rsp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(0x02));
    encoder.encodeReadCoilsReq(mt::Address(0x1020), mt::NumBits(4), req);
    dev.handleMessage(req, rsp);

    REQUIRE(rsp == (std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b00000101}));
}


TEST_CASE("handle read coils - bad unit id", "[server device]") {
    namespace mt = modbus::tcp;

    TestServerDevice dev(mt::UnitId(0xab), [](uint16_t address)->bool {
        static const std::vector<bool> coils{1,0,1,0,0,0,0,0};
        return coils.at(address-0x1020);
    });

    std::vector<uint8_t> req;
    std::vector<uint8_t> rsp;

    mt::Encoder encoder(mt::UnitId(0xac), mt::TransactionId(0x02));
    encoder.encodeReadCoilsReq(mt::Address(0x1020), mt::NumBits(4), req);
    REQUIRE_THROWS_AS(dev.handleMessage(req, rsp), mt::UnitIdMismatch);
}


TEST_CASE("handle read coils - operation not supported") {
    namespace mt = modbus::tcp;

    TestServerDevice dev(mt::UnitId(0xab), [](uint16_t address)->bool {
        throw mt::FunctionCodeNotSupported(mt::FunctionCode::READ_COILS);
    });

    std::vector<uint8_t> req;
    std::vector<uint8_t> rsp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(0x02));
    encoder.encodeReadCoilsReq(mt::Address(0x1020), mt::NumBits(4), req);
    dev.handleMessage(req, rsp);

    mt::decoder_views::Header rsp_header_view(rsp);
    REQUIRE(rsp_header_view.isError() == true);
    REQUIRE(rsp_header_view.getTransactionId() == mt::TransactionId(0x02));
    REQUIRE(rsp_header_view.getUnitId() == mt::UnitId(0xab));

    mt::decoder_views::ErrorResponse rsp_payload_view(rsp);
    REQUIRE(rsp_payload_view.getCode() == mt::ExceptionCode::ILLEGAL_FUNCTION);
}
