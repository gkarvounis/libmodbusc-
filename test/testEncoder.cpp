#define CATCH_CONFIG_MAIN

#include "ModbusEncoder.hpp"
#include "Catch.hpp"

template <typename T>
std::vector<uint32_t> to_vector(const T& buf, std::size_t num_bytes) {
    return std::vector<uint32_t>(reinterpret_cast<const uint8_t*>(&buf), reinterpret_cast<const uint8_t*>(&buf) + num_bytes);
}

TEST_CASE("Encoding read coils request", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadCoilsReq::Buffer buf;
    encoder::ReadCoilsReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x0012);
    req.setNumValues(0x0020);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x01, 0x00, 0x12, 0x00, 0x20};

    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read coils request with too many coils", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadCoilsReq::Buffer buf;
    encoder::ReadCoilsReq req(buf);

    REQUIRE_THROWS_AS(req.setNumValues(modbus::tcp::MODBUS_MAX_NUM_BITS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read discrete inputs req", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadDiscreteInputsReq::Buffer buf;
    encoder::ReadDiscreteInputsReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x0012);
    req.setNumValues(0x0020);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x02, 0x00, 0x12, 0x00, 0x20};

    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read discrete inputs with too many inputs", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadDiscreteInputsReq::Buffer buf;
    encoder::ReadDiscreteInputsReq req(buf);

    REQUIRE_THROWS_AS(req.setNumValues(modbus::tcp::MODBUS_MAX_NUM_BITS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read holding registers req", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadHoldingRegistersReq::Buffer buf;
    encoder::ReadHoldingRegistersReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x0012);
    req.setNumValues(0x0020);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x12, 0x00, 0x20};

    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read holding registers req with too many holding registers", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadHoldingRegistersReq::Buffer buf;
    encoder::ReadHoldingRegistersReq req(buf);

    REQUIRE_THROWS_AS(req.setNumValues(modbus::tcp::MODBUS_MAX_NUM_REGS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read input registers req", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadInputRegistersReq::Buffer buf;
    encoder::ReadInputRegistersReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x0012);
    req.setNumValues(0x0020);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x12, 0x00, 0x20};

    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read input registers req with too many input registers", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadInputRegistersReq::Buffer buf;
    encoder::ReadInputRegistersReq req(buf);

    REQUIRE_THROWS_AS(req.setNumValues(modbus::tcp::MODBUS_MAX_NUM_REGS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding write coil request", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilReq::Buffer buf;
    encoder::WriteCoilReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setAddress(0x0012);
    req.setValue(true);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x00, 0x12, 0xFF, 0x00};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write register request", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegisterReq::Buffer buf;
    encoder::WriteRegisterReq req(buf);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setAddress(0x0012);
    req.setValue(0x1234);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x12, 0x12, 0x34};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write registers request", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegistersReq::Buffer buf;
    encoder::WriteRegistersReq req(buf, 4);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x3456);
    req.setValue(0, 1);
    req.setValue(1, 2);
    req.setValue(2, 3);
    req.setValue(3, 4);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x0f, 0x01, 0x10, 0x34, 0x56, 0x00, 0x04, 0x08, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write registers request - attempt to set holding register with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegistersReq::Buffer buf;
    encoder::WriteRegistersReq req(buf, 8);

    REQUIRE_THROWS_AS(req.setValue(8, 1234), std::out_of_range);
}


