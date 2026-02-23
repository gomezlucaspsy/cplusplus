#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <vector>
#include <cstdint>

class Encryption {
public:
    // Initialize encryption with a password
    static bool InitializeKey(const std::string& password);
    
    // Encrypt plaintext
    static std::vector<uint8_t> Encrypt(const std::string& plaintext);
    
    // Decrypt ciphertext
    static std::string Decrypt(const std::vector<uint8_t>& ciphertext);
    
    // Generate a random nonce
    static std::vector<uint8_t> GenerateNonce();
    
private:
    static std::vector<uint8_t> key;
    static bool initialized;
    
    // Simple but effective encryption using XOR + rotation
    static void XorEncrypt(uint8_t* data, size_t size, const std::vector<uint8_t>& key);
    static void RotateData(uint8_t* data, size_t size, int rotation);
    static uint32_t SimpleHash(const std::string& input);
};

#endif // ENCRYPTION_H
