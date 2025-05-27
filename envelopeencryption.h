#ifndef ENVELOPEENCRYPTION_H
#define ENVELOPEENCRYPTION_H

#include <QByteArray>
#include <QString>

struct KeyDerivationResult {
    QByteArray salt;
    QByteArray key;      // 256-bit derived key
    quint32 timeCost;
    quint32 memoryCost;
};

struct WrappedKEKResult {
    QByteArray kek;              // Save temporarily for file encryption
    QByteArray wrappedKEK;       // Send to server
    QByteArray kekNonce;
    QByteArray salt;
    uint32_t timeCost;
    uint32_t memoryCost;
};

struct EnvelopeEncryptionResult {
    QByteArray ciphertext;
    QByteArray msgNonce;

    QByteArray wrappedDEK;
    QByteArray dekNonce;
};

struct EnvelopeDecryptionInput {
    // Argon2id parameters
    QByteArray salt;
    quint32 timeCost;
    quint32 memoryCost;

    // Encrypted keys
    QByteArray kekNonce;
    QByteArray wrappedKEK;
    QByteArray dekNonce;
    QByteArray wrappedDEK;

    // Encrypted message
    QByteArray msgNonce;
    QByteArray ciphertext;
};

class EnvelopeEncryption
{
public:
    // Key derivation functions
    static KeyDerivationResult deriveEncryptionKey(const QByteArray& password);
    static QByteArray deriveEncryptionKeyFromSalt(const QByteArray& password,
                                                  const QByteArray& salt,
                                                  quint32 timeCost,
                                                  quint32 memoryCost);

    // Encryption/Decryption functions
    static WrappedKEKResult generateAndWrapKEK(const QByteArray& password);
    static EnvelopeEncryptionResult encryptWithKEK(const QByteArray& plaintext, const QByteArray& kek);
    static QByteArray decrypt(const EnvelopeDecryptionInput& input, const QByteArray& password);

    // File operations
    static EnvelopeEncryptionResult encryptFile(const QString& filePath, const QByteArray& password);
    static bool decryptFile(const EnvelopeDecryptionInput& input, const QByteArray& password,
                            const QString& outputPath);
};

#endif // ENVELOPEENCRYPTION_H
