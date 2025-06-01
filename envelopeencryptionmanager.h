#ifndef ENVELOPEENCRYPTIONMANAGER_H
#define ENVELOPEENCRYPTIONMANAGER_H

#include "envelopeencryption.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QUuid>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QUrl>

// JSON data structures matching your database schema
struct UserKEKData {

    QString username;
    QString authPassword;
    QString email;
    QString ivKEK;          // Base64 encoded nonce
    QString encryptedKEK;   // Base64 encoded wrapped KEK
    QString salt;           // Base64 encoded salt
    quint32 timeCost;
    quint32 memoryCost;


    // JSON conversion
    QJsonObject toJson() const;
    static UserKEKData fromJson(const QJsonObject& json);
};


struct FileData {
    QString fileName;
    QString fileType;
    int fileSize;
    QString ivFile;
    QString ivDEK;          // Base64 encoded nonce
    QString encryptedDEK;   // Base64 encoded wrapped DEK
    QByteArray encryptedFile; // Will be Base64 encoded for JSON
    QString assocDataFile;  // Additional metadata

    // JSON conversion
    QJsonObject toJson() const;
    static FileData fromJson(const QJsonObject& json);
};

class EnvelopeEncryptionManager
{

public:
    explicit EnvelopeEncryptionManager(const QString& serverUrl, QObject* parent = nullptr);

    // User setup - creates KEK and sends to server
    static bool setupUserEncryption(const QString& username, const QString& email, const QString& authPassword, const QByteArray& filePassword);

    // Password verification - fetches KEK from server and tries to unwrap
    static bool verifyUserFilePassword(const QString& userId, const QByteArray& password);

    // File operations
    static QString encryptAndStoreFile(const QString& userId, const QString& filePath,
                                const QByteArray& password);
    static QByteArray decryptAndSaveFile(const QString& fileId, const QString& userId,
                            const QByteArray& password, const QString& outputPath);

    // Helper functions
    static QByteArray getUserKEK(const QString& userId, const QByteArray& password);

private:

    // API operations
    static bool sendUserKEK(const UserKEKData& kekData);
    static bool sendFileData(const FileData& fileData);

    static UserKEKData fetchUserKEK(const QString& userId);
    static FileData fetchFileData(const QString& fileId);


    // Utility functions
    static QString encodeBase64(const QByteArray& data);
    static QByteArray decodeBase64(const QString& data);
    static QString generateUuid();
};

#endif // ENVELOPEENCRYPTIONMANAGER_H
