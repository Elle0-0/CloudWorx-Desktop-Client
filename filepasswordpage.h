#ifndef FILEPASSWORDPAGE_H
#define FILEPASSWORDPAGE_H

#include <QWidget>

namespace Ui {
class FilePasswordPage;
}

class FilePasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit FilePasswordPage(QWidget *parent = nullptr);
    ~FilePasswordPage();

private slots:
    void on_createPasswordButton_clicked();

private:
    Ui::FilePasswordPage *ui;
};

#endif // FILEPASSWORDPAGE_H
