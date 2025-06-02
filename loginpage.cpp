#include "loginpage.h"
#include "ui_loginpage.h"
#include "network/authapi.h"
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

    AuthAPI api;
    UserLoginModel loginResponse;
    QString error;
    bool success = api.loginUser(username, password, loginResponse, error);

    if (!success) {
        qWarning() << "Login failed:" << error;
        return;
    } else {
        qDebug() << "Login successful. Token: " << loginResponse.token;
    }

    emit goToDashboard();

    sodium_memzero(password.data(), password.size());
}

