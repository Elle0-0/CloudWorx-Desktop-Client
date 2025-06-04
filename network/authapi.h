#ifndef AUTHAPI_H
#define AUTHAPI_H

#include "../models/UserRegisterModel.h"
#include "../models/UserLoginModel.h"

#include <QJsonObject>


struct ChangeEncryptionPasswordModel {
    QString username;
    QString old_password_derived_key;
    QString new_password_derived_key;
    QString new_iv_KEK;
    QString new_encrypted_KEK;
};

struct UserInfoModel {
    QString username;
    QString email;
};

struct UploadResult {
    bool success = false;
    QString errorMessage;

    QString userId;
    QString username;
    QString email;
    QString publicKey;
    QString createdAt;
    QString modifiedAt;
    QString keyId;
    QString ivKEK;
    QString encryptedKEK;
    QString assocDataKEK;
    QString salt;
    int p = 0;
    int m = 0;
    int t = 0;
    QString kekCreatedAt;
};


class AuthAPI
{
public:
    AuthAPI();

    static bool registerUser(const UserRegisterModel& model, QString& responseOut, QString& errorOut);
    static bool loginUser(const QString& username, const QString& entered_auth_password,
                   UserLoginModel& responseOut, QString& errorOut);
    static bool changeAuthPassword(const QString username, const QString oldPassword,
                            const QString newPassword , QString& responseOut, QString& errorOut);
    static bool changeEncryptionPassword(const ChangeEncryptionPasswordModel& model, QString& responseOut, QString& errorOut);
    static bool fetchAllUsers(QList<UserInfoModel>& usersOut, QString& errorOut);
    static bool deleteUser(const QString userID, const QString password, QString& responseOut, QString& errorOut);
    // static bool getUserInfo(const QString& userId, QJsonObject& userJsonOut);
    static UploadResult getUserInfo(const QString& authToken, QString& errorOut);

};


#endif // AUTHAPI_H
