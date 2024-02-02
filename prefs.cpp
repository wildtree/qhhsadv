
#include <prefs.h>
#include <iostream>
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

    QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok);
    bbox->button(QDialogButtonBox::Ok)->setText("了解");

    QVBoxLayout *vbox = new QVBoxLayout(&dlg);
    vbox->addLayout(form);
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
    }
}

void
Prefs::save() const
{
    std::ofstream fo(_f);
    if (fo.fail()) return;
    fo << "sound: " << ((getSound()) ? "true" : "false") << std::endl;
}
