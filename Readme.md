# libmodbusc++

Work in progress.

## Encoding a modbus message
In order to enode a modbus message you only need to include ModbusEncoder.hpp - no linking is needed since the code is contained in header files. Just set your -I flags such that your compiler can find this file.

Encoding a modbus message takes three steps:

 1. Allocate a buffer to hold the encoded message (can be allocated on the stack)
 2. Allocate an encoder object that will perform the encoding (can be allocated on the stack).
 3. Set message parameters using the encoder object.

Following example shows how to implement those steps for a read coils request
```
#include "ModbusEncoder.hpp"

int main() {
  // A stack allocated buffer with enough capacity to hold read coils modbus request
  modbus::tcp::encoder::ReadCoilsReq::Buffer buf;

  // An object that can encode the request. Encoded result is stored in buf.
  modbus::tcp::encoder::ReadCoilsReq req(buf);

  // Set parameters for the request
  req.setUnitId(123);
  req.setTransactionId(0x5678);
  req.setStartAddress(0x1234);
  req.setNumValues(128);

  // send encoded message from the buffer over some socket object.
  // req.message_size() returns the number that should be sent.
  socket.send(
    reinterpret_cast<uint8_t*>(&buf), 
    req.message_size());
}
```
