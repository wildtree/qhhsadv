
#include <prefs.h>
#include <iostream>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
void
Prefs::dialog()
{
    QDialog dlg;
    QCheckBox *chkSound = new QCheckBox();
    chkSound->setChecked(_sound);
    QObject::connect(chkSound, SIGNAL(stateChanged(int)), this, SLOT(setSound(int)));

    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->addRow("音を鳴らす", chkSound);

    QLabel *label = new QLabel("テーマ");
    QRadioButton *light = new QRadioButton("ライト");
    QRadioButton *dark  = new QRadioButton("ダーク");
    QRadioButton *sys   = new QRadioButton("システm");
    QRadioButton *defchk = sys;
    switch(_theme_type)
    {
    case Light: defchk = light; break;
    case Dark:  defchk = dark; break;
    }
    defchk->setChecked(true);

    QButtonGroup *group = new QButtonGroup();
    group->addButton(light, 0);
    group->addButton(dark, 1);
    group->addButton(sys, 2);
    QObject::connect(group, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int id)
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
    vbox->addLayout(form);
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
    std::ifstream fi(_f);
    if(fi.fail()) return;
    while(!fi.eof())
    {
        std::string key, value;
        fi >> key >> value;
//        std::cerr << "(key, value) = (" << key << "," << value << ")" << std::endl;
        if (key == "sound:")
        {
            setSound(value == "true");
        }
        if (key == "theme:")
        {
            ThemeType t = System;
            if (value == "light") t = Light;
            if (value == "dark") t = Dark;
            setThemeType(t);
        }
    }
}

void
Prefs::save() const
{
    std::ofstream fo(_f);
    if (fo.fail()) return;
    fo << "sound: " << ((getSound()) ? "true" : "false") << std::endl;
    const std::string theme_type[] = { "light", "dark", "system" };
    fo << "theme:" << theme_type[_theme_type] << std::endl;
}
