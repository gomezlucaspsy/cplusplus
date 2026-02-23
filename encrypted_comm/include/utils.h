#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class Utils {
public:
    // String utilities
    static std::string ToHex(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> FromHex(const std::string& hex);
    static std::string ToString(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> ToBytes(const std::string& str);
    
    // Validation
    static bool IsValidIP(const std::string& ip);
    static bool IsValidPort(int port);
    
    // Utility functions
    static std::string GetTimestamp();
    static std::string GetLocalIP();
    
    // File operations
    static bool FileExists(const std::string& filename);
    static std::string ReadFile(const std::string& filename);
    static bool WriteFile(const std::string& filename, const std::string& content);
};

#endif // UTILS_H
