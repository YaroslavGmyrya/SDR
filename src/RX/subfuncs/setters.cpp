#include "../../../includes/Receiver.hpp"

void Receiver::set_L(const int _L) {
    if(_L <= 0) {throw std::invalid_argument("Invalid L (L > 0)");}

    L = _L;
}

void Receiver::set_filename(const std::string& _filename) {
    if(_filename.size() == 0) {throw std::invalid_argument("Invalid message (size must more then 0)");}

    filename = _filename;
}

void Receiver::set_demodulation_type(const std::string& _demodulation_type) {
    if(_demodulation_type != "BPSK" || _demodulation_type != "QPSK" || _demodulation_type != "QAM16") {throw std::invalid_argument("Invalid modulation type (BPSK, QPSK, QAM16)");}

    demodulation_type  = _demodulation_type;
}