#ifndef TESTCASES_REQUESTS_HPP
#define TESTCASES_REQUESTS_HPP

TEST_CASE("modbus::tcp::ReadCoilsReq", "[requests]") {
    modbus::tcp::ReadCoilsReq req(modbus::tcp::Address(123), modbus::tcp::NumBits(10));
    REQUIRE(req.getNumValues().get() == 10);
    REQUIRE(req.getStartAddress().get() == 123);
}


TEST_CASE("modbus::tcp::ReadDiscreteInputs", "[requests]") {
    modbus::tcp::ReadDiscreteInputsReq req(modbus::tcp::Address(123), modbus::tcp::NumBits(10));
    REQUIRE(req.getStartAddress().get() == 123);
    REQUIRE(req.getNumValues().get() == 10);
}


TEST_CASE("modbus::tcp::ReadHoldingRegistersReq", "[requests]") {
    modbus::tcp::ReadHoldingRegistersReq req(modbus::tcp::Address(123), modbus::tcp::NumRegs(100));
    REQUIRE(req.getStartAddress().get() == 123);
    REQUIRE(req.getNumValues().get() == 100);
}

TEST_CASE("modbus::tcp::ReadInputRegistersReq", "[requests]") {
    modbus::tcp::ReadInputRegistersReq req(modbus::tcp::Address(123), modbus::tcp::NumRegs(100));
    REQUIRE(req.getStartAddress().get() == 123);
    REQUIRE(req.getNumValues().get() == 100);
}


TEST_CASE("modbus::tcp::WriteSingleCoilReq", "[requests]") {
    modbus::tcp::WriteSingleCoilReq req(modbus::tcp::Address(123), true);
    REQUIRE(req.getAddress().get() == 123);
    REQUIRE(req.getValue() == 0xFF00);
}


TEST_CASE("modbus::tcp::WriteSingleSingleRegisterReq", "[requests]") {
    modbus::tcp::WriteSingleRegisterReq req(modbus::tcp::Address(123), 1024);
    REQUIRE(req.getAddress().get() == 123);
    REQUIRE(req.getValue() == 1024);
}


#endif
