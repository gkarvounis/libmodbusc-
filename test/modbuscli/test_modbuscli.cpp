#define CATCH_CONFIG_MAIN

#include "Catch.hpp"
#include "ModbusConsts.hpp"
#include "ModbusTypes.hpp"
#include "ModbusEncoder.hpp"
#include "ModbusDecoder.hpp"
#include "OutputFormatter.hpp"
#include "VerboseStandardOutputFormatter.hpp"
#include "ModbusClient.hpp"
#include "ModbusCommand.hpp"
#include "ModbusReadCoilsCommand.hpp"

#include <thread>

#include "test_readcoils.hpp"

