#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "LoginPage.h"
#include "SignUpPage.h"
#include "fileuploadpage.h"
#include "filepasswordpage.h"
#include "dashboard.h"
#include "decryptfilepage.h"
#include "sharefilepage.h"
#include "keygenpage.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void goToLogin();

    void goToSignUp();

    void goToMenu();

    void goToFileUpload();

    void goToFilePassword();

    void goToDashboard();

    void goToFileDecryption();

    void goToFileShare();

    void goToKenGen();


private:
    Ui::MainWindow *ui;
    LoginPage *loginPage;
    SignUpPage *signUpPage;
    FileUploadPage *fileUploadPage;
    FilePasswordPage *filePasswordPage;
    Dashboard *dashboardPage;
    DecryptFilePage *decryptFilePage;
    ShareFilePage *shareFilepage;
    KeyGenPage *keyGenPage;
};
#endif // MAINWINDOW_H
