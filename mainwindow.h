#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "LoginPage.h"
#include "SignUpPage.h"
#include "fileuploadpage.h"
#include "filepasswordpage.h"

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


private:
    Ui::MainWindow *ui;
    LoginPage *loginPage;
    SignUpPage *signUpPage;
    FileUploadPage *fileUploadPage;
    FilePasswordPage *filePasswordPage;
};
#endif // MAINWINDOW_H
