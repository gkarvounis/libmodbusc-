#ifndef TESTCASES_DECODER_HPP
#define TESTCASES_DECODER_HPP

TEST_CASE("decode header", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> header{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x01};
    mt::decoder_views::Header view(header);

    REQUIRE(view.getTransactionId() == mt::TransactionId(0x02));
    REQUIRE(view.getLength() == 0x05);
    REQUIRE(view.getUnitId() == mt::UnitId(0xab));
    REQUIRE(view.getFunctionCode() == mt::FunctionCode::READ_COILS);
    REQUIRE(view.isError() == false);
}

TEST_CASE("decode header with invalid function code", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> header{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x0A};
    mt::decoder_views::Header view(header);

    REQUIRE_THROWS_AS(view.getFunctionCode(), std::runtime_error);
}

TEST_CASE("decode header with exception code", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> header{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x81};
    mt::decoder_views::Header view(header);

    REQUIRE(view.getFunctionCode() == mt::FunctionCode::READ_COILS);
    REQUIRE(view.isError() == true);
}

TEST_CASE("decode read coils req", "[decoder]") {
    namespace mt = modbus::tcp;
    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02};
    mt::decoder_views::ReadCoilsReq view(msg);

    REQUIRE(view.getStartAddress() == mt::Address(0x1020));
    REQUIRE(view.getNumBits() == mt::NumBits(0x0102));
}


TEST_CASE("decode read discrete inputs req", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x02, 0x10, 0x20, 0x01, 0x02};
    mt::decoder_views::ReadDiscreteInputsReq view(msg);

    REQUIRE(view.getStartAddress() == mt::Address(0x1020));
    REQUIRE(view.getNumBits() == mt::NumBits(0x0102));
}


TEST_CASE("decode read holding registers req", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x03, 0x10, 0x20, 0x00, 0x02};
    mt::decoder_views::ReadHoldingRegistersReq view(msg);

    REQUIRE(view.getStartAddress() == mt::Address(0x1020));
    REQUIRE(view.getNumRegs() == mt::NumRegs(0x0002));
}


TEST_CASE("decode read input registers req", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x04, 0x10, 0x20, 0x00, 0x02};
    mt::decoder_views::ReadInputRegistersReq view(msg);

    REQUIRE(view.getStartAddress() == mt::Address(0x1020));
    REQUIRE(view.getNumRegs() == mt::NumRegs(0x0002));
}


TEST_CASE("decode write single coil req", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    mt::decoder_views::WriteSingleCoilReq view(msg);

    REQUIRE(view.getAddress() == mt::Address(0x1020));
    REQUIRE(view.getValue() == true);
}


TEST_CASE("decode write single register req", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    mt::decoder_views::WriteSingleRegisterReq view(msg);

    REQUIRE(view.getAddress() == mt::Address(0x1020));
    REQUIRE(view.getValue() == 0x1234);
}


TEST_CASE("decode read coils rsp, 4 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b00000101};
    mt::decoder_views::ReadCoilsRsp view(msg);

    REQUIRE(view.getNumBits() == 8);

    std::vector<bool> coils{
        view.getBit(0), view.getBit(1), view.getBit(2), view.getBit(3),
        view.getBit(4), view.getBit(5), view.getBit(6), view.getBit(7)};

    REQUIRE(coils == (std::vector<bool>{1, 0, 1, 0, 0, 0, 0, 0}));
}


TEST_CASE("decode read coils rsp, 8 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0xab, 0x01, 0x01, 0b11010001};
    mt::decoder_views::ReadCoilsRsp view(msg);

    REQUIRE(view.getNumBits() == 8);

    std::vector<bool> coils{
        view.getBit(0), view.getBit(1), view.getBit(2), view.getBit(3),
        view.getBit(4), view.getBit(5), view.getBit(6), view.getBit(7)};

    REQUIRE(coils == (std::vector<bool>{1, 0, 0, 0, 1, 0, 1, 1}));
}


TEST_CASE("decode read discrete inputs rsp, encode 10 coils", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x02, 0x02, 0b01010101, 0b00000011};
    mt::decoder_views::ReadDiscreteInputsRsp view(msg);

    REQUIRE(view.getNumBits() == 16);

    std::vector<bool> coils{
        view.getBit(0), view.getBit(1), view.getBit(2), view.getBit(3),
        view.getBit(4), view.getBit(5), view.getBit(6), view.getBit(7),
        view.getBit(8), view.getBit(9), view.getBit(10), view.getBit(11),
        view.getBit(12), view.getBit(13), view.getBit(14), view.getBit(15)
    };

    REQUIRE(coils == (std::vector<bool>{1,0,1,0,1,0,1,0,1,1, 0,0,0,0,0,0}));
}


TEST_CASE("decode read holding registers rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    mt::decoder_views::ReadHoldingRegistersRsp view(msg);

    REQUIRE(view.getNumRegs() == 2);
    REQUIRE(view.getRegister(0) == 0x0102);
    REQUIRE(view.getRegister(1) == 0x0304);
}


TEST_CASE("decode read input registers rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xab, 0x04, 0x04, 0x01, 0x02, 0x03, 0x04};
    mt::decoder_views::ReadInputRegistersRsp view(msg);

    REQUIRE(view.getNumRegs() == 2);
    REQUIRE(view.getRegister(0) == 0x0102);
    REQUIRE(view.getRegister(1) == 0x0304);
}


TEST_CASE("decode write single coil rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x05, 0x10, 0x20, 0xFF, 0x00};
    mt::decoder_views::WriteSingleCoilRsp view(msg);

    REQUIRE(view.getAddress() == mt::Address(0x1020));
    REQUIRE(view.getValue() == true);
}


TEST_CASE("decode write single register rsp", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x06, 0x10, 0x20, 0x12, 0x34};
    mt::decoder_views::WriteSingleRegisterRsp view(msg);

    REQUIRE(view.getAddress() == mt::Address(0x1020));
    REQUIRE(view.getValue() == 0x1234);
}


TEST_CASE("decode error response", "[decoder]") {
    namespace mt = modbus::tcp;

    std::vector<uint8_t> msg{0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0xab, 0x81, 0x01};
    mt::decoder_views::ErrorResponse view(msg);

    REQUIRE(view.getCode() == mt::ExceptionCode::ILLEGAL_FUNCTION);
}

#endif
