#ifndef MODBUS_SERIALIZED_HPP
#define MODBUS_SERIALIZED_HPP

#include <stdint.h>
#include <string.h>


#define NUM_BITS_TO_BYTES(bits) ((bits) / 8 + ((bits) % 8 == 0 ? 0 : 1))

namespace modbus {
namespace tcp {

namespace serialized {

struct Header {
    inline                      Header();

    uint16_t                    transactionId;
    uint16_t                    protocolId;
    uint16_t                    length;
    uint8_t                     unitId;
    uint8_t                     functionCode;
} __attribute__((packed));


struct ReadValuesReq {
    inline                      ReadValuesReq();
    Header                      header;
    uint16_t                    startAddr;
    uint16_t                    numValues;
} __attribute__((packed));


struct WriteValueReq {
    inline                      WriteValueReq();
    Header                      header;
    uint16_t                    address;
    uint16_t                    value;
} __attribute__((packed));


struct SetBitsRequest {
    inline                      SetBitsRequest();
    Header                      header;
    uint16_t                    startAddr;
    uint16_t                    numValues;
    uint8_t                     numBytes;
    uint8_t                     values[NUM_BITS_TO_BYTES(MODBUS_MAX_NUM_BITS_IN_WRITE_REQUEST)];
} __attribute__((packed));


struct SetRegsRequest {
    inline                      SetRegsRequest();
    Header                      header;
    uint16_t                    startAddr;
    uint16_t                    numValues;
    uint8_t                     numBytes;
    uint16_t                    values[MODBUS_MAX_NUM_REGS_IN_WRITE_REQUEST];
} __attribute__((packed));


struct ErrorResponse {
    inline                      ErrorResponse();
    Header                      header;
    uint8_t                     errCode;
} __attribute__((packed));


struct GetBitsResponse {
    inline                      GetBitsResponse();
    Header                      header;
    uint8_t                     numBytes;
    uint8_t                     values[NUM_BITS_TO_BYTES(MODBUS_MAX_NUM_BITS_IN_READ_REQUEST)];
} __attribute__((packed));


struct GetRegsResponse {
    inline                      GetRegsResponse();
    Header                      header;
    uint8_t                     numBytes;
    uint16_t                    values[MODBUS_MAX_NUM_BITS_IN_READ_REQUEST];
} __attribute__((packed));


struct WriteValueResponse {
    inline                      WriteValueResponse();
    Header                      header;
    uint16_t                    address;
    uint16_t                    value;
} __attribute__((packed));


struct WriteValuesResponse {
    inline                      WriteValuesResponse();
    Header                      header;
    uint16_t                    startAddr;
    uint16_t                    numValues;
};


Header::Header():
    transactionId(0),
    protocolId(0),
    length(0),
    unitId(0),
    functionCode(0)
{}


ReadValuesReq::ReadValuesReq() :
    header(),
    startAddr(0),
    numValues(0)
{}


WriteValueReq::WriteValueReq() :
    header(),
    address(),
    value()
{}


SetBitsRequest::SetBitsRequest() :
    header(),
    startAddr(0),
    numValues(0),
    numBytes(0)
{
    memset(values, 0, sizeof(values));
}


SetRegsRequest::SetRegsRequest() :
    header(),
    startAddr(0),
    numValues(0),
    numBytes(0)
{
    memset(values, 0, sizeof(values));
}


ErrorResponse::ErrorResponse() :
    header(),
    errCode(0)
{}


GetBitsResponse::GetBitsResponse() :
    header(),
    numBytes(0)
{
    memset(values, 0, sizeof(values));
}


GetRegsResponse::GetRegsResponse() :
    header(),
    numBytes(0)
{
    memset(values, 0, sizeof(values));
}


WriteValueResponse::WriteValueResponse() :
    header(),
    address(0),
    value(0)
{}


WriteValuesResponse::WriteValuesResponse() :
    header(),
    startAddr(0),
    numValues(0)
{} 

} // namespace serialized
} // namspace tcp
} // namespace modbus

#endif

