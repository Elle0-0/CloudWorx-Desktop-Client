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

private:
    Ui::FileUploadPage *ui;
};

#endif // FILEUPLOADPAGE_H
