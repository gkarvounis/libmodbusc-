#ifndef VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP
#define VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP

#include <iomanip>

class VerboseStandardOutputFormatter : public OutputFormatter {
public:
    inline void         displayOutgoing(const std::vector<uint8_t>& req) const override;
    inline void         displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;

    inline void         displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;

private:
    static void         printBuffer(const std::string& prefix, const std::vector<uint8_t>& buffer);
    static void         printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view);

    template <typename ReadBitsReqView, typename ReadBitsRspView>
    static void         displayReadBitsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp);

    template <typename ReadRegsReqView, typename ReadRegsRspView>
    static void         displayReadRegsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp);
};


void VerboseStandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& req) const {
    printBuffer("          req", req);
}


void VerboseStandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    displayReadBitsResult<
        modbus::tcp::decoder_views::ReadCoilsReq,
        modbus::tcp::decoder_views::ReadCoilsRsp>(req, rsp);
}


void VerboseStandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    displayReadBitsResult<
        modbus::tcp::decoder_views::ReadDiscreteInputsReq,
        modbus::tcp::decoder_views::ReadDiscreteInputsRsp>(req, rsp);
}


template <typename ReadBitsReqView, typename ReadBitsRspView>
void VerboseStandardOutputFormatter::displayReadBitsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    std::cout << "         bits: ";

    ReadBitsReqView req_view(req);
    ReadBitsRspView rsp_view(rsp);

    for(std::size_t i = 0; i < req_view.getNumBits().get(); ++i)
        std::cout << rsp_view.getBit(i) << ' ';

    std::cout << std::endl;
}


void VerboseStandardOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    displayReadRegsResult<
        modbus::tcp::decoder_views::ReadInputRegistersReq,
        modbus::tcp::decoder_views::ReadInputRegistersRsp>(req, rsp);
}


void VerboseStandardOutputFormatter::displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    displayReadRegsResult<
        modbus::tcp::decoder_views::ReadHoldingRegistersReq,
        modbus::tcp::decoder_views::ReadHoldingRegistersRsp>(req, rsp);
}


template <typename ReadRegsReqView, typename ReadRegsRspView>
void VerboseStandardOutputFormatter::displayReadRegsResult(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    std::cout << "    registers: ";

    modbus::tcp::decoder_views::ReadInputRegistersReq req_view(req);
    modbus::tcp::decoder_views::ReadInputRegistersRsp rsp_view(rsp);
    
    for (std::size_t i = 0; i < req_view.getNumRegs().get(); ++i)
        std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex << rsp_view.getRegister(i) << ' ';

    std::cout << std::endl;
}


void VerboseStandardOutputFormatter::displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::WriteSingleCoilRsp rsp_view(rsp);
    std::cout << " coil address: " << static_cast<std::size_t>(rsp_view.getAddress().get()) << std::endl
              << "    new value: " << rsp_view.getValue() << std::endl;
}


void VerboseStandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printBuffer("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ErrorResponse rsp_view(rsp);
    std::cout << "        error: " << static_cast<unsigned>(rsp_view.getCode()) << std::endl;
}


void VerboseStandardOutputFormatter::printBuffer(const std::string& prefix, const std::vector<uint8_t>& buffer) {
    std::cout << prefix << ": [";

    for (auto c: buffer)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c << ' ';

    std::cout << "]" << std::endl;
}


void VerboseStandardOutputFormatter::printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view) {
    std::cout << "       unitId: " << static_cast<unsigned>(rsp_header_view.getUnitId().get()) << std::endl
              << "function code: " << static_cast<unsigned>(rsp_header_view.getFunctionCode()) << std::endl
              << "transactionId: " << rsp_header_view.getTransactionId().get() << std::endl;
}


#endif
