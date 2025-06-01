#include "loginpage.h"
#include "ui_loginpage.h"
#include "argon2id_utils.h"

#include <QMessageBox>
#include <sodium.h>


LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPage)
{
    ui->setupUi(this);
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::on_backButton_clicked()
{
    emit backToMenu();
}


void LoginPage::on_signUpButton_clicked()
{
    emit backToMenu();
}


void LoginPage::on_loginButton_clicked()
{
    QString username = ui->usernameTextField->text();
    QString password = ui->passwordTextField->text();

    QString placeHolder = "$argon2id$v=19$m=12,t=3,p=1$xekKGWWnO1kqr2WZxoIudQ$LSndBzDryGPZYv/DU53KfpdESXQNPObrCozBjdJUBJw";
    QByteArray passwordBytes = password.toUtf8();

    if (!verifyPassword(passwordBytes, placeHolder)) {
        QMessageBox::information(this, "error", "Incorrect Password");
        return;
    }

    sodium_memzero(password.data(), password.size());
    sodium_memzero(passwordBytes.data(), passwordBytes.size());

    emit goToDashboard();
}

