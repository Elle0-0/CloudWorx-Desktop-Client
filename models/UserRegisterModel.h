#ifndef USERREGISTERMODEL_H
#define USERREGISTERMODEL_H

#include <QString>

struct UserRegisterModel {
    QString username;
    QString email;
    QString auth_password;
    QString public_key;
    QString iv_KEK;
    QString encrypted_KEK;
};

#endif // USERREGISTERMODEL_H
