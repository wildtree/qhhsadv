//
// Dialog class
//

#if !defined(DIALOG_H)
#define DIALOG_H

#include <sys/types.h>
#include <string>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVariant>

class Dialog : public QObject
{
    Q_OBJECT

protected:
    QWidget *_parent;
    QDialog *_dlg;
    QPushButton *_btnA, *_btnB, *_btnC;
    QVBoxLayout *_vbox;
    QHBoxLayout *_btnBox;
    QLabel *_msg;
    QVariant _valA, _valB, _valC;
    std::string _title;
    std::string _message;
    int _result;

    void _print() const;
public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void setTitle(const std::string &title) { _title = title; }
    void setMessage(const std::string &message) { _message = message; }
    int draw();
    void invalidate() const {}
    void dismiss();
    void button(const std::string &labelA, const std::string &labelB, const std::string &labelC);
    QPushButton *btnA() { return _btnA; }
    QPushButton *btnB() { return _btnB; }
    QPushButton *btnC() { return _btnC; }

    static const int FontHeight = 16;
    static const int FontWidth = 8;
public slots:
    void exitDialog();
};

#endif
