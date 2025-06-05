#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // stylesheet
    QString mainStyle = R"(
        /* Global App Styles */
        QWidget {
            font-family: "Segoe UI", sans-serif;
            font-size: 14px;
            color: #1a1a1a;
            background-color: #f5f7fa;
        }

        /* Stacked Widgets */
        QStackedWidget {
            background-color: #ffffff;
            border-radius: 12px;
            padding: 20px;
        }

        /* Labels */
        QLabel {
            font-size: 18px;
            color: #1a66ff;
            font-weight: 500;
            background-color: transparent !important;
        }

        /* Customize main heading on login page */
        #LoginPage QLabel#label {
            font-size: 36px;
            font-weight: 600;
            color: #4A90E2;
        }

        /* Secondary text */
        #LoginPage QLabel#label_5 {
            color: #777;
            font-size: 13px;
        }

        /* Input fields */
        QLineEdit {
            border: 2px solid #ccc;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            background-color: #ffffff;
            color: #1a1a1a;

            qproperty-alignment: AlignVCenter;
        }
        QLineEdit:focus {
            border: 2px solid #4A90E2;
        }

        /* Buttons */
        QPushButton {
            background-color: #4A90E2;
            color: white;
            font-weight: 600;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #5AA9F4;
        }
        QPushButton:pressed {
            background-color: #3A78C2;
        }
        QPushButton:disabled {
            background-color: #A0C4FF;
            color: #E0E0E0;
        }

        /* Optional: Back button and Sign Up button on login page */
        #LoginPage QPushButton#backButton {
            background: transparent;
            color: #666;
            font-size: 14px;
        }
        #LoginPage QPushButton#backButton:hover {
            color: #333;
        }

        #LoginPage QPushButton#signUpButton {
            background: transparent;
            color: #4A90E2;
            font-size: 14px;
            text-decoration: underline;
        }
        #LoginPage QPushButton#signUpButton:hover {
            color: #1C70C1;
        }
        /* Dashboard specific styles */
        #Dashboard {
            background-color: #ffffff;
            border-radius: 12px;
            padding: 24px;
            /* Note: Qt does not natively support box-shadow in stylesheets */
            /* You can emulate shadows with QGraphicsDropShadowEffect in code if needed */
        }

        #Dashboard QTabWidget {
            background-color: #f0f4f8;
            border-radius: 12px;
            padding: 12px;
        }

        #Dashboard QTabBar::tab {
            background: #e0e6ef;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: 600;
            color: #2a2a2a;
            min-width: 80px;
        }

        #Dashboard QTabBar::tab:selected {
            background: #4A90E2;
            color: white;
        }

        #Dashboard QListView {
            background-color: white;
            border: 1px solid #ccc;
            border-radius: 10px;
            padding: 8px;
            font-size: 14px;
        }
)";


    qApp->setStyleSheet(mainStyle);


    loginPage = new LoginPage(this);
    signUpPage = new SignUpPage(this);
    fileUploadPage = new FileUploadPage(this);
    filePasswordPage = new FilePasswordPage(this);
    dashboardPage = new Dashboard(this);
    decryptFilePage = new DecryptFilePage(this);
    shareFilepage = new ShareFilePage(this);
    keyGenPage = new KeyGenPage(this);
    decryptSharedFile = new DecryptSharedPage(this);


    ui->stackedWidget->addWidget(loginPage);
    ui->stackedWidget->addWidget(signUpPage);
    ui->stackedWidget->addWidget(fileUploadPage);
    ui->stackedWidget->addWidget(filePasswordPage);
    ui->stackedWidget->addWidget(dashboardPage);
    ui->stackedWidget->addWidget(decryptFilePage);
    ui->stackedWidget->addWidget(shareFilepage);
    ui->stackedWidget->addWidget(keyGenPage);
    ui->stackedWidget->addWidget(decryptSharedFile);

    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::goToLogin);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::goToSignUp);
    // connect(ui->uploadFileButton, &QPushButton::clicked, this, &MainWindow::goToFileUpload);
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
    connect(dashboardPage, &Dashboard::goToFileShare, this, [=](const FileData &file, QString jwtToken){
        shareFilepage->setFileData(file, jwtToken);
        goToFileShare();
    });
    connect(dashboardPage, &Dashboard::goToFileUpload, this, [=](QString userId, QString jwtToken){
        fileUploadPage->setIdAndToken(userId, jwtToken);
        goToFileUpload();
    });
    connect(filePasswordPage, &FilePasswordPage::goToKeyGen, this, [=](const QString pub, QString priv, const QString signPub, QString signPriv){
        keyGenPage->setPubAndPriv(pub, priv, signPub, signPriv);
        goToKenGen();
    });
    connect(loginPage, &LoginPage::goToDashboard, this, [=](QString userId, QString jwtToken){
        dashboardPage->setIdAndToken(userId, jwtToken);
        goToDashboard();
    });
    connect(dashboardPage, &Dashboard::goToDecryptSharedFile, this, [=](const FileData &file, QString jwtToken){
        decryptSharedFile->setFileData(file);
        goToDecryptSharedFile();
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
void MainWindow::goToDecryptSharedFile() {
    ui->stackedWidget->setCurrentWidget(decryptSharedFile);
}

