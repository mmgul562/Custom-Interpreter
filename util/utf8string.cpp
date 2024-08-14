#include "utf8string.h"


size_t getStrLen(const std::string &s) {
    size_t len = 0;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        if ((c & 0x80) == 0x00) {
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            i += 4;
        } else {
            throw std::runtime_error("Invalid UTF-8 encoding");
        }
        len++;
    }
    return len;
}


std::string getStrChar(const std::string &s, size_t index) {
    size_t i = 0;
    for (size_t count = 0; i < s.size() && count < index; count++) {
        unsigned char c = s[i];
        if ((c & 0x80) == 0x00) {
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            i += 4;
        } else {
            throw std::runtime_error("Invalid UTF-8 encoding");
        }
    }
    unsigned char c = s[i];
    size_t char_len = 1;
    if ((c & 0x80) == 0x00) {
        char_len = 1;
    } else if ((c & 0xE0) == 0xC0) {
        char_len = 2;
    } else if ((c & 0xF0) == 0xE0) {
        char_len = 3;
    } else if ((c & 0xF8) == 0xF0) {
        char_len = 4;
    }
    return s.substr(i, char_len);
}
