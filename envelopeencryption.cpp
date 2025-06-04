#include "envelopeencryption.h"
#include <sodium.h>
#include <stdexcept>
#include <QFile>
#include <QDebug>

//HELPER FUNCTIONS WOOO
QByteArray sha256First12Bytes(const QByteArray& data) {
    unsigned char hash[crypto_hash_sha256_BYTES];
    crypto_hash_sha256(hash, reinterpret_cast<const unsigned char*>(data.constData()), data.size());
    return QByteArray(reinterpret_cast<char*>(hash), 12); // First 12 bytes
}

QByteArray generateRandomBytes(int length) {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed.");
    }

    QByteArray bytes(length, 0);
    randombytes_buf(bytes.data(), bytes.size());
    return bytes;
}



KeyDerivationResult EnvelopeEncryption::deriveEncryptionKey(const QByteArray& password) {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium init failed!");
    }

    KeyDerivationResult result;

    // generate random salt
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof salt);
    result.salt = QByteArray(reinterpret_cast<char*>(salt), sizeof salt);

    // Set parameters
    result.timeCost = 3;
    result.memoryCost = 12288;

    // Derive 256-bit key
    unsigned char key[32];  // 256 bits
    if (crypto_pwhash(key, sizeof key,
                      password.constData(), password.length(),
                      salt,
                      result.timeCost, result.memoryCost,
                      crypto_pwhash_ALG_ARGON2ID13) != 0) {
        throw std::runtime_error("Argon2id key derivation failed - out of memory");
    }

    result.key = QByteArray(reinterpret_cast<char*>(key), sizeof key);

    // Clear sensitive data
    sodium_memzero(key, sizeof key);

    return result;
}

QByteArray EnvelopeEncryption::deriveEncryptionKeyFromSalt(const QByteArray& password,
                                                           const QByteArray& salt,
                                                           quint32 timeCost,
                                                           quint32 memoryCost) {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium init failed!");
    }

    if (salt.size() != crypto_pwhash_SALTBYTES) {
        throw std::runtime_error("Invalid salt size");
    }

    unsigned char key[32];  // 256 bits
    if (crypto_pwhash(key, sizeof key,
                      password.constData(), password.length(),
                      reinterpret_cast<const unsigned char*>(salt.constData()),
                      timeCost, memoryCost,
                      crypto_pwhash_ALG_ARGON2ID13) != 0) {
        throw std::runtime_error("Argon2id key derivation failed - out of memory");
    }

    QByteArray result(reinterpret_cast<char*>(key), sizeof key);
    sodium_memzero(key, sizeof key);

    return result;
}



WrappedKEKResult EnvelopeEncryption::generateAndWrapKEK(const QString username, const QByteArray& password) {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed.");
    }

    // Step 1: Derive PDK
    KeyDerivationResult pdkResult = deriveEncryptionKey(password);

    // Step 2: Generate KEK
    unsigned char kek[crypto_aead_aes256gcm_KEYBYTES];
    randombytes_buf(kek, sizeof kek);


    QByteArray kekNonce = sha256First12Bytes(username.toUtf8() + generateRandomBytes(12));


    // // Step 3: Wrap KEK with PDK
    // QByteArray kekNonce(crypto_aead_aes256gcm_NPUBBYTES, 0);
    // randombytes_buf(kekNonce.data(), kekNonce.size());

    QByteArray wrappedKEK(sizeof kek + crypto_aead_aes256gcm_ABYTES, 0);
    unsigned long long wrappedKEKLen;

    if (crypto_aead_aes256gcm_encrypt(
            reinterpret_cast<unsigned char*>(wrappedKEK.data()), &wrappedKEKLen,
            kek, sizeof kek,
            nullptr, 0,
            nullptr,
            reinterpret_cast<const unsigned char*>(kekNonce.constData()),
            reinterpret_cast<const unsigned char*>(pdkResult.key.constData())) != 0) {
        throw std::runtime_error("KEK wrapping failed.");
    }

    return {
        QByteArray(reinterpret_cast<char*>(kek), sizeof kek), // Return KEK for later use
        wrappedKEK.left(wrappedKEKLen),
        kekNonce,
        pdkResult.salt,
        pdkResult.timeCost,
        pdkResult.memoryCost
    };
}


