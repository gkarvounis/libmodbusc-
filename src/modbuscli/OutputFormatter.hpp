#ifndef OUTPUT_FORMATTER_HPP
#define OUTPUT_FORMATTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"


class OutputFormatter {
public:
                        OutputFormatter(std::ostream& s);

    virtual void        displayOutgoing(const std::vector<uint8_t>& req) = 0;
    virtual void        displayErrorResponse(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;
    virtual void        displayReadCoils(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;
    virtual void        displayReadDiscreteInputs(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;
    virtual void        displayReadInputRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;
    virtual void        displayReadHoldingRegisters(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;

    virtual void        displayWriteCoil(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;
    virtual void        displayWriteRegister(const std::vector<uint8_t>& req, const std::vector<uint8_t>& rsp) = 0;

protected:
    std::ostream       &m_outStream;
};


OutputFormatter::OutputFormatter(std::ostream& s) :
    m_outStream(s)
{}

#endif
