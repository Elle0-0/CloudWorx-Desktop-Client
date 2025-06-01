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

    ui->stackedWidget->addWidget(loginPage);
    ui->stackedWidget->addWidget(signUpPage);
    ui->stackedWidget->addWidget(fileUploadPage);
    ui->stackedWidget->addWidget(filePasswordPage);
    ui->stackedWidget->addWidget(dashboardPage);
    ui->stackedWidget->addWidget(decryptFilePage);
    ui->stackedWidget->addWidget(shareFilepage);

    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::goToLogin);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::goToSignUp);
    connect(ui->uploadFileButton, &QPushButton::clicked, this, &MainWindow::goToFileUpload);
    connect(loginPage, &LoginPage::backToMenu, this, &MainWindow::goToMenu);
    connect(signUpPage, &SignUpPage::backToMenu, this, &MainWindow::goToMenu);
    connect(signUpPage, &SignUpPage::signUpSuccessful, this, [=](const QString &username, const QString &email, const QString &hashedPassword) {
        filePasswordPage->setUserData(username, email, hashedPassword);
        goToFilePassword();
    });
    connect(loginPage, &LoginPage::goToDashboard, this, &MainWindow::goToDashboard);
    connect(dashboardPage, &Dashboard::goToFileDecryption, this, [=](const FileModel &file){
            decryptFilePage->setFileData(file);
            goToFileDecryption();
    });
    connect(dashboardPage, &Dashboard::goToFileShare, this, [=](QString file_id){
        shareFilepage->setFileId(file_id);
        goToFileShare();
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

