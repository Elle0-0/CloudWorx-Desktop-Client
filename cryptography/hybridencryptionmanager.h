#ifndef HYBRIDENCRYPTIONMANAGER_H
#define HYBRIDENCRYPTIONMANAGER_H

#include <QString>
#include <QByteArray>
#include <QVector>

struct EncryptedPayload {
    QByteArray ciphertext;
    QByteArray nonce;
    QByteArray ephemeralPublicKey;
    QByteArray senderSigningPublicKey;
    QByteArray signature;
};

class HybridEncryptionManager {
public:
    static std::optional<EncryptedPayload> encryptAndSign(const QString& filePath,
                                           const QByteArray& recipientX25519PubKey,
                                           const QByteArray& senderX25519PrivKey,
                                           const QByteArray& senderEd25519PrivKey,
                                           const QByteArray& senderEd25519PubKey);

    static std::optional<QByteArray> decryptAndVerify(const EncryptedPayload& payload,
                                       const QByteArray& recipientX25519PrivKey,
                                       const QByteArray& senderX25519PubKey,
                                       const QByteArray& senderEd25519PubKey,
                                       bool& signatureValid);
};

#endif // HYBRIDENCRYPTIONMANAGER_H
