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
    ~FileUploadPage();

private slots:

    void on_uploadFileButton_clicked();

    void on_encryptFileButton_clicked();

private:
    Ui::FileUploadPage *ui;
};

#endif // FILEUPLOADPAGE_H
