#ifndef MODBUS_DECODER_HPP
#define MODBUS_DECODER_HPP

#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include <arpa/inet.h>


namespace modbus {
namespace tcp {
namespace decoder_views {


class Header {
public:
                                Header(const std::vector<uint8_t>& rx_buffer);

    TransactionId               getTransactionId() const;
    std::size_t                 getLength() const;
    UnitId                      getUnitId() const;
    FunctionCode                getFunctionCode() const;
    bool                        isError() const;
private:
    const modbus::tcp::Header  *m_header;
};


template <int dummy>
class ReadBitsReq {
public:
                                ReadBitsReq(const std::vector<uint8_t>& rx_buffer);
    Address                     getStartAddress() const;
    NumBits                     getNumBits() const;
private:
    const ReadReq              *m_read_req;
};


template <int dummy>
class ReadRegistersReq {
public:
                                ReadRegistersReq(const std::vector<uint8_t>& rx_buffer);
    Address                     getStartAddress() const;
    NumRegs                     getNumRegs() const;
private:
    const ReadReq              *m_read_req;
};


template <int dummy>
class ReadBitsRsp {
public:
                                ReadBitsRsp(const std::vector<uint8_t>& rx_buffer);
    uint8_t                     getNumBits() const;
    bool                        getBit(std::size_t pos);
private:
    const modbus::tcp::ReadCoilsRsp *m_read_rsp;
};


template <int dummy>
class ReadRegistersRsp {
public:
                                ReadRegistersRsp(const std::vector<uint8_t>& rx_buffer);
    uint16_t                    getNumRegs() const;
    uint16_t                    getRegister(uint16_t idx) const;
private:
    const modbus::tcp::ReadRegsRsp* m_read_rsp;
};


template <int dummy>
class WriteSingleCoil {
public:
                                WriteSingleCoil(const std::vector<uint8_t>& rx_buffer);
    Address                     getAddress() const;
    bool                        getValue() const;
private:
    const WriteSingleValue     *m_write_req;
};


template <int dummy>
class WriteSingleRegister {
public:
                                WriteSingleRegister(const std::vector<uint8_t>& rx_buffer);
    Address                     getAddress() const;
    uint16_t                    getValue() const;
private:
    const WriteSingleValue     *m_write_req;
};


using ReadCoilsReq = ReadBitsReq<0>;
using ReadDiscreteInputsReq = ReadBitsReq<1>;
using ReadHoldingRegistersReq = ReadRegistersReq<0>;
using ReadInputRegistersReq = ReadRegistersReq<1>;
using WriteSingleCoilReq = WriteSingleCoil<0>;
using WriteSingleRegisterReq = WriteSingleRegister<0>;

using ReadCoilsRsp = ReadBitsRsp<0>;
using ReadDiscreteInputsRsp = ReadBitsRsp<1>;
using ReadHoldingRegistersRsp = ReadRegistersRsp<0>;
using ReadInputRegistersRsp = ReadRegistersRsp<1>;
using WriteSingleCoilRsp = WriteSingleCoil<1>;
using WriteSingleRegisterRsp = WriteSingleRegister<1>;


class WriteCoilsReq {
public:
                                WriteCoilsReq(const std::vector<uint8_t>& rx_buffer);

