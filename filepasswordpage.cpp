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

    GeneratedKeypair keypair = generateAndExportX25519Keypair();
    if (!keypair.success) {
        QMessageBox::warning(this, "KeyGen Error", "Failed to generate keys");
        return;
    }

    qDebug() << "Public Key (PEM):\n" << keypair.publicKeyPEM;
    qDebug() << "Private Key (PEM):\n" << keypair.privateKeyPEM;

    bool success = EnvelopeEncryptionManager::setupUserEncryption(username, email, password, passwordBytes,
                                                                  keypair.publicKeyPEM, keypair.privateKeyPEM);


    if (!success) {
        qDebug() << "error";
        return;
    }else {
        qDebug() << "User file password set";
        emit goToKeyGen(keypair.publicKeyPEM, keypair.privateKeyPEM);
    }

    // Clear sensitive data from memory
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
    sodium_memzero(password.data(), password.size());

}

