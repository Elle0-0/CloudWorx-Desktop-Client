#include "filepasswordpage.h"
#include "ui_filepasswordpage.h"
#include "argon2id_utils.h"
#include "envelopeencryptionmanager.h"

#include <QMessageBox>
#include <sodium.h>

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
void FilePasswordPage::setUserData(const QString &username, const QString &email, const QString &hashedPassword) {
    this->username = username;
    this->email = email;
    this->hashedPassword = hashedPassword;

    //display to make sure for now
    qDebug() << "User data received in FilePasswordPage:";
    qDebug() << "Username:" << username;
    qDebug() << "Email:" << email;
    qDebug() << "Hashed Password:" << hashedPassword;
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

    bool success = EnvelopeEncryptionManager::setupUserEncryption(username, email, hashedPassword, passwordBytes);

    if (success) qDebug() << "User file password set";


    // Clear sensitive data from memory
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
    sodium_memzero(password.data(), password.size());

}

