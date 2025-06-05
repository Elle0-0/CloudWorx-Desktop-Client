#include "keygenutils.h"

#include <sodium.h>
#include <QByteArray>
#include <QDebug>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

QString encodeBase64(const QByteArray& data) {
    return QString::fromLatin1(data.toBase64());
}

bool initializeSodium() {
    if (sodium_init() < 0) {
        qWarning() << "Failed to initialize libsodium";
        return false;
    }
    return true;
}

bool generateX25519Keypair(QString& outPublicKeyB64, QString& outPrivateKeyB64) {
    unsigned char publicKey[crypto_kx_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_kx_SECRETKEYBYTES];

    if (crypto_kx_keypair(publicKey, privateKey) != 0) {
        qWarning() << "Failed to generate X25519 keypair";
        return false;
    }

    outPrivateKeyB64 = encodeBase64(QByteArray(reinterpret_cast<const char*>(privateKey), crypto_kx_SECRETKEYBYTES));
    outPublicKeyB64 = encodeBase64(QByteArray(reinterpret_cast<const char*>(publicKey), crypto_kx_PUBLICKEYBYTES));

    return true;
}


QString publicKeyToPem(const unsigned char* publicKey, size_t keyLen) {
    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, publicKey, keyLen);
    if (!pkey) {
        qWarning() << "Failed to create EVP_PKEY from raw public key";
        return {};
    }

    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
        qWarning() << "Failed to create BIO";
        EVP_PKEY_free(pkey);
        return {};
    }

    if (!PEM_write_bio_PUBKEY(mem, pkey)) {
        qWarning() << "Failed to write PEM public key";
        EVP_PKEY_free(pkey);
        BIO_free(mem);
        return {};
    }

    BUF_MEM* memPtr;
    BIO_get_mem_ptr(mem, &memPtr);

    QString pemString = QString::fromUtf8(memPtr->data, memPtr->length);

    EVP_PKEY_free(pkey);
    BIO_free(mem);

    return pemString;
}

QString privateKeyToPem(const unsigned char* privateKey, size_t keyLen) {
    EVP_PKEY* pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, privateKey, keyLen);
    if (!pkey) {
        qWarning() << "Failed to create EVP_PKEY from raw private key";
        return {};
    }

    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
        qWarning() << "Failed to create BIO";
        EVP_PKEY_free(pkey);
        return {};
    }

    if (!PEM_write_bio_PrivateKey(mem, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        qWarning() << "Failed to write PEM private key";
        EVP_PKEY_free(pkey);
        BIO_free(mem);
        return {};
    }

    BUF_MEM* memPtr;
    BIO_get_mem_ptr(mem, &memPtr);

    QString pemString = QString::fromUtf8(memPtr->data, memPtr->length);

    EVP_PKEY_free(pkey);
    BIO_free(mem);

    return pemString;
}

GeneratedKeypair generateAndExportX25519Keypair() {
    GeneratedKeypair result;

    QString pubB64, privB64;
    bool keygenSuccess = generateX25519Keypair(pubB64, privB64);
    if (!keygenSuccess) {
        qWarning() << "Failed to generate X25519 keypair";
        result.success = false;
        return result;
    }

    QByteArray pubRaw = QByteArray::fromBase64(pubB64.toUtf8());
    QByteArray privRaw = QByteArray::fromBase64(privB64.toUtf8());

    QString pubPEM = publicKeyToPem(reinterpret_cast<const unsigned char*>(pubRaw.constData()), pubRaw.size());
    QString privPEM = privateKeyToPem(reinterpret_cast<const unsigned char*>(privRaw.constData()), privRaw.size());

    result.publicKeyBase64 = pubB64;
    result.privateKeyBase64 = privB64;
    result.publicKeyPEM = pubPEM;
    result.privateKeyPEM = privPEM;
    result.success = true;

    return result;
}

bool generateEd25519Keypair(QString& outPublicKeyB64, QString& outPrivateKeyB64) {
    unsigned char publicKey[crypto_sign_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_sign_SECRETKEYBYTES];

    if (crypto_sign_keypair(publicKey, privateKey) != 0) {
        qWarning() << "Failed to generate Ed25519 keypair";
        return false;
    }

    outPrivateKeyB64 = encodeBase64(QByteArray(reinterpret_cast<const char*>(privateKey), crypto_sign_SECRETKEYBYTES));
    outPublicKeyB64 = encodeBase64(QByteArray(reinterpret_cast<const char*>(publicKey), crypto_sign_PUBLICKEYBYTES));

    return true;
}

QString ed25519PublicKeyToPem(const unsigned char* publicKey, size_t keyLen) {
    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, publicKey, keyLen);
    if (!pkey) {
        qWarning() << "Failed to create EVP_PKEY from raw Ed25519 public key";
        return {};
    }

    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
        qWarning() << "Failed to create BIO";
        EVP_PKEY_free(pkey);
        return {};
    }

    if (!PEM_write_bio_PUBKEY(mem, pkey)) {
        qWarning() << "Failed to write PEM Ed25519 public key";
        EVP_PKEY_free(pkey);
        BIO_free(mem);
        return {};
    }

    BUF_MEM* memPtr;
    BIO_get_mem_ptr(mem, &memPtr);

    QString pemString = QString::fromUtf8(memPtr->data, memPtr->length);

    EVP_PKEY_free(pkey);
    BIO_free(mem);

    return pemString;
}

QString ed25519PrivateKeyToPem(const unsigned char* privateKey, size_t keyLen) {
    EVP_PKEY* pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, privateKey, keyLen);
    if (!pkey) {
        qWarning() << "Failed to create EVP_PKEY from raw Ed25519 private key";
        return {};
    }

    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
        qWarning() << "Failed to create BIO";
        EVP_PKEY_free(pkey);
        return {};
    }

    if (!PEM_write_bio_PrivateKey(mem, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        qWarning() << "Failed to write PEM Ed25519 private key";
        EVP_PKEY_free(pkey);
        BIO_free(mem);
        return {};
    }

    BUF_MEM* memPtr;
    BIO_get_mem_ptr(mem, &memPtr);

    QString pemString = QString::fromUtf8(memPtr->data, memPtr->length);

    EVP_PKEY_free(pkey);
    BIO_free(mem);

    return pemString;
}

GeneratedKeypair generateAndExportEd25519Keypair() {
    GeneratedKeypair result;

    QString pubB64, privB64;
    bool keygenSuccess = generateEd25519Keypair(pubB64, privB64);
    if (!keygenSuccess) {
        qWarning() << "Failed to generate Ed25519 keypair";
        result.success = false;
        return result;
    }

    QByteArray pubRaw = QByteArray::fromBase64(pubB64.toUtf8());
    QByteArray privRaw = QByteArray::fromBase64(privB64.toUtf8());

    QString pubPEM = ed25519PublicKeyToPem(reinterpret_cast<const unsigned char*>(pubRaw.constData()), pubRaw.size());
    QString privPEM = ed25519PrivateKeyToPem(reinterpret_cast<const unsigned char*>(privRaw.constData()), 32);


    result.publicKeyBase64 = pubB64;
    result.privateKeyBase64 = privB64;
    result.publicKeyPEM = pubPEM;
    result.privateKeyPEM = privPEM;
    result.success = true;

    return result;
}

