#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginPage = new LoginPage(this);
    signUpPage = new SignUpPage(this);
    fileUploadPage = new FileUploadPage(this);

    ui->stackedWidget->addWidget(loginPage);
    ui->stackedWidget->addWidget(signUpPage);
    ui->stackedWidget->addWidget(fileUploadPage);

    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::goToLogin);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::goToSignUp);
    connect(ui->uploadFileButton, &QPushButton::clicked, this, &MainWindow::goToFileUpload);
    connect(loginPage, &LoginPage::backToMenu, this, &MainWindow::goToMenu);
    connect(signUpPage, &SignUpPage::backToMenu, this, &MainWindow::goToMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::goToLogin() {
    ui->stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::goToSignUp() {
    ui->stackedWidget->setCurrentWidget(signUpPage);
}
void MainWindow::goToMenu() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::goToFileUpload() {
    ui->stackedWidget->setCurrentWidget(fileUploadPage);
}


