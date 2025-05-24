#include "loginpage.h"
#include "ui_loginpage.h"
#include "argon2id_utils.h"

#include <QMessageBox>


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

    QString placeHolder = "$argon2id$v=19$m=64,t=8,p=1$lopuQimNPoUlZW5gX3YcYQ$BHWg85+QjRAy9vzvtMa8oWnh0gb2vM08fDw3eFhipLk";
    QByteArray passwordBytes = password.toUtf8();

    if (!verifyPassword(passwordBytes, placeHolder)) {
        QMessageBox::information(this, "error", "Incorrect Password");
    }

}

