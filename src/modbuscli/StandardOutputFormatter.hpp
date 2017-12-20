#ifndef STANDARD_OUTPUT_FORMATTER_HPP
#define STANDARD_OUTPUT_FORMATTER_HPP


class StandardOutputFormatter : public OutputFormatter {
public:
    void                displayOutgoing(const std::vector<uint8_t>& req) const override;
    void                displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;

private:
    static void         printMessage(const std::string& prefix, const std::vector<uint8_t>& buffer);
    static void         printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view);
    static void         displayReadBitsResult(const modbus::tcp::decoder_views::ReadCoilsReq& req_view, const modbus::tcp::decoder_views::ReadCoilsRsp& rsp_view);
};


void StandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& req) const {
    printMessage("req", req);
}


void StandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ReadCoilsReq req_view(req);
    modbus::tcp::decoder_views::ReadCoilsRsp rsp_view(rsp);
    displayReadBitsResult(req_view, rsp_view);
}


void StandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
    printMessage("rsp", rsp);

    modbus::tcp::decoder_views::Header rsp_header_view(rsp);
    printResponseHeader(rsp_header_view);

    modbus::tcp::decoder_views::ReadCoilsReq req_view(req);
    modbus::tcp::decoder_views::ReadCoilsRsp rsp_view(rsp);
    displayReadBitsResult(req_view, rsp_view);
}


void StandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
/*
      if (rsp_header_view.isError()) {
          modbus::tcp::decoder_views::ErrorResponse rsp(m_rx_buffer);
          std::cout << "Device responded with error " << static_cast<unsigned>(rsp.getCode()) << std::endl;
      }
*/
}


void StandardOutputFormatter::printMessage(const std::string& prefix, const std::vector<uint8_t>& buffer) {
    std::cout << prefix << ": [";

    for (auto c: buffer)
        std::cout << std::hex << (int)c << ' ';

    std::cout << "]" << std::endl;

}


void StandardOutputFormatter::displayReadBitsResult(const modbus::tcp::decoder_views::ReadCoilsReq& req_view, const modbus::tcp::decoder_views::ReadCoilsRsp& rsp_view) {
    uint16_t startAddress = req_view.getStartAddress().get();
    uint16_t numCoils = req_view.getNumBits().get();

    std::size_t numRows = numCoils / 8;
    if (numCoils % 8 != 0)
        numRows++;

    for (std::size_t row = 0; row < numRows; ++row) {
        uint16_t fromCoil = startAddress + row * 8;
        uint16_t toCoil = std::min(fromCoil + 7, numCoils + startAddress - 1);

        std::cout << "bits " << fromCoil << " to " << toCoil << ": ";

        for (unsigned i = fromCoil; i <= toCoil; ++i) {
            std::cout << rsp_view.getBit(i) << ' ';
        }

        std::cout << std::endl;
    }
}


void StandardOutputFormatter::printResponseHeader(const modbus::tcp::decoder_views::Header& rsp_header_view) {
    std::cout << "unitId:" << static_cast<unsigned>(rsp_header_view.getUnitId().get()) << std::endl
              << "transactionId:" << rsp_header_view.getTransactionId().get() << std::endl;
}


#endif
