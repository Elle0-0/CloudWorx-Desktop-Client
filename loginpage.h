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
    ~LoginPage();

private slots:
    void on_backButton_clicked();

    void on_signUpButton_clicked();

private:
    Ui::LoginPage *ui;

signals:
    void backToMenu();
};

#endif // LOGINPAGE_H
