#ifndef JSON_OUTPUT_FORMATTER_HPP
#define JSON_OUTPUT_FORMATTER_HPP


class JsonOutputFormatter : public OutputFormatter {
public:
                        JsonOutputFormatter(std::ostream& s);

    void                displayOutgoing(const std::vector<uint8_t>& req) override;
    void                displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
    void                displayWriteRegister(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) override;
};


JsonOutputFormatter::JsonOutputFormatter(std::ostream& s) :
    OutputFormatter(s)
{}

void JsonOutputFormatter::displayOutgoing(const std::vector<uint8_t>& /*req*/) {
}


void JsonOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void JsonOutputFormatter::displayReadCoils(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void JsonOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}


void JsonOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void JsonOutputFormatter::displayReadHoldingRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void JsonOutputFormatter::displayWriteCoil(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

void JsonOutputFormatter::displayWriteRegister(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) {
}

#endif

