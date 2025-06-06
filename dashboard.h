#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "models/filelistmodel.h"

#include <QWidget>

namespace Ui {
class Dashboard;
}

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    void setIdAndToken(QString userId, QString jwtToken);
    void loadFiles();
    void loadSharedFiles();
    void reset();
    ~Dashboard();

private:
    Ui::Dashboard *ui;
    FileListModel *model;
    QString userId;
    QString jwtToken;


signals:
    void goToFileDecryption(const FileData &file, QString jwtToken);
    void goToFileShare(const FileData &file, QString jwtToken);
    void goToFileUpload(const QString userId, const QString jwtToken);
    void goToDecryptSharedFile(FileData file, QString jwtToken);
    void logOutRequested();


private slots:
    void on_decryptFileButton_clicked();
    void on_shareFileButton_clicked();
    void on_uploadFileButton_clicked();
    void on_refreshButton_clicked();
    void on_deleteFileButton_clicked();
    void onTabChanged(int index);
    void on_logOutButton_clicked();
};

#endif // DASHBOARD_H