TEST_CASE("Encoding write registers request that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    WriteRegistersReq::Buffer buf;
    REQUIRE_THROWS_AS(WriteRegistersReq(buf, modbus::tcp::MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding write coils request (10 coils to be set)", "[]") {
    REQUIRE(NUM_BITS_TO_BYTES(7)==1);
    REQUIRE(NUM_BITS_TO_BYTES(8)==1);
    REQUIRE(NUM_BITS_TO_BYTES(9)==2);

    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilsReq::Buffer buf;
    encoder::WriteCoilsReq req(buf, 10);

    req.setUnitId(0x01);
    req.setTransactionId(0x1122);
    req.setStartAddress(0x3456);
    req.setValue(0, 1);
    req.setValue(1, 1);
    req.setValue(2, 1);
    req.setValue(3, 0);
    req.setValue(4, 1);
    req.setValue(5, 1);
    req.setValue(6, 0);
    req.setValue(7, 1);
    req.setValue(8, 1);
    req.setValue(9, 1);

    std::vector<uint32_t> encoded(to_vector(buf, req.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x09, 0x01, 0x0F, 0x34, 0x56, 0x00, 0x0A, 0x02, 0xB7, 0x03};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write coils request - attempt to set coil with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilsReq::Buffer buf;
    encoder::WriteCoilsReq req(buf, 5);

    REQUIRE_THROWS_AS(req.setValue(5, true), std::out_of_range);
}


TEST_CASE("Encoding write coils request that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    WriteCoilsReq::Buffer buf;
    REQUIRE_THROWS_AS(WriteCoilsReq(buf, modbus::tcp::MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding error response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ErrorResponse::Buffer buf;
    encoder::ErrorResponse resp(buf, modbus::tcp::READ_COILS);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);
    resp.setExceptionCode(modbus::tcp::ILLEGAL_FUNCTION);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x03, 0x01, 0x81, 0x01};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read coils response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadCoilsRsp::Buffer buf;
    encoder::ReadCoilsRsp resp(buf, 10);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setValue(0, 1);
    resp.setValue(1, 1);
    resp.setValue(2, 1);
    resp.setValue(3, 0);
    resp.setValue(4, 1);
    resp.setValue(5, 1);
    resp.setValue(6, 0);
    resp.setValue(7, 1);
    resp.setValue(8, 1);
    resp.setValue(9, 1);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x05, 0x01, 0x01, 0x02, 0xB7, 0x03};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read coils response - attempt to set coil with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadCoilsRsp::Buffer buf;
    encoder::ReadCoilsRsp resp(buf, 5);
    REQUIRE_THROWS_AS(resp.setValue(5, true), std::out_of_range);
}


TEST_CASE("Encoding read coils response that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    ReadCoilsRsp::Buffer buf;
    REQUIRE_THROWS_AS(ReadCoilsRsp(buf, modbus::tcp::MODBUS_MAX_NUM_BITS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read discrete inputs response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadDiscreteInputsRsp::Buffer buf;
    encoder::ReadDiscreteInputsRsp resp(buf, 10);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setValue(0, 1);
    resp.setValue(1, 1);
    resp.setValue(2, 1);
    resp.setValue(3, 0);
    resp.setValue(4, 1);
    resp.setValue(5, 1);
    resp.setValue(6, 0);
    resp.setValue(7, 1);
    resp.setValue(8, 1);
    resp.setValue(9, 1);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{0x11, 0x22, 0x00, 0x00, 0x00, 0x05, 0x01, 0x02, 0x02, 0xB7, 0x03};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read discrete inputs response - attempt to set coil with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadDiscreteInputsRsp::Buffer buf;
    encoder::ReadDiscreteInputsRsp resp(buf, 5);

    REQUIRE_THROWS_AS(resp.setValue(5, true), std::out_of_range);
}


TEST_CASE("Encoding read discrete inputs response that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    ReadDiscreteInputsRsp::Buffer buf;
    REQUIRE_THROWS_AS(ReadDiscreteInputsRsp(buf, modbus::tcp::MODBUS_MAX_NUM_BITS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read holding registers response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadHoldingRegsRsp::Buffer buf;
    encoder::ReadHoldingRegsRsp resp(buf, 3);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setValue(0, 1);
    resp.setValue(1, 2);
    resp.setValue(2, 3);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 17, 34, 0, 0, 0, 9, 1, 3, 6, 0, 1, 0, 2, 0, 3};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read holding registers response - attempt to set register with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadHoldingRegsRsp::Buffer buf;
    encoder::ReadHoldingRegsRsp resp(buf, 2);

    REQUIRE_THROWS_AS(resp.setValue(2, true), std::out_of_range);
}


TEST_CASE("Encoding read holding registers response that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    ReadHoldingRegsRsp::Buffer buf;
    REQUIRE_THROWS_AS(ReadHoldingRegsRsp(buf, modbus::tcp::MODBUS_MAX_NUM_REGS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding read input registers response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadInputRegsRsp::Buffer buf;
    encoder::ReadInputRegsRsp resp(buf, 3);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setValue(0, 1);
    resp.setValue(1, 2);
    resp.setValue(2, 3);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 17, 34, 0, 0, 0, 9, 1, 4, 6, 0, 1, 0, 2, 0, 3};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding read input registers response - attempt to set register with index out of bounds", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::ReadInputRegsRsp::Buffer buf;
    encoder::ReadInputRegsRsp resp(buf, 2);

    REQUIRE_THROWS_AS(resp.setValue(2, true), std::out_of_range);
}


TEST_CASE("Encoding read input registers response that exceeds message capacity", "[]") {
    using namespace modbus::tcp::encoder;

    ReadInputRegsRsp::Buffer buf;
    REQUIRE_THROWS_AS(ReadInputRegsRsp(buf, modbus::tcp::MODBUS_MAX_NUM_REGS_IN_READ_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding write coil response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilResponse::Buffer buf;
    encoder::WriteCoilResponse resp(buf);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setAddress(0x3344);
    resp.setValue(true);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 0x11, 0x22, 0, 0, 0, 6, 1, 5, 0x33, 0x44, 0xFF, 0x00};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write register response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegisterResponse::Buffer buf;
    encoder::WriteRegisterResponse resp(buf);
    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);

    resp.setAddress(0x3344);
    resp.setValue(0x5566);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 0x11, 0x22, 0, 0, 0, 6, 1, 6, 0x33, 0x44, 0x55, 0x66};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write coils response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilsRsp::Buffer buf;
    encoder::WriteCoilsRsp resp(buf);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);
    resp.setStartAddress(0x3344);
    resp.setNumValues(0x0055);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 0x11, 0x22, 0, 0, 0, 6, 1, 0x0F, 0x33, 0x44, 0x00, 0x55};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write coils response - attempt to set numer of coils biger than modbus limit") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteCoilsRsp::Buffer buf;
    encoder::WriteCoilsRsp resp(buf);

    REQUIRE_THROWS_AS(resp.setNumValues(modbus::tcp::MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST+1), std::out_of_range);
}


TEST_CASE("Encoding write registers response", "[]") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegsRsp::Buffer buf;
    encoder::WriteRegsRsp resp(buf);

    resp.setUnitId(0x01);
    resp.setTransactionId(0x1122);
    resp.setStartAddress(0x3344);
    resp.setNumValues(0x0055);

    std::vector<uint32_t> encoded(to_vector(buf, resp.message_size()));
    std::vector<uint32_t> expected{ 0x11, 0x22, 0, 0, 0, 6, 1, 0x10, 0x33, 0x44, 0x00, 0x55};
    REQUIRE(encoded == expected);
}


TEST_CASE("Encoding write registers response - attempt to set numer of registers biger than modbus limit") {
    namespace encoder = modbus::tcp::encoder;

    encoder::WriteRegsRsp::Buffer buf;
    encoder::WriteRegsRsp resp(buf);

    REQUIRE_THROWS_AS(resp.setNumValues(modbus::tcp::MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST+1), std::out_of_range);
}

/*
#include <iomanip>
#include <iostream>
#include <string.h>

template <typename T>
void dump(const T& t) {
    for (auto x: t) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (std::size_t)(x) << " ";
    }
    std::cout << std::endl;
}
*/
