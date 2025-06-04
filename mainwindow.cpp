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
    filePasswordPage = new FilePasswordPage(this);
    dashboardPage = new Dashboard(this);
    decryptFilePage = new DecryptFilePage(this);
    shareFilepage = new ShareFilePage(this);
    keyGenPage = new KeyGenPage(this);

    ui->stackedWidget->addWidget(loginPage);
    ui->stackedWidget->addWidget(signUpPage);
    ui->stackedWidget->addWidget(fileUploadPage);
    ui->stackedWidget->addWidget(filePasswordPage);
    ui->stackedWidget->addWidget(dashboardPage);
    ui->stackedWidget->addWidget(decryptFilePage);
    ui->stackedWidget->addWidget(shareFilepage);
    ui->stackedWidget->addWidget(keyGenPage);

    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::goToLogin);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::goToSignUp);
    connect(ui->uploadFileButton, &QPushButton::clicked, this, &MainWindow::goToFileUpload);
    connect(loginPage, &LoginPage::backToMenu, this, &MainWindow::goToMenu);
    connect(signUpPage, &SignUpPage::backToMenu, this, &MainWindow::goToMenu);
    connect(filePasswordPage, &FilePasswordPage::goToKeyGen, this, &MainWindow::goToKenGen);
    connect(decryptFilePage, &DecryptFilePage::backToDashboard, this, &MainWindow::goToDashboard);
    connect(fileUploadPage, &FileUploadPage::backToDashboard, this, &MainWindow::goToDashboard);
    connect(shareFilepage, &ShareFilePage::backToDashboard, this, &MainWindow::goToDashboard);
    connect(keyGenPage, &KeyGenPage::goToLogin, this, &MainWindow::goToLogin);

    connect(signUpPage, &SignUpPage::signUpSuccessful, this, [=](const QString &username, const QString &email, const QString &hashedPassword) {
        filePasswordPage->setUserData(username, email, hashedPassword);
        goToFilePassword();
    });
    connect(dashboardPage, &Dashboard::goToFileDecryption, this, [=](const FileData &file, QString jwtToken){
            decryptFilePage->setFileData(file, jwtToken);
            goToFileDecryption();
    });
    connect(dashboardPage, &Dashboard::goToFileUpload, this, [=](QString userId, QString jwtToken){
        fileUploadPage->setIdAndToken(userId, jwtToken);
        goToFileUpload();
    });
    connect(filePasswordPage, &FilePasswordPage::goToKeyGen, this, [=](const QString pub, QString priv){
        keyGenPage->setPubAndPriv(pub, priv);
        goToKenGen();
    });
    connect(loginPage, &LoginPage::goToDashboard, this, [=](QString userId, QString jwtToken){
        dashboardPage->setIdAndToken(userId, jwtToken);
        goToDashboard();
    });

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
void MainWindow::goToFilePassword() {
    ui->stackedWidget->setCurrentWidget(filePasswordPage);
}
void MainWindow::goToDashboard(){
    ui->stackedWidget->setCurrentWidget(dashboardPage);
}

void MainWindow::goToFileDecryption(){
    ui->stackedWidget->setCurrentWidget(decryptFilePage);
}
void MainWindow::goToFileShare() {
    ui->stackedWidget->setCurrentWidget(shareFilepage);
}
void MainWindow::goToKenGen() {
    ui->stackedWidget->setCurrentWidget(keyGenPage);
}

