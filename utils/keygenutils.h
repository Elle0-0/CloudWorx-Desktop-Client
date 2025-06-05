#ifndef KEYGENUTILS_H
#define KEYGENUTILS_H

#pragma once
#include <QString>


struct GeneratedKeypair {
    QString publicKeyBase64;
    QString privateKeyBase64;
    QString publicKeyPEM;
    QString privateKeyPEM;
    bool success;
};


bool initializeSodium();
bool generateEd25519Keypair(QString& outPublicKeyB64, QString& outPrivateKeyB64);
QString privateKeyToPem(const unsigned char* privateKey, size_t keyLen);
QString publicKeyToPem(const unsigned char* publicKey, size_t keyLen) ;
GeneratedKeypair generateAndExportX25519Keypair();
QString ed25519PublicKeyToPem(const unsigned char* publicKey, size_t keyLen);
QString ed25519PrivateKeyToPem(const unsigned char* privateKey, size_t keyLen);
GeneratedKeypair generateAndExportEd25519Keypair();
QString encodeBase64(const QByteArray& data);
QByteArray decodeBase64(const QString& data);
QString encodePemStringToBase64(const QString& pemString);


// function overloading
static std::string generateKey(int length);
static std::string generateKey(); // overload

#endif // KEYGENUTILS_H