EnvelopeEncryptionResult EnvelopeEncryption::encryptWithKEK(const QString userid, const QString fileName,
                                                            const QByteArray& plaintext, const QByteArray& kek) {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed.");
    }

    // Step 1: Generate DEK
    unsigned char dek[crypto_aead_aes256gcm_KEYBYTES];
    randombytes_buf(dek, sizeof dek);

    // Step 2: Encrypt plaintext with DEK
    QByteArray msgNonce(crypto_aead_aes256gcm_NPUBBYTES, 0);
    randombytes_buf(msgNonce.data(), msgNonce.size());

    QByteArray ciphertext(plaintext.size() + crypto_aead_aes256gcm_ABYTES, 0);
    unsigned long long ciphertextLen;

    if (crypto_aead_aes256gcm_encrypt(
            reinterpret_cast<unsigned char*>(ciphertext.data()), &ciphertextLen,
            reinterpret_cast<const unsigned char*>(plaintext.constData()), plaintext.size(),
            nullptr, 0,
            nullptr,
            reinterpret_cast<const unsigned char*>(msgNonce.constData()),
            dek) != 0) {
        throw std::runtime_error("File encryption failed.");
    }

    // // Step 3: Wrap DEK with KEK
    // QByteArray dekNonce(crypto_aead_aes256gcm_NPUBBYTES, 0);
    // randombytes_buf(dekNonce.data(), dekNonce.size());

    QByteArray dekNonce = sha256First12Bytes(userid.toUtf8() + fileName.toUtf8() + generateRandomBytes(12));

    QByteArray wrappedDEK(sizeof dek + crypto_aead_aes256gcm_ABYTES, 0);
    unsigned long long wrappedDEKLen;

    if (crypto_aead_aes256gcm_encrypt(
            reinterpret_cast<unsigned char*>(wrappedDEK.data()), &wrappedDEKLen,
            dek, sizeof dek,
            nullptr, 0,
            nullptr,
            reinterpret_cast<const unsigned char*>(dekNonce.constData()),
            reinterpret_cast<const unsigned char*>(kek.constData())
            ) != 0) {
        throw std::runtime_error("DEK wrapping failed.");
    }

    sodium_memzero(dek, sizeof dek);

    return {
        ciphertext.left(ciphertextLen),
        msgNonce,
        wrappedDEK.left(wrappedDEKLen),
        dekNonce
    };
}




QByteArray EnvelopeEncryption::decrypt(const EnvelopeDecryptionInput& input, const QByteArray& password)
{
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed.");
    }

    // Step 1: Derive PDK using stored salt + KDF params
    QByteArray pdk = deriveEncryptionKeyFromSalt(password, input.salt, input.timeCost, input.memoryCost);

    // Step 2: Unwrap KEK with PDK
    unsigned char kek[crypto_aead_aes256gcm_KEYBYTES];
    unsigned long long kekLen;

    if (input.kekNonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        throw std::runtime_error("Invalid KEK nonce size.");
    }
    if (input.wrappedKEK.size() <= crypto_aead_aes256gcm_ABYTES) {
        throw std::runtime_error("Invalid wrapped KEK size.");
    }


    if (crypto_aead_aes256gcm_decrypt(
            kek, &kekLen,
            nullptr,
            reinterpret_cast<const unsigned char*>(input.wrappedKEK.constData()),
            input.wrappedKEK.size(),
            nullptr, 0,
            reinterpret_cast<const unsigned char*>(input.kekNonce.constData()),
            reinterpret_cast<const unsigned char*>(pdk.constData())) != 0) {
        sodium_memzero(pdk.data(), pdk.size());
        throw std::runtime_error("KEK decryption failed - wrong password?");
    }

    // Step 3: Unwrap DEK with KEK
    unsigned char dek[crypto_aead_aes256gcm_KEYBYTES];
    unsigned long long dekLen;

    if (input.dekNonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        throw std::runtime_error("Invalid DEK nonce size.");
    }
    if (input.wrappedDEK.size() <= crypto_aead_aes256gcm_ABYTES) {
        throw std::runtime_error("Invalid wrapped DEK size.");
    }


    if (crypto_aead_aes256gcm_decrypt(
            dek, &dekLen,
            nullptr,
            reinterpret_cast<const unsigned char*>(input.wrappedDEK.constData()),
            input.wrappedDEK.size(),
            nullptr, 0,
            reinterpret_cast<const unsigned char*>(input.dekNonce.constData()),
            kek) != 0) {
        sodium_memzero(pdk.data(), pdk.size());
        sodium_memzero(kek, sizeof kek);
        throw std::runtime_error("DEK decryption failed");
    }

    // Step 4: Decrypt message with DEK
    QByteArray plaintext(input.ciphertext.size(), 0);
    unsigned long long plaintextLen;

    if (input.msgNonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        throw std::runtime_error("Invalid message nonce size.");
    }
    if (input.ciphertext.size() <= crypto_aead_aes256gcm_ABYTES) {
        throw std::runtime_error("Invalid ciphertext size.");
    }


    if (crypto_aead_aes256gcm_decrypt(
            reinterpret_cast<unsigned char*>(plaintext.data()), &plaintextLen,
            nullptr,
            reinterpret_cast<const unsigned char*>(input.ciphertext.constData()),
            input.ciphertext.size(),
            nullptr, 0,
            reinterpret_cast<const unsigned char*>(input.msgNonce.constData()),
            dek) != 0) {
        sodium_memzero(pdk.data(), pdk.size());
        sodium_memzero(kek, sizeof kek);
        sodium_memzero(dek, sizeof dek);
        throw std::runtime_error("Message decryption failed");
    }

    // Cleanup sensitive data
    sodium_memzero(pdk.data(), pdk.size());
    sodium_memzero(kek, sizeof kek);
    sodium_memzero(dek, sizeof dek);

    return plaintext.left(plaintextLen);
}


