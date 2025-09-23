#include "zsystem.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ZSystem w;

    w.loadDict(w.files()->dict_file());
    w.loadMap(w.files()->map_file());
    w.loadObjs(w.files()->obj_file());
    w.loadUser(w.files()->init_file());
    w.loadRules(w.files()->rule_file());
    w.loadMsgs(w.files()->mesg_file());

    w.title();
    w.show();
    return a.exec();
}
