#ifndef TESTCASES_DECODER_HPP
#define TESTCASES_DECODER_HPP

TEST_CASE("decode header", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Decoder::Header header;

    decoder.decodeHeader(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x01}, header);
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

    REQUIRE_THROWS_AS(decoder.decodeHeader(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x0A}, header), std::runtime_error);
}

TEST_CASE("decode header with exception code", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::Decoder::Header header;

    decoder.decodeHeader(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0xab, 0x81}, header);
    REQUIRE(header.functionCode == mt::FunctionCode::READ_COILS);
    REQUIRE(header.isError == true);
}

TEST_CASE("decode read coils req", "[decoder]") {
    namespace mt = modbus::tcp;

    mt::Decoder decoder;
    mt::NumBits numBits;
    mt::Address startAddress;

    decoder.decodeReadCoilsReq(std::vector<uint8_t>{0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0xab, 0x01, 0x10, 0x20, 0x01, 0x02}, startAddress, numBits);

    REQUIRE(startAddress.get() == 0x1020);
    REQUIRE(numBits.get() == 0x0102);
}


#endif
