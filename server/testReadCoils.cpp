TEST_CASE("Handling read coils request", "[]") {
    class MyBackend {
    public:
        void getCoils(uint16_t startAddr, uint16_t numCoils, std::function<void(uint16_t pos, bool value)> cb) {
            REQUIRE(startAddr == 0x0405);
            REQUIRE(numCoils == 10);
            cb(0, false);
            cb(1, true);
            cb(2, true);
            cb(3, true);
            cb(4, false);
            cb(5, true);
            cb(6, true);
            cb(7, false);
            cb(8, true);
            cb(9, true);
        }
    };

    MyBackend backend;
    uint8_t tx_buffer[1024];
    ModbusRequestHandler<MyBackend> handler(backend, tx_buffer, sizeof(tx_buffer));

    modbus::tcp::encoder::ReadCoilsReq::Buffer buf;
    modbus::tcp::encoder::ReadCoilsReq req(buf);
    req.setUnitId(0xAB);
    req.setTransactionId(0x0203);
    req.setStartAddress(0x0405);
    req.setNumValues(10);

    const auto* rsp = reinterpret_cast<const modbus::tcp::encoder::ReadCoilsRsp::Buffer*>(tx_buffer);

    REQUIRE(handler.handleReadCoils(buf) == 11);
    REQUIRE(rsp->header.transactionId == htons(0x0203));
    REQUIRE(rsp->header.unitId == 0xAB);
    REQUIRE(rsp->header.functionCode == modbus::tcp::READ_COILS);
    REQUIRE(rsp->numBytes == 2);
    REQUIRE((uint32_t)rsp->values[0] == 0b01101110);
    REQUIRE((uint32_t)rsp->values[1] == 0b00000011);
}


TEST_CASE("Handling read coils request - Illegal function", "[]") {
    class MyBackend {
    public:
        void getCoils(uint16_t startAddr, uint16_t numCoils, std::function<void(uint16_t pos, bool value)> /*cb*/) {
            REQUIRE(startAddr == 0x0405);
            REQUIRE(numCoils == 10);
            throw IllegalFunction(modbus::tcp::READ_COILS);
        }
    };

    MyBackend backend;
    uint8_t tx_buffer[1024];
    ModbusRequestHandler<MyBackend> handler(backend, tx_buffer, sizeof(tx_buffer));

    modbus::tcp::encoder::ReadCoilsReq::Buffer buf;
    modbus::tcp::encoder::ReadCoilsReq req(buf);
    req.setUnitId(0xAB);
    req.setTransactionId(0x0203);
    req.setStartAddress(0x0405);
    req.setNumValues(10);

    const auto* rsp = reinterpret_cast<const modbus::tcp::encoder::ErrorResponse::Buffer*>(tx_buffer);

    REQUIRE(handler.handleReadCoils(buf) == sizeof(modbus::tcp::encoder::ErrorResponse::Buffer));
    REQUIRE(rsp->header.transactionId == htons(0x0203));
    REQUIRE(rsp->header.unitId == 0xAB);
    REQUIRE(rsp->header.functionCode == (0x80 | modbus::tcp::READ_COILS));
    REQUIRE(rsp->errCode == modbus::tcp::ILLEGAL_FUNCTION);
}


TEST_CASE("Handling read coils request - Device error", "[]") {
    class MyBackend {
    public:
        void getCoils(uint16_t startAddr, uint16_t numCoils, std::function<void(uint16_t pos, bool value)> /*cb*/) {
            REQUIRE(startAddr == 0x0405);
            REQUIRE(numCoils == 10);
            throw SlaveDeviceFailure("...");
        }
    };

    MyBackend backend;
    uint8_t tx_buffer[1024];
    ModbusRequestHandler<MyBackend> handler(backend, tx_buffer, sizeof(tx_buffer));

    modbus::tcp::encoder::ReadCoilsReq::Buffer buf;
    modbus::tcp::encoder::ReadCoilsReq req(buf);
    req.setUnitId(0xAB);
    req.setTransactionId(0x0203);
    req.setStartAddress(0x0405);
    req.setNumValues(10);

    const auto* rsp = reinterpret_cast<const modbus::tcp::encoder::ErrorResponse::Buffer*>(tx_buffer);

    REQUIRE(handler.handleReadCoils(buf) == sizeof(modbus::tcp::encoder::ErrorResponse::Buffer));
    REQUIRE(rsp->header.transactionId == htons(0x0203));
    REQUIRE(rsp->header.unitId == 0xAB);
    REQUIRE(rsp->header.functionCode == (0x80 | modbus::tcp::READ_COILS));
    REQUIRE(rsp->errCode == modbus::tcp::SLAVE_DEVICE_FAILURE);
}


TEST_CASE("Handling read coils request - Illegal data address", "[]") {
    class MyBackend {
    public:
        void getCoils(uint16_t startAddr, uint16_t numCoils, std::function<void(uint16_t pos, bool value)> /*cb*/) {
            REQUIRE(startAddr == 0x0405);
            REQUIRE(numCoils == 10);
            throw IllegalDataAddress("...");
        }
    };

    MyBackend backend;
    uint8_t tx_buffer[1024];
    ModbusRequestHandler<MyBackend> handler(backend, tx_buffer, sizeof(tx_buffer));

    modbus::tcp::encoder::ReadCoilsReq::Buffer buf;
    modbus::tcp::encoder::ReadCoilsReq req(buf);
    req.setUnitId(0xAB);
    req.setTransactionId(0x0203);
    req.setStartAddress(0x0405);
    req.setNumValues(10);

    const auto* rsp = reinterpret_cast<const modbus::tcp::encoder::ErrorResponse::Buffer*>(tx_buffer);

    REQUIRE(handler.handleReadCoils(buf) == sizeof(modbus::tcp::encoder::ErrorResponse::Buffer));
    REQUIRE(rsp->header.transactionId == htons(0x0203));
    REQUIRE(rsp->header.unitId == 0xAB);
    REQUIRE(rsp->header.functionCode == (0x80 | modbus::tcp::READ_COILS));
    REQUIRE(rsp->errCode == modbus::tcp::ILLEGAL_DATA_ADDRESS);
}

