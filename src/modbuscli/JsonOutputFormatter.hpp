#ifndef JSON_OUTPUT_FORMATTER_HPP
#define JSON_OUTPUT_FORMATTER_HPP


class JsonOutputFormatter : public OutputFormatter {
public:
    void                displayOutgoing(const std::vector<uint8_t>& req) const override;
    void                displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
    void                displayWriteRegister(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) const override;
};


void JsonOutputFormatter::displayOutgoing(const std::vector<uint8_t>& /*req*/) const {
}


void JsonOutputFormatter::displayErrorResponse(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}


void JsonOutputFormatter::displayReadCoils(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}


void JsonOutputFormatter::displayReadDiscreteInputs(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}


void JsonOutputFormatter::displayReadInputRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}

void JsonOutputFormatter::displayReadHoldingRegisters(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}

void JsonOutputFormatter::displayWriteCoil(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}

void JsonOutputFormatter::displayWriteRegister(const std::vector<uint8_t>& /*req*/, const std::vector<uint8_t>& /*rsp*/) const {
}

#endif

