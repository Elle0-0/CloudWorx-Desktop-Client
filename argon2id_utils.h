#ifndef ARGON2ID_UTILS_H
#define ARGON2ID_UTILS_H

#include <QString>

    QString hashPassword(const QByteArray& password);
    bool verifyPassword(const QByteArray& password, const QString& hash);
    bool isPasswordNISTCompliant(const QString& password, QString& errorMessage);
    bool hasRepetitivePattern(const QString& password);

#endif // ARGON2ID_UTILS_H
