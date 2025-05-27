#include "signuppage.h"
#include "ui_signuppage.h"
#include "argon2id_utils.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <iostream>
#include <sodium.h>


SignUpPage::SignUpPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignUpPage)
{
    ui->setupUi(this);
}

SignUpPage::~SignUpPage()
{
    delete ui;
}

void SignUpPage::on_backButton_clicked()
{
    emit backToMenu();
}

void SignUpPage::on_loginButton_clicked()
{
    emit backToMenu();
}

bool isValidEmail(const QString& email) {
    static const QRegularExpression regex(R"((^[\w.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$))");
    QRegularExpressionMatch match = regex.match(email);
    return match.hasMatch();
}

void SignUpPage::on_signUpButton_clicked()
{
    QString username = ui->usernameTextField->text();
    QString email = ui->emailTextField->text();
    QString password = ui->passwordTextField->text();
    QString rePassword = ui->rePasswordTextField->text();
    QString errorMessage;

    if (!isValidEmail(email)) {
        QMessageBox::information(this, "error", "invalid email address");
        return;
    }

    if (password != rePassword) {
        ui->errorMessage->setText("Passwords do not match.");
        return;
    }

    if (!isPasswordNISTCompliant(password, errorMessage)) {
        QMessageBox::information(this, "error", errorMessage);
        return;
    }

    // Password is valid, continue with signup logic
    ui->errorMessage->clear();

    // Convert to QByteArray for secure handling
    QByteArray passwordBytes = password.toUtf8();
    QString hashedPassword = hashPassword(passwordBytes);

    // Clear sensitive data from memory
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
    sodium_memzero(password.data(), password.size());

    std::cout << hashedPassword.toStdString() << std::endl;
    std::cout << "Username : " << username.toStdString() << std::endl;
    std::cout << "email : " << email.toStdString() << std::endl;

    emit signUpSuccessful();

}




