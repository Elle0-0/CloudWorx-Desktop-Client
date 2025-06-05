#include "hybridencryptionmanager.h"
#include <sodium.h>
#include <QDebug>
#include <QFile>

// Helper: Concatenate two byte arrays
QByteArray concat(const QByteArray& a, const QByteArray& b) {
    QByteArray result;
    result.reserve(a.size() + b.size());
    result.append(a);
    result.append(b);
    return result;
}

std::optional<EncryptedPayload> HybridEncryptionManager::encryptAndSign(const QString& filePath,
                                                         const QByteArray& recipientX25519PubKey,
                                                         const QByteArray& senderX25519PrivKey,
                                                         const QByteArray& senderEd25519PrivKey,
                                                         const QByteArray& senderEd25519PubKey)
{
    EncryptedPayload payload;

    // Step 1: Load file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for encryption:" << filePath;
        return std::nullopt;
    }
    QByteArray plaintext = file.readAll();
    file.close();

    // 1. Generate ephemeral X25519 keypair
    unsigned char ephPriv[crypto_kx_SECRETKEYBYTES];
    unsigned char ephPub[crypto_kx_PUBLICKEYBYTES];
    crypto_kx_keypair(ephPub, ephPriv);
    payload.ephemeralPublicKey = QByteArray(reinterpret_cast<char*>(ephPub), crypto_kx_PUBLICKEYBYTES);

    // 2. Derive shared secrets
    unsigned char shared1[crypto_scalarmult_BYTES];
    unsigned char shared2[crypto_scalarmult_BYTES];

    if (crypto_scalarmult(shared1, ephPriv,
                          reinterpret_cast<const unsigned char*>(recipientX25519PubKey.constData())) != 0) {
        qWarning() << "Failed to compute shared1 (ephemeral ECDH)";
        return std::nullopt;  // or handle the error appropriately
    }

    if (crypto_scalarmult(shared2,
                          reinterpret_cast<const unsigned char*>(senderX25519PrivKey.constData()),
                          reinterpret_cast<const unsigned char*>(recipientX25519PubKey.constData())) != 0) {
        qWarning() << "Failed to compute shared2 (sender ECDH)";
        return std::nullopt;  // or handle the error appropriately
    }

    // 3. Hash to get final key
    QByteArray combined = concat(QByteArray::fromRawData(reinterpret_cast<char*>(shared1), sizeof(shared1)),
                                 QByteArray::fromRawData(reinterpret_cast<char*>(shared2), sizeof(shared2)));

    unsigned char aesKey[crypto_aead_aes256gcm_KEYBYTES];
    crypto_generichash(aesKey, sizeof(aesKey),
                       reinterpret_cast<const unsigned char*>(combined.constData()), combined.size(),
                       nullptr, 0);

    // 4. Encrypt with AES-GCM
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    randombytes_buf(nonce, sizeof(nonce));
    payload.nonce = QByteArray(reinterpret_cast<char*>(nonce), sizeof(nonce));

    QByteArray ciphertext;
    ciphertext.resize(plaintext.size() + crypto_aead_aes256gcm_ABYTES);

    unsigned long long ciphertextLen;
    crypto_aead_aes256gcm_encrypt(reinterpret_cast<unsigned char*>(ciphertext.data()), &ciphertextLen,
                                  reinterpret_cast<const unsigned char*>(plaintext.constData()), plaintext.size(),
                                  nullptr, 0, nullptr, nonce, aesKey);

    ciphertext.resize(ciphertextLen);
    payload.ciphertext = ciphertext;

    // 5. Sign (ek_pub || nonce || ciphertext) with Ed25519
    QByteArray dataToSign = concat(payload.ephemeralPublicKey, concat(payload.nonce, payload.ciphertext));
    unsigned char signature[crypto_sign_BYTES];

    crypto_sign_detached(signature, nullptr,
                         reinterpret_cast<const unsigned char*>(dataToSign.constData()), dataToSign.size(),
                         reinterpret_cast<const unsigned char*>(senderEd25519PrivKey.constData()));

    payload.signature = QByteArray(reinterpret_cast<char*>(signature), crypto_sign_BYTES);
    payload.senderSigningPublicKey = senderEd25519PubKey;

    return payload;
}

std::optional<QByteArray> HybridEncryptionManager::decryptAndVerify(const EncryptedPayload& payload,
                                                     const QByteArray& recipientX25519PrivKey,
                                                     const QByteArray& senderX25519PubKey,
                                                     const QByteArray& senderEd25519PubKey,
                                                     bool& signatureValid)
{
    // 1. Derive shared secrets
    unsigned char shared1[crypto_scalarmult_BYTES];
    unsigned char shared2[crypto_scalarmult_BYTES];


    if (crypto_scalarmult(shared1,
                          reinterpret_cast<const unsigned char*>(recipientX25519PrivKey.constData()),
                          reinterpret_cast<const unsigned char*>(payload.ephemeralPublicKey.constData())) != 0)
    {
        return std::nullopt;
    }

    if (crypto_scalarmult(shared2,
                          reinterpret_cast<const unsigned char*>(recipientX25519PrivKey.constData()),
                          reinterpret_cast<const unsigned char*>(senderX25519PubKey.constData())) != 0)
    {
        return std::nullopt;
    }
    // 2. Hash for key
    QByteArray combined = concat(QByteArray::fromRawData(reinterpret_cast<char*>(shared1), sizeof(shared1)),
                                 QByteArray::fromRawData(reinterpret_cast<char*>(shared2), sizeof(shared2)));

    unsigned char aesKey[crypto_aead_aes256gcm_KEYBYTES];
    crypto_generichash(aesKey, sizeof(aesKey),
                       reinterpret_cast<const unsigned char*>(combined.constData()), combined.size(),
                       nullptr, 0);

    // 3. Verify signature
    QByteArray signedData = concat(payload.ephemeralPublicKey, concat(payload.nonce, payload.ciphertext));

    int sigCheck = crypto_sign_verify_detached(reinterpret_cast<const unsigned char*>(payload.signature.constData()),
                                               reinterpret_cast<const unsigned char*>(signedData.constData()), signedData.size(),
                                               reinterpret_cast<const unsigned char*>(senderEd25519PubKey.constData()));

    signatureValid = (sigCheck == 0);

    if (!signatureValid) {
        qWarning() << "Invalid signature!";
        return {};
    }

    // 4. Decrypt
    QByteArray decrypted;
    decrypted.resize(payload.ciphertext.size() - crypto_aead_aes256gcm_ABYTES);
    unsigned long long decryptedLen;

    if (crypto_aead_aes256gcm_decrypt(reinterpret_cast<unsigned char*>(decrypted.data()), &decryptedLen,
                                      nullptr,
                                      reinterpret_cast<const unsigned char*>(payload.ciphertext.constData()), payload.ciphertext.size(),
                                      nullptr, 0,
                                      reinterpret_cast<const unsigned char*>(payload.nonce.constData()), aesKey) != 0) {
        qWarning() << "Decryption failed";
        return {};
    }

    decrypted.resize(decryptedLen);
    return decrypted;
}

