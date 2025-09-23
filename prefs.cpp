
#include <prefs.h>
#include <iostream>
#include <QtCore>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QtCore/qglobal.h>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>


void
Prefs::dialog()
{
    QDialog dlg;
    QCheckBox *chkSound = new QCheckBox();
    chkSound->setChecked(_sound);
    QObject::connect(chkSound, SIGNAL(stateChanged(int)), this, SLOT(setSound(int)));

    QCheckBox *chkSkipOpening = new QCheckBox();
    chkSkipOpening->setChecked(_skip_opening);
    QObject::connect(chkSkipOpening, SIGNAL(stateChanged(int)), this, SLOT(setSkipOpening(int)));

    QFormLayout *form0 = new QFormLayout();
    form0->setLabelAlignment(Qt::AlignLeft);
    form0->addRow("音を鳴らす", chkSound);

    QFormLayout *form1 = new QFormLayout();
    form1->setLabelAlignment(Qt::AlignLeft);
    form1->addRow("オープニングを飛ばす", chkSkipOpening);

    QLabel *label = new QLabel("テーマ");
    QRadioButton *light = new QRadioButton("ライト");
    QRadioButton *dark  = new QRadioButton("ダーク");
    QRadioButton *sys   = new QRadioButton("システム");
    QRadioButton *defchk = sys;
    switch(_theme_type)
    {
    case ThemeType::Light: defchk = light; break;
    case ThemeType::Dark:  defchk = dark; break;
    }
    defchk->setChecked(true);

    QButtonGroup *group = new QButtonGroup();
    group->addButton(light, 0);
    group->addButton(dark, 1);
    group->addButton(sys, 2);
    QObject::connect(group, &QButtonGroup::idClicked, this, [=](int id)
    {
        setThemeType(id);
    });

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(label);
    hbox->addWidget(light);
    hbox->addWidget(dark);
    hbox->addWidget(sys);
    hbox->addStretch();

    QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok);
    bbox->button(QDialogButtonBox::Ok)->setText("了解");

    QVBoxLayout *vbox = new QVBoxLayout(&dlg);
    vbox->addLayout(form0);
    vbox->addLayout(form1);
    vbox->addLayout(hbox);
    vbox->addWidget(bbox);

    dlg.setModal(true);
    dlg.setWindowTitle("設定");

    QObject::connect(bbox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    dlg.exec();
    save();
//    QObject::disconnect(&dlg, SIGNAL(accepted()), &dlg, SLOT(accept()));
}

void
Prefs::load()
{
    QFile f(_f.c_str());
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) return;
    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    setSound(root.value("sound").toBool(true));
    std::string theme = root.value("theme").toString().toStdString();
    if (theme == "light") setThemeType(ThemeType::Light);
    else if (theme == "dark") setThemeType(ThemeType::Dark);
    else setThemeType(ThemeType::System);
}

void
Prefs::save() const
{
    QJsonObject root;
    root["sound"] = getSound();
    switch(getThemeType())
    {
    case ThemeType::Light:
        root["theme"] = "light";
        break;
    case ThemeType::Dark:
        root["theme"] = "dark";
        break;
    default:
        root["theme"] = "system";
    }
    QJsonDocument doc(root);
    QFile f(_f.c_str());
    if (!f.open(QIODevice::WriteOnly|QIODevice::Text)) return;
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
}
