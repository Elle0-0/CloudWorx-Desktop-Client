#include "envelopeencryptionmanager.h"

#include "envelopeencryption.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QUuid>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <sodium.h>
#include <QUrl>

bool EnvelopeEncryptionManager::setupUserEncryption(const QString& username, const QString& email, const QString& authPassword, const QByteArray& filePassword)
{
    try {
        // Generate KEK and wrap it with password-derived key
        WrappedKEKResult kekResult = EnvelopeEncryption::generateAndWrapKEK(filePassword);

        // Prepare data for server
        UserKEKData kekData;
        kekData.username = username;
        kekData.authPassword = authPassword;
        kekData.email = email;
        kekData.ivKEK = encodeBase64(kekResult.kekNonce);
        kekData.encryptedKEK = encodeBase64(kekResult.wrappedKEK);
        kekData.salt = encodeBase64(kekResult.salt);
        kekData.timeCost = kekResult.timeCost;
        kekData.memoryCost = kekResult.memoryCost;

        // Send to server - bool set to default for now
        bool success = true;
        //sendUserKEK(kekData);

        // Clear sensitive data
        sodium_memzero(const_cast<char*>(kekResult.kek.constData()), kekResult.kek.size());

        return success;

    } catch (const std::exception& e) {
        qDebug() << "Setup user encryption failed:" << e.what();
        return false;
    }
}

bool EnvelopeEncryptionManager::verifyUserPassword(const QString& userId, const QByteArray& password)
{
    try {
        // Fetch KEK data from server
        UserKEKData kekData = fetchUserKEK(userId);
        if (kekData.encryptedKEK.isEmpty()) {
            qDebug() << "No KEK found for user:" << userId;
            return false;
        }

        // Try to derive PDK and unwrap KEK
        QByteArray salt = decodeBase64(kekData.salt);
        QByteArray pdk = EnvelopeEncryption::deriveEncryptionKeyFromSalt(
            password, salt, kekData.timeCost, kekData.memoryCost);

        QByteArray wrappedKEK = decodeBase64(kekData.encryptedKEK);
        QByteArray kekNonce = decodeBase64(kekData.ivKEK);

        // Try to decrypt the wrapped KEK
        unsigned char kek[crypto_aead_aes256gcm_KEYBYTES];
        unsigned long long kekLen;

        int result = crypto_aead_aes256gcm_decrypt(
            kek, &kekLen,
            nullptr,
            reinterpret_cast<const unsigned char*>(wrappedKEK.constData()),
            wrappedKEK.size(),
            nullptr, 0,
            reinterpret_cast<const unsigned char*>(kekNonce.constData()),
            reinterpret_cast<const unsigned char*>(pdk.constData())
            );

        // Clean up
        sodium_memzero(pdk.data(), pdk.size());
        sodium_memzero(kek, sizeof kek);

        return result == 0;

    } catch (const std::exception& e) {
        qDebug() << "Password verification failed:" << e.what();
        return false;
    }
}

QString EnvelopeEncryptionManager::encryptAndStoreFile(const QString& userId, const QString& filePath,
                                                       const QByteArray& password)
{
    try {
        // Get user's KEK
        QByteArray userKEK = getUserKEK(userId, password);
        if (userKEK.isEmpty()) {
            qDebug() << "Failed to get user KEK";
            return QString();
        }

        // Read file
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Cannot open file:" << filePath;
            return QString();
        }
        QByteArray fileData = file.readAll();
        file.close();

        // Encrypt file with KEK
        EnvelopeEncryptionResult encResult = EnvelopeEncryption::encryptWithKEK(fileData, userKEK);



        // Prepare file data
        FileData fileDataObj;
        fileDataObj.fileName = QFileInfo(filePath).fileName();
        fileDataObj.ivFile = encodeBase64(encResult.msgNonce);
        fileDataObj.encryptedFile = encResult.ciphertext;
        fileDataObj.fileType = QFileInfo(filePath).suffix();
        fileDataObj.fileSize = fileData.size();
        fileDataObj.ivFile = encodeBase64(encResult.msgNonce);
        fileDataObj.ivDEK = encodeBase64(encResult.dekNonce);
        fileDataObj.encryptedDEK = encodeBase64(encResult.wrappedDEK);

        //uncomment once implemented
        bool fileSuccess = true;
        //bool fileSuccess = sendFileData(fileDataObj);

        // Clear sensitive data
        sodium_memzero(userKEK.data(), userKEK.size());

        if (fileSuccess) {
            return filePath;
        } else {
            qDebug() << "Failed to store file data on server";
            return QString();
        }

    } catch (const std::exception& e) {
        qDebug() << "File encryption failed:" << e.what();
        return QString();
    }
}

QByteArray EnvelopeEncryptionManager::decryptAndSaveFile(const QString& fileId, const QString& userId,
                                                   const QByteArray& password, const QString& outputPath)
{
    try {
        // Fetch all required data from server
        UserKEKData kekData = fetchUserKEK(userId);
        FileData fileData = fetchFileData(fileId);

        if (kekData.encryptedKEK.isEmpty() || fileData.encryptedFile.isEmpty()) {
            qDebug() << "Missing data from server";
            return QByteArray();
        }

        // Build decryption input
        EnvelopeDecryptionInput input;
        input.salt = decodeBase64(kekData.salt);
        input.timeCost = kekData.timeCost;
        input.memoryCost = kekData.memoryCost;
        input.kekNonce = decodeBase64(kekData.ivKEK);
        input.wrappedKEK = decodeBase64(kekData.encryptedKEK);
        input.dekNonce = decodeBase64(fileData.ivDEK);
        input.wrappedDEK = decodeBase64(fileData.encryptedDEK);
        input.msgNonce = decodeBase64(fileData.ivFile);
        input.ciphertext = fileData.encryptedFile;

        // Decrypt and save
        return EnvelopeEncryption::decrypt(input, password);

    } catch (const std::exception& e) {
        qDebug() << "File decryption failed:" << e.what();
        return QByteArray();
    }
}

