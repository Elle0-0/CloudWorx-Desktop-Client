#include "filepasswordpage.h"
#include "ui_filepasswordpage.h"
#include "argon2id_utils.h"
#include "envelopeencryptionmanager.h"
#include "utils/keygenutils.h"

#include <QMessageBox>
#include <sodium.h>
#include <QString>

FilePasswordPage::FilePasswordPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilePasswordPage)
{
    ui->setupUi(this);
}

FilePasswordPage::~FilePasswordPage()
{
    delete ui;
}
void FilePasswordPage::setUserData(const QString &username, const QString &email, const QString &password) {
    this->username = username;
    this->email = email;
    this->password = password;

    //display to make sure for now
    qDebug() << "User data received in FilePasswordPage:";
    qDebug() << "Username:" << username;
    qDebug() << "Email:" << email;
    qDebug() << "Password:" << password;
}

void FilePasswordPage::on_createPasswordButton_clicked()
{
    QString password = ui->passwordTextField->text();
    QString rePassword = ui->rePasswordTextField->text();

    QString errorMessage;

    if (password != rePassword) {
        ui->errorLabel->setText("Passwords no not match");
        return;
    }

    if (!isPasswordNISTCompliant(password, errorMessage)) {
        QMessageBox::warning(this, "error", errorMessage);
        return;
    }

    ui->errorLabel->clear();

    QByteArray passwordBytes = password.toUtf8();

    GeneratedKeypair xKeypair = generateAndExportX25519Keypair();
    if (!xKeypair.success) {
        QMessageBox::warning(this, "KeyGen Error", "Failed to generate X25519 keys");
        return;
    }

    GeneratedKeypair edKeypair = generateAndExportEd25519Keypair();
    if (!edKeypair.success) {
        QMessageBox::warning(this, "KeyGen Error", "Failed to generate Ed25519 keys");
        return;
    }

    qDebug() << "X25519 Public Key (PEM):\n" << encodeBase64(xKeypair.publicKeyPEM.toUtf8());
    qDebug() << "X25519 Private Key (PEM):\n" << xKeypair.privateKeyPEM;
    qDebug() << "Ed25519 Public Key (PEM):\n" << edKeypair.publicKeyPEM;
    qDebug() << "Ed25519 Private Key (PEM):\n" << edKeypair.privateKeyPEM;

    bool success = EnvelopeEncryptionManager::setupUserEncryption(username, email, password, passwordBytes,
                                                                  xKeypair.publicKeyPEM, edKeypair.publicKeyPEM);


    if (!success) {
        qDebug() << "error";
        return;
    }else {
        qDebug() << "User file password set";
        emit goToKeyGen(encodeBase64(xKeypair.publicKeyPEM.toUtf8()), encodeBase64(xKeypair.privateKeyPEM.toUtf8()),
                        encodeBase64(edKeypair.publicKeyPEM.toUtf8()), encodeBase64(edKeypair.privateKeyPEM.toUtf8()));
    }

    // Clear sensitive data from memory
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
    sodium_memzero(password.data(), password.size());

}

