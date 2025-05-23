#include "signuppage.h"
#include "ui_signuppage.h"

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

