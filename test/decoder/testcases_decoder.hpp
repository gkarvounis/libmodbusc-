#ifndef TESTCASES_DECODER_HPP
#define TESTCASES_DECODER_HPP

TEST_CASE("decode header", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Decoder::Header header;

    decoder.decodeHeader({0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x01}, header);
    REQUIRE(header.transactionId.get() == 0x02);
    REQUIRE(header.payloadSize == 0x05);
    REQUIRE(header.unitId.get() == 0xab);
    REQUIRE(header.functionCode == mt::FunctionCode::READ_COILS);
    REQUIRE(header.isError == false);
}

TEST_CASE("decode header with invalid function code", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Decoder::Header header;

    REQUIRE_THROWS_AS(decoder.decodeHeader({0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x0A}, header), std::runtime_error);
}

TEST_CASE("decode header with exception code", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Decoder::Header header;

    decoder.decodeHeader({0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x81}, header);
    REQUIRE(header.functionCode == mt::FunctionCode::READ_COILS);
    REQUIRE(header.isError == true);
}

TEST_CASE("decode read coils req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::NumBits numBits;
    mt::Address startAddress;

    decoder.decodeReadCoilsReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02}, startAddress, numBits);

    REQUIRE(startAddress.get() == 0x1020);
    REQUIRE(numBits.get() == 0x0102);
}


TEST_CASE("decode read discrete inputs req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::NumBits numBits;
    mt::Address startAddress;

    decoder.decodeReadDiscreteInputsReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x02, 0x10, 0x20, 0x01, 0x02}, startAddress, numBits);

    REQUIRE(startAddress.get() == 0x1020);
    REQUIRE(numBits.get() == 0x0102);
}


TEST_CASE("decode read holding registers req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::NumRegs numRegs;
    mt::Address startAddress;

    decoder.decodeReadHoldingRegistersReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x03, 0x10, 0x20, 0x00, 0x02}, startAddress, numRegs);

    REQUIRE(startAddress.get() == 0x1020);
    REQUIRE(numRegs.get() == 0x0002);
}


TEST_CASE("decode read input registers req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::NumRegs numRegs;
    mt::Address startAddress;

    decoder.decodeReadInputRegistersReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x04, 0x10, 0x20, 0x00, 0x02}, startAddress, numRegs);

    REQUIRE(startAddress.get() == 0x1020);
    REQUIRE(numRegs.get() == 0x0002);
}


TEST_CASE("decode write single coil req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Address address;
    bool value;

    decoder.decodeWriteSingleCoilReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00}, address, value);

    REQUIRE(address.get() == 0x1020);
    REQUIRE(value == true);
}


TEST_CASE("decode write single register req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Address address;
    uint16_t value;

    decoder.decodeWriteSingleRegisterReq({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34}, address, value);

    REQUIRE(address.get() == 0x1020);
    REQUIRE(value == 0x1234);
}


TEST_CASE("decode read coils rsp, 4 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    std::vector<bool> result;

    decoder.decodeReadCoilsRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b00000101}, result);

    REQUIRE(result.size() == 8);
    REQUIRE(result == (std::vector<bool>{1, 0, 1, 0, 0, 0, 0, 0}));
}


TEST_CASE("decode read coils rsp, 8 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    std::vector<bool> result;

    decoder.decodeReadCoilsRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b11010001}, result);

    REQUIRE(result.size() == 8);
    REQUIRE(result == (std::vector<bool>{1, 0, 0, 0, 1, 0, 1, 1}));
}


TEST_CASE("decode read discrete inputs rsp, encode 10 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    std::vector<bool> result;

    decoder.decodeReadDiscreteInputsRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x02, 0x02, 0b01010101, 0b00000011}, result);

    REQUIRE(result == (std::vector<bool>{1,0,1,0,1,0,1,0,1,1, 0,0,0,0,0,0}));
}


TEST_CASE("decode read holding registers rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    std::vector<uint16_t> result;

    decoder.decodeReadHoldingRegistersRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04}, result);

    REQUIRE(result == (std::vector<uint16_t>{0x0102, 0x0304}));
}

TEST_CASE("decode read input registers rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    std::vector<uint16_t> result;

    decoder.decodeReadHoldingRegistersRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x04, 0x04, 0x01, 0x02, 0x03, 0x04}, result);

    REQUIRE(result == (std::vector<uint16_t>{0x0102, 0x0304}));
}


TEST_CASE("decode write single coil rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Address address;
    bool value = false;

    decoder.decodeWriteSingleCoilRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00}, address, value);

    REQUIRE(address.get() == 0x1020);
    REQUIRE(value == true);
}


TEST_CASE("decode write single register rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Address address;
    uint16_t value = false;

    decoder.decodeWriteSingleRegisterRsp({0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34}, address, value);

    REQUIRE(address.get() == 0x1020);
    REQUIRE(value == 0x1234);
}


TEST_CASE("decode error response", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::ExceptionCode code;

    decoder.decodeErrorResponse({0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0xab, 0x81, 0x01}, code);

    REQUIRE(code == mt::ExceptionCode::ILLEGAL_FUNCTION);
}

#endif
