#include "utils.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#endif

std::string Utils::ToHex(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    for (const auto& byte : data) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

std::vector<uint8_t> Utils::FromHex(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteStr = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

std::string Utils::ToString(const std::vector<uint8_t>& data) {
    return std::string(data.begin(), data.end());
}

std::vector<uint8_t> Utils::ToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

bool Utils::IsValidIP(const std::string& ip) {
    struct sockaddr_in sa;
#ifdef _WIN32
    return InetPtonA(AF_INET, ip.c_str(), &sa.sin_addr) == 1;
#else
    return inet_aton(ip.c_str(), &sa.sin_addr) != 0;
#endif
}

bool Utils::IsValidPort(int port) {
    return port > 0 && port < 65536;
}

std::string Utils::GetTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%H:%M:%S");
    return ss.str();
}

std::string Utils::GetLocalIP() {
#ifdef _WIN32
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    struct hostent *he = gethostbyname(hostname);
    if (he != nullptr) {
        struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
        if (addr_list[0] != nullptr) {
            return inet_ntoa(*addr_list[0]);
        }
    }
#else
    struct ifaddrs *ifaddr = nullptr;
    if (getifaddrs(&ifaddr) == -1) {
        return "127.0.0.1";
    }
    
    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            std::string ip = inet_ntoa(addr->sin_addr);
            
            if (ip != "127.0.0.1") {
                freeifaddrs(ifaddr);
                return ip;
            }
        }
    }
    
    freeifaddrs(ifaddr);
#endif
    
    return "127.0.0.1";
}

bool Utils::FileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string Utils::ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Utils::WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}