QByteArray EnvelopeEncryptionManager::getUserKEK(const QString& userId, const QByteArray& password)
{
    try {
        // Fetch KEK data from server
        UserKEKData kekData = fetchUserKEK(userId);
        if (kekData.encryptedKEK.isEmpty()) {
            return QByteArray();
        }

        // Derive PDK
        QByteArray salt = decodeBase64(kekData.salt);
        QByteArray pdk = EnvelopeEncryption::deriveEncryptionKeyFromSalt(
            password, salt, kekData.timeCost, kekData.memoryCost);

        // Unwrap KEK
        QByteArray wrappedKEK = decodeBase64(kekData.encryptedKEK);
        QByteArray kekNonce = decodeBase64(kekData.ivKEK);

        unsigned char kek[crypto_aead_aes256gcm_KEYBYTES];
        unsigned long long kekLen;

        if (crypto_aead_aes256gcm_decrypt(
                kek, &kekLen,
                nullptr,
                reinterpret_cast<const unsigned char*>(wrappedKEK.constData()),
                wrappedKEK.size(),
                nullptr, 0,
                reinterpret_cast<const unsigned char*>(kekNonce.constData()),
                reinterpret_cast<const unsigned char*>(pdk.constData())) != 0) {
            sodium_memzero(pdk.data(), pdk.size());
            return QByteArray();
        }

        QByteArray kekBytes(reinterpret_cast<char*>(kek), sizeof kek);

        // Clean up
        sodium_memzero(pdk.data(), pdk.size());
        sodium_memzero(kek, sizeof kek);

        return kekBytes;

    } catch (...) {
        return QByteArray();
    }
}

// API operations
bool EnvelopeEncryptionManager::sendUserKEK(const UserKEKData& kekData)
{
    // QJsonObject response = makeRequest("/api/user/kek", "POST", kekData.toJson());
    // return response.value("success").toBool();
}


bool EnvelopeEncryptionManager::sendFileData(const FileData& fileData)
{
    // QJsonObject response = makeRequest("/api/file/data", "POST", fileData.toJson());
    // return response.value("success").toBool();
}

UserKEKData EnvelopeEncryptionManager::fetchUserKEK(const QString& userId)
{
    // QJsonObject response = makeRequest(QString("/api/user/kek/%1").arg(userId), "GET");
    // if (response.value("success").toBool()) {
    //     return UserKEKData::fromJson(response.value("data").toObject());
    // }
    // return UserKEKData(); // Empty if failed
}


FileData EnvelopeEncryptionManager::fetchFileData(const QString& fileId)
{
    // QJsonObject response = makeRequest(QString("/api/file/data/%1").arg(fileId), "GET");
    // if (response.value("success").toBool()) {
    //     return FileData::fromJson(response.value("data").toObject());
    // }
    // return FileData(); // Empty if failed
}

// JSON conversion implementations
QJsonObject UserKEKData::toJson() const
{
    QJsonObject obj;
    obj["username"] = username;
    obj["auth_password"] = authPassword;
    obj["email"] = email;
    obj["encrypted_kek"] = encryptedKEK;
    obj["salt"] = salt;
    obj["time_cost"] = static_cast<int>(timeCost);
    obj["memory_cost"] = static_cast<int>(memoryCost);
    return obj;
}

UserKEKData UserKEKData::fromJson(const QJsonObject& json)
{
    UserKEKData data;
    data.username = json["username"].toString();
    data.authPassword = json["auth_password"].toString();
    data.email = json["email"].toString();
    data.ivKEK = json["iv_kek"].toString();
    data.encryptedKEK = json["encrypted_kek"].toString();
    data.salt = json["salt"].toString();
    data.timeCost = json["time_cost"].toInt();
    data.memoryCost = json["memory_cost"].toInt();
    return data;
}


QJsonObject FileData::toJson() const
{
    QJsonObject obj;
    obj["fileName"] = fileName;
    obj["file_type"] = fileType;
    obj["file_size"] = fileSize;
    obj["iv_file"] = ivFile;
    obj["iv_dek"] = ivDEK;
    obj["encrypted_file"] = QString::fromUtf8(encryptedFile.toBase64()); // Base64 encode binary data
    obj["encrypted_dek"] = encryptedDEK;
    return obj;
}

FileData FileData::fromJson(const QJsonObject& json)
{
    FileData data;
    data.fileName = json["fileName"].toString(); // Note: toJson uses "fileName", not "file_name"
    data.fileType = json["file_type"].toString();
    data.fileSize = json["file_size"].toInt();
    data.ivFile = json["iv_file"].toString();
    data.ivDEK = json["iv_dek"].toString();
    data.encryptedFile = QByteArray::fromBase64(json["encrypted_file"].toString().toUtf8());
    data.encryptedDEK = json["encrypted_dek"].toString();
    return data;
}


// Utility functions
QString EnvelopeEncryptionManager::encodeBase64(const QByteArray& data)
{
    return data.toBase64();
}

QByteArray EnvelopeEncryptionManager::decodeBase64(const QString& data)
{
    return QByteArray::fromBase64(data.toUtf8());
}
