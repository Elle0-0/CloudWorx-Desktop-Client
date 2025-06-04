#ifndef FILEUPLOADPAGE_H
#define FILEUPLOADPAGE_H

#include <QWidget>

namespace Ui {
class FileUploadPage;
}

class FileUploadPage : public QWidget
{
    Q_OBJECT

public:
    explicit FileUploadPage(QWidget *parent = nullptr);
    void setIdAndToken(QString userId, QString jwtToken);
    ~FileUploadPage();

private slots:

    void on_uploadFileButton_clicked();

    void on_encryptFileButton_clicked();

    void on_dashboardButton_clicked();

private:
    Ui::FileUploadPage *ui;
    QString selectedFilePath;
    QString getSelectedFilePath() const;
    QString userId;
    QString jwtToken;

signals:
    void backToDashboard();
};


#endif // FILEUPLOADPAGE_H
