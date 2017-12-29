#ifndef STANDARD_OUTPUT_FORMATTER_HPP
#define STANDARD_OUTPUT_FORMATTER_HPP


class StandardOutputFormatter : public OutputFormatter {
public:
                        StandardOutputFormatter(std::ostream& s);

    void                displayOutgoing(const std::vector<uint8_t>& req) override;
    void                displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayWriteRegister(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
};


StandardOutputFormatter::StandardOutputFormatter(std::ostream& s) :
    OutputFormatter(s)
{}

void StandardOutputFormatter::displayOutgoing(const std::vector<uint8_t>& /*req*/) {
}


void StandardOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void StandardOutputFormatter::displayReadCoils(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void StandardOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void StandardOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void StandardOutputFormatter::displayReadHoldingRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void StandardOutputFormatter::displayWriteCoil(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void StandardOutputFormatter::displayWriteRegister(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

#endif

