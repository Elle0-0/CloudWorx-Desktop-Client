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

private:
    Ui::ShareFilePage *ui;
    QString file_id;
};

#endif // SHAREFILEPAGE_H
