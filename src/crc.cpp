#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cstring>

class CRC {
private:
    std::string polynomial;
    int degree;
    std::vector<bool> divisor;

public:
    CRC(const std::string& poly) : polynomial(poly) {
        degree = poly.length() - 1;
        divisor.resize(degree + 1);
        for (int i = 0; i <= degree; i++) {
            divisor[i] = (poly[i] == '1');
        }
    }

    std::string calculate_crc(const std::string& data) {
        std::vector<bool> dividend;
        
        // Convert data to binary
        for (char c : data) {
            std::bitset<8> bits(c);
            for (int i = 7; i >= 0; i--) {
                dividend.push_back(bits[i]);
            }
        }
        
        // Append zeros for CRC
        for (int i = 0; i < degree; i++) {
            dividend.push_back(false);
        }
        
        // Perform division
        std::vector<bool> remainder = divide(dividend);
        
        // Convert remainder to string
        std::string crc;
        for (int i = 0; i < degree; i++) {
            crc += (remainder[i] ? '1' : '0');
        }
        
        return crc;
    }

    bool verify_crc(const std::string& data, const std::string& received_crc) {
        std::vector<bool> dividend;
        
        // Convert data to binary
        for (char c : data) {
            std::bitset<8> bits(c);
            for (int i = 7; i >= 0; i--) {
                dividend.push_back(bits[i]);
            }
        }
        
        // Append received CRC
        for (char c : received_crc) {
            dividend.push_back(c == '1');
        }
        
        // Perform division
        std::vector<bool> remainder = divide(dividend);
        
        // Check if remainder is zero
        for (bool bit : remainder) {
            if (bit) return false;
        }
        return true;
    }

    std::string add_crc_to_data(const std::string& data) {
        std::string crc = calculate_crc(data);
        return data + crc;
    }

    std::string remove_crc_from_data(const std::string& data_with_crc) {
        if (data_with_crc.length() < degree) {
            return "";
        }
        return data_with_crc.substr(0, data_with_crc.length() - degree);
    }

private:
    std::vector<bool> divide(const std::vector<bool>& dividend) {
        std::vector<bool> remainder = dividend;
        
        for (int i = 0; i <= dividend.size() - degree - 1; i++) {
            if (remainder[i]) {
                for (int j = 0; j <= degree; j++) {
                    remainder[i + j] = remainder[i + j] ^ divisor[j];
                }
            }
        }
        
        // Return the remainder (last degree bits)
        std::vector<bool> result;
        for (int i = dividend.size() - degree; i < dividend.size(); i++) {
            result.push_back(remainder[i]);
        }
        
        return result;
    }
}; 