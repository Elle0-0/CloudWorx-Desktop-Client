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


#endif // KEYGENUTILS_H
