#include "../../../includes/Transmitter.hpp"

void Transmitter::set_L(const int _L) {
    if(_L <= 0) {throw std::invalid_argument("Invalid L (L > 0)");}

    L = _L;
}

void Transmitter::set_message(const std::string& _message) {
    if(_message.size() == 0) {throw std::invalid_argument("Invalid message (size must more then 0)");}

    message = _message;
}

void Transmitter::set_filename(const std::string& _filename) {
    if(_filename.size() == 0) {throw std::invalid_argument("Invalid filename (size must more then 0)");}

    filename = _filename;
}

void Transmitter::set_modulation_type(const std::string& _modulation_type) {
    if(_modulation_type != "BPSK" || _modulation_type != "QPSK" || _modulation_type != "QAM16") {throw std::invalid_argument("Invalid modulation type (BPSK, QPSK, QAM16)");}

    modulation_type  = _modulation_type;
}