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
    ~Dashboard();

private:
    Ui::Dashboard *ui;
    FileListModel *model;

signals:
    void goToFileDecryption(const FileModel &file);
    void goToFileShare(const QString file_id);

private slots:
    void on_decryptFileButton_clicked();
    void on_shareFileButton_clicked();
};

#endif // DASHBOARD_H
