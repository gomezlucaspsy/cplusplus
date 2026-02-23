/**
 * HolyC Version - TempleOS Encrypted Communications
 * Encryption Module for TempleOS
 */

#define KEY_SIZE 32
#define NONCE_SIZE 24
#define MAX_MESSAGE_SIZE 65536

U8 *encryption_key = NULL;
BOOL encryption_initialized = FALSE;

/**
 * Initialize encryption with password
 */
BOOL EncryptionInitialize(U8 *password) {
    I64 len = StrLen(password);
    if (len == 0) return FALSE;

    // Allocate key buffer
    encryption_key = MAlloc(KEY_SIZE);
    if (!encryption_key) return FALSE;

    // Generate key from password using simple hash-based KDF
    U32 hash = 0x811c9dc5;  // FNV-1a offset basis
    I64 i;
    
    for (i = 0; i < len; i++) {
        hash ^= password[i];
        hash *= 16777619;
    }

    // Fill key with expanded hash
    for (i = 0; i < KEY_SIZE; i++) {
        hash = hash * 1103515245 + 12345;
        encryption_key[i] = (hash >> ((i % 4) * 8)) & 0xFF;
    }

    encryption_initialized = TRUE;
    return TRUE;
}

/**
 * XOR encryption
 */
VOID EncryptionXor(U8 *data, I64 size) {
    I64 i;
    if (!encryption_initialized || !data) return;

    for (i = 0; i < size; i++) {
        data[i] ^= encryption_key[i % KEY_SIZE];
    }
}

/**
 * Rotate data by specified amount
 */
VOID EncryptionRotate(U8 *data, I64 size, I32 rotation) {
    I64 i;
    rotation = rotation % 8;
    if (rotation == 0) return;

    for (i = 0; i < size; i++) {
        data[i] = (data[i] << rotation) | (data[i] >> (8 - rotation));
    }
}

/**
 * Encrypt plaintext
 */
U8 *EncryptionEncrypt(U8 *plaintext, I64 *out_size) {
    if (!encryption_initialized || !plaintext) return NULL;

    I64 plaintext_len = StrLen(plaintext);
    I64 total_size = NONCE_SIZE + plaintext_len;
    
    U8 *result = MAlloc(total_size);
    if (!result) return NULL;

    // Generate random nonce (simplified for TempleOS - using system tick)
    I64 i;
    U32 seed = GetTick();
    for (i = 0; i < NONCE_SIZE; i++) {
        seed = seed * 1103515245 + 12345;
        result[i] = (seed >> ((i % 4) * 8)) & 0xFF;
    }

    // Copy plaintext to result buffer
    MemCpy(result + NONCE_SIZE, plaintext, plaintext_len);

    // Encrypt with rotating XOR
    I32 rotation = result[0] % 8;
    EncryptionRotate(result + NONCE_SIZE, plaintext_len, rotation);
    EncryptionXor(result + NONCE_SIZE, plaintext_len);
    EncryptionRotate(result + NONCE_SIZE, plaintext_len, -rotation);

    *out_size = total_size;
    return result;
}

/**
 * Decrypt ciphertext
 */
U8 *EncryptionDecrypt(U8 *ciphertext, I64 cipher_size) {
    if (!encryption_initialized || !ciphertext || cipher_size <= NONCE_SIZE)
        return NULL;

    I64 data_size = cipher_size - NONCE_SIZE;
    U8 *result = MAlloc(data_size + 1);
    if (!result) return NULL;

    // Copy encrypted data
    MemCpy(result, ciphertext + NONCE_SIZE, data_size);

    // Decrypt with reverse operations
    I32 rotation = ciphertext[0] % 8;
    EncryptionRotate(result, data_size, rotation);
    EncryptionXor(result, data_size);
    EncryptionRotate(result, data_size, -rotation);

    // Null-terminate for string use
    result[data_size] = 0;
    return result;
}

/**
 * Cleanup encryption resources
 */
VOID EncryptionCleanup() {
    if (encryption_key) {
        Free(encryption_key);
        encryption_key = NULL;
    }
    encryption_initialized = FALSE;
}
