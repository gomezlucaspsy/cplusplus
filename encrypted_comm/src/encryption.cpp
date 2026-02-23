#include "encryption.h"
#include "config.h"
#include <cstring>
#include <random>
#include <algorithm>
#include <numeric>

std::vector<uint8_t> Encryption::key;
bool Encryption::initialized = false;

bool Encryption::InitializeKey(const std::string& password) {
    if (password.empty()) {
        return false;
    }
    
    // Generate key from password using a simple hash-based key derivation
    key.resize(Config::KEY_SIZE);
    uint32_t hash = SimpleHash(password);
    
    // Fill key with expanded hash
    for (size_t i = 0; i < Config::KEY_SIZE; i++) {
        hash = hash * 1103515245 + 12345; // Linear congruential generator
        key[i] = (hash >> ((i % 4) * 8)) & 0xFF;
    }
    
    initialized = true;
    return true;
}

std::vector<uint8_t> Encryption::Encrypt(const std::string& plaintext) {
    if (!initialized) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> result;
    std::vector<uint8_t> nonce = GenerateNonce();
    std::vector<uint8_t> data(plaintext.begin(), plaintext.end());
    
    // Add nonce to result
    result.insert(result.end(), nonce.begin(), nonce.end());
    
    // Encrypt data with rotating XOR
    int rotation = static_cast<int>(nonce[0]) % 8;
    RotateData(data.data(), data.size(), rotation);
    XorEncrypt(data.data(), data.size(), key);
    RotateData(data.data(), data.size(), -rotation);
    
    // Add encrypted data
    result.insert(result.end(), data.begin(), data.end());
    
    return result;
}

std::string Encryption::Decrypt(const std::vector<uint8_t>& ciphertext) {
    if (!initialized || ciphertext.size() <= Config::NONCE_SIZE) {
        return "";
    }
    
    // Extract nonce
    std::vector<uint8_t> nonce(ciphertext.begin(), ciphertext.begin() + Config::NONCE_SIZE);
    
    // Extract encrypted data
    std::vector<uint8_t> data(ciphertext.begin() + Config::NONCE_SIZE, ciphertext.end());
    
    // Decrypt with reverse operations
    int rotation = static_cast<int>(nonce[0]) % 8;
    RotateData(data.data(), data.size(), rotation);
    XorEncrypt(data.data(), data.size(), key);
    RotateData(data.data(), data.size(), -rotation);
    
    return std::string(data.begin(), data.end());
}

std::vector<uint8_t> Encryption::GenerateNonce() {
    std::vector<uint8_t> nonce(Config::NONCE_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    for (auto& byte : nonce) {
        byte = dis(gen);
    }
    
    return nonce;
}

void Encryption::XorEncrypt(uint8_t* data, size_t size, const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % key.size()];
    }
}

void Encryption::RotateData(uint8_t* data, size_t size, int rotation) {
    rotation = rotation % 8;
    if (rotation == 0) return;
    
    for (size_t i = 0; i < size; i++) {
        data[i] = (data[i] << rotation) | (data[i] >> (8 - rotation));
    }
}

uint32_t Encryption::SimpleHash(const std::string& input) {
    uint32_t hash = 0x811c9dc5; // FNV-1a offset basis
    
    for (unsigned char c : input) {
        hash ^= c;
        hash *= 16777619; // FNV-1a prime
    }
    
    return hash;
}
