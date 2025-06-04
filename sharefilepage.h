#ifndef SHAREFILEPAGE_H
#define SHAREFILEPAGE_H

#include <QWidget>

namespace Ui {
class ShareFilePage;
}

class ShareFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFilePage(QWidget *parent = nullptr);
    void setFileId(const QString file_id);
    ~ShareFilePage();

private slots:
    void on_dashboardButton_clicked();

private:
    Ui::ShareFilePage *ui;
    QString file_id;

signals:
    void backToDashboard();
};

#endif // SHAREFILEPAGE_H
