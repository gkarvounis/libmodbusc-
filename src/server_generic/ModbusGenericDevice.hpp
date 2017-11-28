#ifndef GENERIC_BACKEND_HPP
#define GENERIC_BACKEND_HPP

class ModbusGenericDevice {
public:
    inline ModbusGenericDevice();

    template <typename Callback>
    void  getCoils(uint16_t startAddr, uint16_t numCoils, Callback cb) {
        std::size_t fromCoil = startAddr;
        std::size_t toCoil = fromCoil + numCoils;

        if (toCoil >= 0x00FFFF)
            throw IllegalDataAddress("request for coils up to " + std::to_string(toCoil-1));

        for (std::size_t i = startAddr; i < startAddr + numCoils; ++i)
            cb(i - startAddr, m_coils[i]);
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

private:
    std::vector<bool>       m_coils;
    std::vector<bool>       m_discreteInputs;
    std::vector<uint16_t>   m_holdingRegs;
    std::vector<uint16_t>   m_inputRegs;
};


ModbusGenericDevice::ModbusGenericDevice() :
    m_coils(0xFFFF),
    m_discreteInputs(0xFFFF),
    m_holdingRegs(0xFFFF),
    m_inputRegs(0xFFFF)
{}

#endif

