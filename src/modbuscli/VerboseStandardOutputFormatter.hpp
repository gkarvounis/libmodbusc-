#ifndef VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP
#define VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP

#include <iomanip>

class VerboseStandardOutputFormatter : public OutputFormatter {
public:
                        VerboseStandardOutputFormatter(std::ostream& s);

    inline void         displayOutgoing(const std::vector<uint8_t>& req) override;
    inline void         displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    inline void         displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    inline void         displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    inline void         displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    inline void         displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;

    inline void         displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    inline void         displayWriteRegister(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;

private:
    void                printBuffer(const std::string& prefix, const std::vector<uint8_t>& buffer);
    void                printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view);

    template <typename ReadBitsReqView, typename ReadBitsRspView>
    void               displayReadBitsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp);

    template <typename ReadRegsReqView, typename ReadRegsRspView>
    void               displayReadRegsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp);
};


VerboseStandardOutputFormatter::VerboseStandardOutputFormatter(std::ostream& s) :
    OutputFormatter(s)
{}


void VerboseStandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& req) {
    printBuffer("          req", req);
}


void VerboseStandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    displayReadBitsResult<
        modbus::tcp::decoder_views::ReadCoilsReq,
        modbus::tcp::decoder_views::ReadCoilsRsp>(req, rsp);
}


void VerboseStandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    displayReadBitsResult<
        modbus::tcp::decoder_views::ReadDiscreteInputsReq,
        modbus::tcp::decoder_views::ReadDiscreteInputsRsp>(req, rsp);
}


template <typename ReadBitsReqView, typename ReadBitsRspView>
void VerboseStandardOutputFormatter::displayReadBitsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    m_outStream << "         bits: ";

    ReadBitsReqView req_view(req);
    ReadBitsRspView rsp_view(rsp);

    for(std::size_t i = 0; i < req_view.getNumBits().get(); ++i)
        m_outStream << rsp_view.getBit(i) << ' ';

    m_outStream << std::endl;
}


void VerboseStandardOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    displayReadRegsResult<
        modbus::tcp::decoder_views::ReadInputRegistersReq,
        modbus::tcp::decoder_views::ReadInputRegistersRsp>(req, rsp);
}


void VerboseStandardOutputFormatter::displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    displayReadRegsResult<
        modbus::tcp::decoder_views::ReadHoldingRegistersReq,
        modbus::tcp::decoder_views::ReadHoldingRegistersRsp>(req, rsp);
}


template <typename ReadRegsReqView, typename ReadRegsRspView>
void VerboseStandardOutputFormatter::displayReadRegsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    m_outStream << "    registers: ";

    modbus::tcp::decoder_views::ReadInputRegistersReq req_view(req);
    modbus::tcp::decoder_views::ReadInputRegistersRsp rsp_view(rsp);
    
    for (std::size_t i = 0; i < req_view.getNumRegs().get(); ++i)
        m_outStream << "0x" << std::setfill('0') << std::setw(4) << std::hex << rsp_view.getRegister(i) << ' ';

    m_outStream << std::endl;
}


void VerboseStandardOutputFormatter::displayWriteCoil(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::WriteSingleCoilRsp rsp_view(rsp);
    std::size_t address = rsp_view.getAddress().get();

    m_outStream << " coil address: " << "0x" << std::hex << std::setfill('0') << std::setw(4) << address << std::endl
              << "    new value: " << rsp_view.getValue() << std::endl;
}


void VerboseStandardOutputFormatter::displayWriteRegister(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::WriteSingleRegisterRsp rsp_view(rsp);
    std::size_t address = rsp_view.getAddress().get();

    m_outStream << "  reg address: " << "0x" << std::hex << std::setfill('0') << std::setw(4) << address << std::dec << " (" << address << ")" << std::endl
              << "    new value: " << "0x" << std::hex << std::setfill('0') << std::setw(4) << rsp_view.getValue() << " (" << std::dec << rsp_view.getValue() << ")"  << std::endl;
}


void VerboseStandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ErrorResponse rsp_view(rsp);
    m_outStream << "        error: " << static_cast<unsigned>(rsp_view.getCode()) << std::endl;
}


void VerboseStandardOutputFormatter::printBuffer(const std::string& prefix, const std::vector<uint8_t>& buffer) {
    m_outStream << prefix << ": [";

    for (auto c: buffer)
        m_outStream << std::setfill('0') << std::setw(2) << std::hex << (int)c << ' ';

    m_outStream << "]" << std::endl;
}


void VerboseStandardOutputFormatter::printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view) {
    m_outStream << "       unitId: " << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(rsp_header_view.getUnitId().get()) << std::endl
              << "function code: " << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(rsp_header_view.getFunctionCode()) << std::endl
              << "transactionId: " << "0x" << std::hex << std::setfill('0') << std::setw(4) <<rsp_header_view.getTransactionId().get() << std::endl;
}


#endif
