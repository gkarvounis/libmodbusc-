#ifndef VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP
#define VERBOSE_STANDARD_OUTPUT_FORMATTER_HPP

#include <iomanip>

class VerboseStandardOutputFormatter : public OutputFormatter {
public:
    inline              VerboseStandardOutputFormatter();

    inline void         displayOutgoing(const std::vector<uint8_t>& req) const override;
    inline void         displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    inline void         displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;

private:
    static void         printMessage(const std::string& prefix, const std::vector<uint8_t>& buffer);
    static void         printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view);
    static void         displayReadBitsResult(const modbus::tcp::decoder_views::ReadCoilsReq& req_view, const modbus::tcp::decoder_views::ReadCoilsRsp& rsp_view);
    static void         displayReadRegsResult(const modbus::tcp::decoder_views::ReadInputRegistersReq& req_view, const modbus::tcp::decoder_views::ReadInputRegistersRsp& rsp_view);
};


VerboseStandardOutputFormatter::VerboseStandardOutputFormatter() {
}


void VerboseStandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& req) const {
    printMessage("          req", req);
}


void VerboseStandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ReadCoilsReq req_view(req);
    modbus::tcp::decoder_views::ReadCoilsRsp rsp_view(rsp);
    displayReadBitsResult(req_view, rsp_view);
}


void VerboseStandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ReadCoilsReq req_view(req);
    modbus::tcp::decoder_views::ReadCoilsRsp rsp_view(rsp);
    displayReadBitsResult(req_view, rsp_view);
}


void VerboseStandardOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ReadInputRegistersReq req_view(req);
    modbus::tcp::decoder_views::ReadInputRegistersRsp rsp_view(rsp);
    displayReadRegsResult(req_view, rsp_view);
}


void VerboseStandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("          rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ErrorResponse rsp_view(rsp);
    std::cout << "        error: " << static_cast<unsigned>(rsp_view.getCode()) << std::endl;
}


void VerboseStandardOutputFormatter::printMessage(const std::string& prefix, const std::vector<uint8_t>& buffer) {
    std::cout << prefix << ": [";

    for (auto c: buffer)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c << ' ';

    std::cout << "]" << std::endl;

}


void VerboseStandardOutputFormatter::displayReadBitsResult(const modbus::tcp::decoder_views::ReadCoilsReq& req_view, const modbus::tcp::decoder_views::ReadCoilsRsp& rsp_view) {
    std::cout << "         bits: ";

    for(std::size_t i = 0; i < req_view.getNumBits().get(); ++i)
        std::cout << rsp_view.getBit(i) << ' ';

    std::cout << std::endl;
}


void VerboseStandardOutputFormatter::displayReadRegsResult(const modbus::tcp::decoder_views::ReadInputRegistersReq& req_view, const modbus::tcp::decoder_views::ReadInputRegistersRsp& rsp_view) {
    std::cout << "    registers: ";

    for (std::size_t i = 0; i < req_view.getNumRegs().get(); ++i)
        std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex << rsp_view.getRegister(i) << ' ';

    std::cout << std::endl;
}


void VerboseStandardOutputFormatter::printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view) {
    std::cout << "       unitId: " << static_cast<unsigned>(rsp_header_view.getUnitId().get()) << std::endl
              << "function code: " << static_cast<unsigned>(rsp_header_view.getFunctionCode()) << std::endl
              << "transactionId: " << rsp_header_view.getTransactionId().get() << std::endl;
}


#endif
