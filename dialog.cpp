//
// Dialog class
//

#include <dialog.h>
#include <string>

Dialog::Dialog(QWidget *parent)
   : _parent(parent),_valA(1),_valB(2),_valC(3),_title("dialog"),_result(-1)
{
    _dlg = new QDialog(parent);
    _vbox = new QVBoxLayout(_dlg);
    _msg = new QLabel();
    _btnBox = new QHBoxLayout();
    _vbox->addWidget(_msg);
    _vbox->addLayout(_btnBox);

    _btnA = new QPushButton();
    _btnB = new QPushButton();
    _btnC = new QPushButton();

    _btnA->setFlat(true);
    _btnB->setFlat(true);
    _btnC->setFlat(true);

    _btnA->setProperty("buttonNo", _valA);
    _btnB->setProperty("buttonNo", _valB);
    _btnC->setProperty("buttonNo", _valC);

    QObject::connect(_btnA, SIGNAL(clicked()), this, SLOT(exitDialog()));
    QObject::connect(_btnB, SIGNAL(clicked()), this, SLOT(exitDialog()));
    QObject::connect(_btnC, SIGNAL(clicked()), this, SLOT(exitDialog()));

    _btnBox->addWidget(_btnA);
    _btnBox->addWidget(_btnB);
    _btnBox->addWidget(_btnC);
}

Dialog::~Dialog()
{
    dismiss();
    _btnBox->removeWidget(_btnA);
    _btnBox->removeWidget(_btnB);
    _btnBox->removeWidget(_btnC);
    _vbox->removeWidget(_msg);
    delete _msg;
    delete _btnA;
    delete _btnB;
    delete _btnC;
    delete _btnBox;
    delete _vbox;
    delete _dlg;
}


void
Dialog::_print() const
{
    _msg->setText(_message.c_str());
    _msg->wordWrap();
}

int
Dialog::draw(void)
{
    _dlg->setModal(true);
    _dlg->setWindowTitle(_title.c_str());
    _print(); // put message
    _result = _dlg->exec();
    return _result;
}

void
Dialog::dismiss()
{
    _btnA->hide();
    _btnB->hide();
    _btnC->hide();
    _msg->hide();
    _dlg->hide();
}

void
Dialog::button(const std::string &labelA, const std::string &labelB, const std::string &labelC)
{
    _btnA->setText(QString(labelA.c_str()));
    _btnB->setText(QString(labelB.c_str()));
    _btnC->setText(QString(labelC.c_str()));

    if (labelA.empty()) _btnA->hide(); // setEnabled(false);
    if (labelB.empty()) _btnB->hide(); // setEnabled(false);
    if (labelC.empty()) _btnC->hide(); // setEnabled(false);
}

void
Dialog::exitDialog()
{
    _result = (sender()->property("buttonNo")).toInt();
    _dlg->done(_result);
}

ScrollMessageBox::ScrollMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags f)
    :QMessageBox(icon, title, text, buttons, parent, f)
{
    this->init(text);
}

void
ScrollMessageBox::init(const QString &text)
{
    QGridLayout *l = qobject_cast<QGridLayout*>(this->layout());
    QLabel *label = this->findChild<QLabel*>("qt_msgbox_label");
    QLabel *iconLabel = this->findChild<QLabel*>("qt_msgboxex_icon_label");
    QLabel *content = new QLabel(label->text());
    label->clear();
    label->hide();
    content->setWordWrap(true);
    content->setOpenExternalLinks(true);
    QVBoxLayout *vbox = new QVBoxLayout(label);
    vbox->addWidget(content);
    _scrollArea = new QScrollArea(label);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _scrollArea->setMinimumSize(label->width(),label->height());
    label->setMinimumSize(label->width(),label->height());
    label->show();
    content->show();
}

void
ScrollMessageBox::about(QWidget *parent, const QString &title, const QString &text)
{
    ScrollMessageBox *scrBox = new ScrollMessageBox(QMessageBox::Icon::Information, title, text, QMessageBox::StandardButton::Ok, parent);
    scrBox->setAttribute(Qt::WA_DeleteOnClose);
    QIcon icon = scrBox->windowIcon();
    QSize size = icon.actualSize(QSize(64, 64));
    scrBox->setIconPixmap(icon.pixmap(size));

    scrBox->exec();
}
