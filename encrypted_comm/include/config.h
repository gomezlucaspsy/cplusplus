#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    constexpr const char* APP_NAME = "ENCRYPTED COMM v1.0";
    constexpr const char* APP_VERSION = "1.0.0";
    constexpr int DEFAULT_PORT = 7777;
    constexpr int MAX_MESSAGE_SIZE = 65536;
    constexpr int KEY_SIZE = 32; // 256-bit key
    constexpr int NONCE_SIZE = 24; // for ChaCha20-Poly1305
    
    // MS-DOS Colors
    namespace Colors {
        constexpr const char* RESET = "\033[0m";
        constexpr const char* BRIGHT_GREEN = "\033[92m";
        constexpr const char* BRIGHT_CYAN = "\033[96m";
        constexpr const char* BRIGHT_YELLOW = "\033[93m";
        constexpr const char* BRIGHT_RED = "\033[91m";
        constexpr const char* BRIGHT_WHITE = "\033[97m";
        constexpr const char* DARK_GREEN = "\033[32m";
    }
}

#endif // CONFIG_H