    modbus::tcp::Address        getStartAddress() const;
    modbus::tcp::NumBits        getNumBits() const;
    bool                        getCoil(uint16_t idx) const;
private:
    const modbus::tcp::WriteCoilsReq* m_req;
};


class WriteCoilsRsp {
public:
                            WriteCoilsRsp(const std::vector<uint8_t>& rx_buffer);
    modbus::tcp::Address    getStartAddress() const;
    modbus::tcp::NumBits    getNumBits() const;

private:
    const modbus::tcp::WriteValuesRsp* m_rsp;
};


class WriteRegistersReq {
public:
                            WriteRegistersReq(const std::vector<uint8_t>& rx_buffer);
    modbus::tcp::Address    getStartAddress() const;
    modbus::tcp::NumRegs    getNumRegs() const;
    uint16_t                getRegister(uint16_t idx) const;

private:
    const modbus::tcp::WriteRegistersReq* m_req;
};


class WriteRegistersRsp {
public:
                            WriteRegistersRsp(const std::vector<uint8_t>& rx_buffer);
    modbus::tcp::Address    getStartAddress() const;
    modbus::tcp::NumRegs    getNumRegs() const;

private:
    const modbus::tcp::WriteValuesRsp* m_rsp;
};


class ErrorResponse {
public:
                                ErrorResponse(const std::vector<uint8_t>& rx_buffer);
    ExceptionCode               getCode() const;
private:
    const modbus::tcp::ExceptionRsp* m_response;
};


Header::Header(const std::vector<uint8_t>& rx_buffer) :
    m_header(reinterpret_cast<const modbus::tcp::Header*>(rx_buffer.data()))
{}


TransactionId Header::getTransactionId() const {
    return TransactionId(ntohs(m_header->transactionId));
}


std::size_t Header::getLength() const {
    return ntohs(m_header->length);
}


UnitId Header::getUnitId() const {
    return UnitId(m_header->unitId);
}


FunctionCode Header::getFunctionCode() const {
    switch (m_header->functionCode & 0x7F) {
        case static_cast<uint8_t>(FunctionCode::READ_COILS):
        case static_cast<uint8_t>(FunctionCode::READ_DISCRETE_INPUTS):
        case static_cast<uint8_t>(FunctionCode::READ_HOLDING_REGISTERS):
        case static_cast<uint8_t>(FunctionCode::READ_INPUT_REGISTERS):
        case static_cast<uint8_t>(FunctionCode::WRITE_COIL):
        case static_cast<uint8_t>(FunctionCode::WRITE_REGISTER):
        case static_cast<uint8_t>(FunctionCode::WRITE_COILS):
        case static_cast<uint8_t>(FunctionCode::WRITE_REGISTERS):
            return static_cast<FunctionCode>(m_header->functionCode & 0x7F);

        default:
            throw std::runtime_error("Invalid function code received");
    }
}


bool Header::isError() const {
    return m_header->functionCode & 0x80;
}


template <int dummy>
ReadBitsReq<dummy>::ReadBitsReq(const std::vector<uint8_t>& rx_buffer) :
    m_read_req(reinterpret_cast<const ReadReq*>(rx_buffer.data()))
{}


template <int dummy>
Address ReadBitsReq<dummy>::getStartAddress() const {
    return Address(ntohs(m_read_req->startAddress));
}


template <int dummy>
NumBits ReadBitsReq<dummy>::getNumBits() const {
    return NumBits(ntohs(m_read_req->numEntries));
}


template <int dummy>
ReadRegistersReq<dummy>::ReadRegistersReq(const std::vector<uint8_t>& rx_buffer) :
    m_read_req(reinterpret_cast<const ReadReq*>(rx_buffer.data()))
{}


template <int dummy>
Address ReadRegistersReq<dummy>::getStartAddress() const {
    return Address(ntohs(m_read_req->startAddress));
}


template <int dummy>
NumRegs ReadRegistersReq<dummy>::getNumRegs() const {
    return NumRegs(ntohs(m_read_req->numEntries));
}


template <int dummy>
WriteSingleCoil<dummy>::WriteSingleCoil(const std::vector<uint8_t>& rx_buffer) :
    m_write_req(reinterpret_cast<const WriteSingleValue*>(rx_buffer.data()))
{}


template <int dummy>
Address WriteSingleCoil<dummy>::getAddress() const {
    return Address(ntohs(m_write_req->address));
}

template <int dummy>
bool WriteSingleCoil<dummy>::getValue() const {
    return m_write_req->value != 0;
}


template <int dummy>
WriteSingleRegister<dummy>::WriteSingleRegister(const std::vector<uint8_t>& rx_buffer) :
    m_write_req(reinterpret_cast<const WriteSingleValue*>(rx_buffer.data()))
{}


template <int dummy>
Address WriteSingleRegister<dummy>::getAddress() const {
    return Address(ntohs(m_write_req->address));
}


template <int dummy>
uint16_t WriteSingleRegister<dummy>::getValue() const {
    return ntohs(m_write_req->value);
}


template <int dummy>
ReadBitsRsp<dummy>::ReadBitsRsp(const std::vector<uint8_t>& rx_buffer) :
    m_read_rsp(reinterpret_cast<const modbus::tcp::ReadCoilsRsp*>(rx_buffer.data()))
{}


template <int dummy>
uint8_t ReadBitsRsp<dummy>::getNumBits() const {
    return m_read_rsp->numBytes * 8;
}


template <int dummy>
bool ReadBitsRsp<dummy>::getBit(std::size_t pos) {
    std::size_t idx = pos / 8;
    std::size_t mask = 1 << (pos % 8);

    return m_read_rsp->coils[idx] & mask;
}


template <int dummy>
ReadRegistersRsp<dummy>::ReadRegistersRsp(const std::vector<uint8_t>& rx_buffer) :
    m_read_rsp(reinterpret_cast<const modbus::tcp::ReadRegsRsp*>(rx_buffer.data()))
{}


template <int dummy>
uint16_t ReadRegistersRsp<dummy>::getNumRegs() const {
    return m_read_rsp->numBytes / 2;
}


template <int dummy>
uint16_t ReadRegistersRsp<dummy>::getRegister(uint16_t idx) const {
    return ntohs(m_read_rsp->regs[idx]);
}


WriteCoilsReq::WriteCoilsReq(const std::vector<uint8_t>& rx_buffer) :
    m_req(reinterpret_cast<const modbus::tcp::WriteCoilsReq*>(rx_buffer.data()))
{}


modbus::tcp::Address WriteCoilsReq::getStartAddress() const {
    return modbus::tcp::Address(ntohs(m_req->startAddress));
}


modbus::tcp::NumBits WriteCoilsReq::getNumBits() const {
    return modbus::tcp::NumBits(ntohs(m_req->numBits));

}


bool WriteCoilsReq::getCoil(uint16_t idx) const {
    uint16_t pos = idx / 8;
    uint16_t mask = 1 << (idx%8);

    return m_req->coils[pos] & mask;
}


WriteCoilsRsp::WriteCoilsRsp(const std::vector<uint8_t>& rx_buffer) :
    m_rsp(reinterpret_cast<const modbus::tcp::WriteValuesRsp*>(rx_buffer.data()))
{}


modbus::tcp::Address WriteCoilsRsp::getStartAddress() const {
    return modbus::tcp::Address(ntohs(m_rsp->startAddress));
}


modbus::tcp::NumBits WriteCoilsRsp::getNumBits() const {
    return modbus::tcp::NumBits(ntohs(m_rsp->numValues));
}


WriteRegistersReq::WriteRegistersReq(const std::vector<uint8_t>& rx_buffer) :
    m_req(reinterpret_cast<const modbus::tcp::WriteRegistersReq*>(rx_buffer.data()))
{}


modbus::tcp::Address WriteRegistersReq::getStartAddress() const {
    return modbus::tcp::Address(ntohs(m_req->startAddress));
}


modbus::tcp::NumRegs WriteRegistersReq::getNumRegs() const {
    return modbus::tcp::NumRegs(ntohs(m_req->numRegs));
}


uint16_t WriteRegistersReq::getRegister(uint16_t idx) const {
    return ntohs(m_req->regs[idx]);
}


WriteRegistersRsp::WriteRegistersRsp(const std::vector<uint8_t>& rx_buffer) :
    m_rsp(reinterpret_cast<const modbus::tcp::WriteValuesRsp*>(rx_buffer.data()))
{
}


modbus::tcp::Address WriteRegistersRsp::getStartAddress() const {
    return modbus::tcp::Address(ntohs(m_rsp->startAddress));
}


modbus::tcp::NumRegs WriteRegistersRsp::getNumRegs() const {
    return modbus::tcp::NumRegs(ntohs(m_rsp->numValues));
}





ErrorResponse::ErrorResponse(const std::vector<uint8_t>& rx_buffer) :
    m_response(reinterpret_cast<const ExceptionRsp*>(rx_buffer.data()))
{}


ExceptionCode ErrorResponse::getCode() const {
    switch (m_response->code) {
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_FUNCTION):
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_DATA_ADDRESS):
        case static_cast<uint8_t>(ExceptionCode::ILLEGAL_DATA_VALUE):
        case static_cast<uint8_t>(ExceptionCode::SLAVE_DEVICE_FAILURE):
        case static_cast<uint8_t>(ExceptionCode::ACKNOWLEDGE):
        case static_cast<uint8_t>(ExceptionCode::SLAVE_DEVICE_BUSY):
        case static_cast<uint8_t>(ExceptionCode::MEMORY_PARITY_ERROR):
        case static_cast<uint8_t>(ExceptionCode::GATEWAY_PATH_UNAVAILABLE):
        case static_cast<uint8_t>(ExceptionCode::GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND):
            return static_cast<ExceptionCode>(m_response->code);

        default:
            throw std::runtime_error("Invalid exception error response code");
    }
}

} // namespace decoder
} // namespace tcp
} // namespace modbus

#endif
