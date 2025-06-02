#ifndef AUTHAPI_H
#define AUTHAPI_H

#include "../models/UserRegisterModel.h"
#include "../models/UserLoginModel.h"


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



class AuthAPI
{
public:
    AuthAPI();

    bool registerUser(const UserRegisterModel& model, QString& responseOut);
    bool loginUser(const QString& username, const QString& entered_auth_password,
                   UserLoginModel& responseOut, QString& errorOut);
    bool changeAuthPassword(const QString username, const QString oldPassword,
                            const QString newPassword , QString& responseOut);
    bool changeEncryptionPassword(const ChangeEncryptionPasswordModel& model, QString& responseOut);
    bool fetchAllUsers(QList<UserInfoModel>& usersOut, QString& errorOut);
    bool deleteUser(const QString userID, const QString password, QString& responseOut);


};


#endif // AUTHAPI_H
