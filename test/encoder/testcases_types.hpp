#ifndef TESTCASES_TYPES_HPP
#define TESTCASES_TYPES_HPP

TEST_CASE("modbus::tcp::TransactionId", "[types]") {
    modbus::tcp::TransactionId transactionId(123);

    REQUIRE(sizeof(transactionId.get()) == 2);
    REQUIRE(transactionId.get() == 123);
}

TEST_CASE("modbus::tcp::MODBUS_PROTOCOL_ID", "[types]") {
    REQUIRE(sizeof(modbus::tcp::MODBUS_PROTOCOL_ID) == 2);
}


TEST_CASE("modbus::tcp::UnitId", "[types]") {
    modbus::tcp::UnitId unitId(1);
    REQUIRE(sizeof(unitId.get()) == 1);
    REQUIRE(unitId.get() == 1);
}


TEST_CASE("modbus::tcp::StartAddress", "[types]") {
    modbus::tcp::Address startAddress(123);
    REQUIRE(sizeof(startAddress.get()) == 2);
    REQUIRE(startAddress.get() == 123);
}


TEST_CASE("modbus::tcp::NumBits", "[types]") {
    modbus::tcp::NumBits numBits(100);
    REQUIRE(sizeof(numBits.get()) == 2);
    REQUIRE(numBits.get() == 100);
}


TEST_CASE("modbus::tcp::NumCoils bounds check", "[types]") {
    REQUIRE_THROWS_AS([]() {
        modbus::tcp::NumBits numCoils(0);
    }(), modbus::tcp::NumBitsOutOfRange);

    REQUIRE_NOTHROW([]() {
        modbus::tcp::NumBits numCoils1(1);
        modbus::tcp::NumBits numCoils2(2000);
    }());

    REQUIRE_THROWS_AS([]() {
        modbus::tcp::NumBits numCoils(2001);
    }(), modbus::tcp::NumBitsOutOfRange);

}


TEST_CASE("modbus::tcp::NumRegs", "[types]") {
    modbus::tcp::NumRegs numRegs(5);
    REQUIRE(sizeof(numRegs.get()) == 2);
    REQUIRE(numRegs.get() == 5);
}


TEST_CASE("modbus::tcp::NumRegs bounds check", "[types]") {
    REQUIRE_THROWS_AS([]() {
        modbus::tcp::NumRegs numRegs(0);
    }(), std::logic_error);


    REQUIRE_NOTHROW([]() {
        modbus::tcp::NumRegs numRegs1(1);
        modbus::tcp::NumRegs numRegs2(125);
    });

    REQUIRE_THROWS_AS([]() {
        modbus::tcp::NumRegs numRegs(126);
    }(), std::logic_error);
}

#endif
