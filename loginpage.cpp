#include "loginpage.h"
#include "ui_loginpage.h"
#include "network/authapi.h"

#include <QMessageBox>
#include <sodium.h>
#include <QString>


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

    UserLoginModel loginResponse;
    QString error;
    bool success = AuthAPI::loginUser(username, password, loginResponse, error);

    if (!success) {
        qWarning() << "Login failed:" << error;
        return;
    } else {
        qDebug() << "Login successful. usernID: " << loginResponse.user_id;
        qDebug() << "Login successful. Token: " << loginResponse.token;
        emit goToDashboard(loginResponse.user_id, loginResponse.token);
    }


    sodium_memzero(password.data(), password.size());
}

