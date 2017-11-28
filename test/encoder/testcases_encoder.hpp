#ifndef TESTCASES_ENCODER_HPP
#define TESTCASES_ENCODER_HPP

TEST_CASE("encode read coils req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::ReadCoilsReq(mt::Address(0x1020), mt::NumBits(0x0102)), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read discrete inputs req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x02, 0x10, 0x20, 0x01, 0x02};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::ReadDiscreteInputsReq(mt::Address(0x1020), mt::NumBits(0x0102)), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read holding registers req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x03, 0x10, 0x20, 0x00, 0x02};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::ReadHoldingRegistersReq(mt::Address(0x1020), mt::NumRegs(0x0002)), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read input registers req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x04, 0x10, 0x20, 0x00, 0x02};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::ReadInputRegistersReq(mt::Address(0x1020), mt::NumRegs(0x0002)), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode write single coil req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::WriteSingleCoilReq(mt::Address(0x1020), true), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode write single regitster req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::WriteSingleRegisterReq(mt::Address(0x1020), 0x1234), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode write single coil rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::WriteSingleCoilRsp(mt::Address(0x1020), true), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode write single regitster rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    uint8_t encoded[16] = {0};

    std::size_t ret = encoder.encode(mt::WriteSingleRegisterRsp(mt::Address(0x1020), 0x1234), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read coils rsp, encode 4 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0a, 0xab, 0x01, 0x01, 0b00000101};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read coils rsp, encode 8 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0a, 0xab, 0x01, 0x01, 0b01010101};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read coils rsp, encode 10 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0, 1, 1};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0b, 0xab, 0x01, 0x02, 0b01010101, 0b00000011};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read discrete inputs rsp, encode 10 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0, 1, 1};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0b, 0xab, 0x02, 0x02, 0b01010101, 0b00000011};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadDiscreteInputsRsp(coils.begin(), coils.end(), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read holding registers rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<uint16_t> regs{0x0102, 0x0304};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0d, 0xab, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadHoldingRegistersRsp(regs.begin(), regs.end(), encoded, sizeof(encoded));

    //for (unsigned i = 0; i < ret; ++i) std::cout << std::hex << (unsigned)encoded[i] << ' ';
    //std::cout << std::endl;

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}


TEST_CASE("encode read input registers rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<uint16_t> regs{0x0102, 0x0304};

    uint8_t target[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x0d, 0xab, 0x04, 0x04, 0x01, 0x02, 0x03, 0x04};
    uint8_t encoded[32] = {0};

    std::size_t ret = encoder.encodeReadInputRegistersRsp(regs.begin(), regs.end(), encoded, sizeof(encoded));

    REQUIRE(ret == sizeof(target));
    REQUIRE(std::equal(target, target+sizeof(target), encoded));
}

#endif
