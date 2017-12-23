#ifndef STANDARD_OUTPUT_FORMATTER_HPP
#define STANDARD_OUTPUT_FORMATTER_HPP


class StandardOutputFormatter : public OutputFormatter {
public:
    void                displayOutgoing(const std::vector<uint8_t>& req) const override;
    void                displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
};


void StandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& req) const {
}


void StandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
}


void StandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
}


void StandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
}


void StandardOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const {
}

#endif

