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
#include <functional>

enum class ThemeType {
    Light = 0, Dark =  1, System = 2,
};

class Prefs : public QObject
{
    Q_OBJECT

public:
    Prefs() : _sound(true), _skip_opening(false), _theme_type(ThemeType::System) {}
    ~Prefs() {}

    void setPrefsFilename(std::string f) { _f = f; }
    void load();
    void save() const;

    bool getSound() const { return _sound; }
    bool getSkipOpening() const { return _skip_opening; }
    ThemeType getThemeType() const { return _theme_type; }
    void setThemeHandler(std::function<void(ThemeType)> f) { _themeHandler = f; };
public slots:
    void dialog();
    void setSound(int f) { _sound = (f != 0); }
    void setSkipOpening(int f) { _skip_opening = (f != 0); }
    void setThemeType(int id){
        switch(id)
        {
        case 0: setThemeType(ThemeType::Light); break;
        case 1: setThemeType(ThemeType::Dark); break;
        default: setThemeType(ThemeType::System); break;
        }
    }
    void setThemeType(ThemeType t) { _theme_type = t; _themeHandler(_theme_type); }
protected:
    std::string _f;
    bool _sound;
    bool _skip_opening;
    ThemeType _theme_type;
    std::function<void(ThemeType)> _themeHandler;
};

#endif // PREFS_H
