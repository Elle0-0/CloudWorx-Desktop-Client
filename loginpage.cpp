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

    // Input validation
    if (username.trimmed().isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Username and password cannot be empty.");
        return;
    }

    UserLoginModel loginResponse;
    QString error;
    bool success = AuthAPI::loginUser(username, password, loginResponse, error);


    sodium_memzero(password.data(), password.size());

    if (!success) {
        qWarning() << "Login failed:" << error;
        QMessageBox::critical(this, "Login failed:", error);
        return;
    } else {
        qDebug() << "Login successful. usernID: " << loginResponse.user_id;
        qDebug() << "Login successful. Token: " << loginResponse.token;
        emit goToDashboard(loginResponse.user_id, loginResponse.token);
    }

}
void LoginPage::reset()
{
    ui->usernameTextField->clear();
    ui->passwordTextField->clear();

    ui->usernameTextField->setFocus();

}

