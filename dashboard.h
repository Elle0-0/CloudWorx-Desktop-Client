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
    ~Dashboard();

private:
    Ui::Dashboard *ui;
    FileListModel *model;
    QString userId;
    QString jwtToken;

signals:
    void goToFileDecryption(const FileData &file, QString jwtToken);
    void goToFileShare(const QString file_id);
    void goToFileUpload(const QString userId,const QString jwtToken);

private slots:
    void on_decryptFileButton_clicked();
    void on_shareFileButton_clicked();
    void on_uploadFileButton_clicked();
};

#endif // DASHBOARD_H
