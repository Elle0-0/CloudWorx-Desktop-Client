#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    void reset();
    ~LoginPage();

private slots:
    void on_backButton_clicked();

    void on_signUpButton_clicked();

    void on_loginButton_clicked();

private:
    Ui::LoginPage *ui;

signals:
    void backToMenu();

    void goToDashboard(QString userId, QString jwtToken);
};

#endif // LOGINPAGE_H
