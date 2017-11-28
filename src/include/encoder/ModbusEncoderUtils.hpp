#ifndef MODBUS_ENCODER_UTILS_HPP
#define MODBUS_ENCODER_UTILS_HPP

#include <sstream>
#include <stdexcept>


namespace modbus {
namespace tcp {

class StringBuilder {
public:
    template <typename T>
    StringBuilder& operator<<(const T& rhs) {
        ss << rhs;
        return *this;
    }

    std::string str() const {
        return ss.str();
    }

private:
    std::stringstream ss;
};

} // namespace tcp
} // namespace modbus

#endif
