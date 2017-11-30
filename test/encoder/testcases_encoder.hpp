#ifndef TESTCASES_ENCODER_HPP
#define TESTCASES_ENCODER_HPP

TEST_CASE("encode read coils req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02};
    std::vector<uint8_t> encoded;
    encoder.encodeReadCoilsReq(mt::Address(0x1020), mt::NumBits(0x0102), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read discrete inputs req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x02, 0x10, 0x20, 0x01, 0x02};
    std::vector<uint8_t> encoded;
    encoder.encodeReadDiscreteInputsReq(mt::Address(0x1020), mt::NumBits(0x0102), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read holding registers req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x03, 0x10, 0x20, 0x00, 0x02};
    std::vector<uint8_t> encoded;
    encoder.encodeReadHoldingRegistersReq(mt::Address(0x1020), mt::NumRegs(0x0002), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read input registers req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x04, 0x10, 0x20, 0x00, 0x02};
    std::vector<uint8_t> encoded;
    encoder.encodeReadInputRegistersReq(mt::Address(0x1020), mt::NumRegs(0x0002), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode write single coil req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    std::vector<uint8_t> encoded;
    encoder.encodeWriteSingleCoilReq(mt::Address(0x1020), true, encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode write single regitster req", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    std::vector<uint8_t> encoded;
    encoder.encodeWriteSingleRegisterReq(mt::Address(0x1020), 0x1234, encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode write single coil rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    std::vector<uint8_t> encoded;
    encoder.encodeWriteSingleCoilRsp(mt::Address(0x1020), true, encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode write single regitster rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    std::vector<uint8_t> encoded;
    encoder.encodeWriteSingleRegisterRsp(mt::Address(0x1020), 0x1234, encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read coils rsp, encode 4 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b00000101};
    std::vector<uint8_t> encoded;

    encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read coils rsp, encode 8 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b01010101};
    std::vector<uint8_t> encoded;

    encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read coils rsp, encode 10 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0, 1, 1};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x01, 0x02, 0b01010101, 0b00000011};
    std::vector<uint8_t> encoded;

    encoder.encodeReadCoilsRsp(coils.begin(), coils.end(), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read discrete inputs rsp, encode 10 coils", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<bool> coils{1, 0, 1, 0, 1, 0, 1, 0, 1, 1};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x02, 0x02, 0b01010101, 0b00000011};
    std::vector<uint8_t> encoded;

    encoder.encodeReadDiscreteInputsRsp(coils.begin(), coils.end(), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read holding registers rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<uint16_t> regs{0x0102, 0x0304};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> encoded;

    encoder.encodeReadHoldingRegistersRsp(regs.begin(), regs.end(), encoded);

    REQUIRE(target == encoded);
}


TEST_CASE("encode read input registers rsp", "[encoder]") {
    namespace mt = modbus::tcp;

    mt::Encoder encoder(mt::UnitId(0xab), mt::TransactionId(2));

    std::initializer_list<uint16_t> regs{0x0102, 0x0304};

    std::vector<uint8_t> target{0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x04, 0x04, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> encoded;

    encoder.encodeReadInputRegistersRsp(regs.begin(), regs.end(), encoded);

    REQUIRE(target == encoded);
}

#endif
