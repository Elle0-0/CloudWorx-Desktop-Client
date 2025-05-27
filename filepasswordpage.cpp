#include "filepasswordpage.h"
#include "ui_filepasswordpage.h"
#include "argon2id_utils.h"
#include "envelopeencryption.h"

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
        QMessageBox::information(this, "error", errorMessage);
        return;
    }

    ui->errorLabel->clear();

    QByteArray passwordBytes = password.toUtf8();
    WrappedKEKResult hashedPassword = EnvelopeEncryption::generateAndWrapKEK(passwordBytes);

    // function call here to save encryption key Ckek to db

    // Clear sensitive data from memory
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
    sodium_memzero(password.data(), password.size());


}

