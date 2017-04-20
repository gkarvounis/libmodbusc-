#include <iostream>
#include "ModbusDevice.hpp"
#include "GenericBackend.hpp"
#include <thread>

/*
class MyBackend {
public:
    template <typename Callback>
    void  getCoils(uint16_t startAddr, uint16_t numCoils, Callback cb) {
        cb(0, true);
        throw SlaveDeviceFailure("");
    }

    template <typename Callback>
    void getDiscreteInputs(uint16_t startAddr, uint16_t numRegs, Callback cb) {
        throw SlaveDeviceFailure("");
    }

    template <typename Callback>
    void getHoldingRegs(uint16_t startAddr, uint16_t numRegs, Callback cb) {
        throw SlaveDeviceFailure("");
    }

    template <typename Callback>
    void getInputRegs(uint16_t startAddr, uint16_t numRegs, Callback cb) {
        throw SlaveDeviceFailure("");
    }

    void writeCoil(uint16_t addr, bool value) {
        throw SlaveDeviceFailure("");
    }

    void writeRegister(uint16_t addr, uint16_t value) {
        throw SlaveDeviceFailure("");
    }
};
*/

void wait_signal() {
    boost::asio::io_service io;
    boost::asio::signal_set signals(io);
    signals.add(SIGINT);

    signals.async_wait([](const boost::system::error_code& /*ec*/, int /*signum*/) {
    });

    io.run();
}

int main() {
    boost::asio::io_service io;
    ModbusGenericBackend backend;
    ModbusDevice<ModbusGenericBackend> dev(io, backend);
    dev.start("127.0.0.1", 8502, []() {
        std::cout << "done" << std::endl;
    });

    std::thread t([&io]() { io.run(); });

    wait_signal();

    dev.stop();
    t.join();
}

