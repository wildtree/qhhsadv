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

enum ThemeType {
    Light = 0, Dark =  1, System = 2,
};

class Prefs : public QObject
{
    Q_OBJECT

public:
    Prefs() : _sound(true), _theme_type(System) {}
    ~Prefs() {}

    void setPrefsFilename(std::string f) { _f = f; }
    void load();
    void save() const;

    bool getSound() const { return _sound; }
    ThemeType getThemeType() const { return _theme_type; }
public slots:
    void dialog();
    void setSound(int f) { _sound = (f != 0); }
    void setThemeType(ThemeType t) { _theme_type = t;}
protected:
    std::string _f;
    bool _sound;
    ThemeType _theme_type;
};

#endif // PREFS_H
