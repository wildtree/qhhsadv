#ifndef PREFS_H
#define PREFS_H

#include <QObject>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <fstream>

class Prefs : public QObject
{
    Q_OBJECT

public:
    Prefs() : _sound(true) {}
    ~Prefs() {}

    void setPrefsFilename(std::string f) { _f = f; }
    void load();
    void save() const;

    bool getSound() const { return _sound; }

public slots:
    void dialog();
    void setSound(int f) { _sound = (f != 0); }
protected:
    std::string _f;
    bool _sound;
};

#endif // PREFS_H
