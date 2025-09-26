//
// files
//

#include <QCoreApplication>
#include <string>
#include <sys/stat.h>

#if !defined(FILES_H)
#define FILES_H

#include <QFileInfo>
#include <QDir>

class Files
{
protected:
    std::string _base_dir;
public:
    Files() {}
    ~Files() {}

    void base_dir(std::string dir) { _base_dir = dir; }
    std::string base_dir() const { return _base_dir; }

    std::string map_file() const { return base_dir() + "/map.dat"; }
    std::string dict_file() const { return base_dir() + "/highds.com"; }
    std::string obj_file() const { return base_dir() + "/thin.dat"; }
    std::string rule_file() const { return base_dir() + "/rule.dat"; }
    std::string init_file() const { return base_dir() + "/data.dat"; }
    std::string mesg_file() const { return base_dir() + "/msg.dat"; }
    std::string user_file(int i) const { return base_dir() + "/" + std::to_string(i) + ".dat"; }
    std::string mp3_file(const std::string &s) const { return base_dir() + "/" + s + ".mp3"; }
    std::string icon_file() const { return base_dir() + "/icon.png"; }
    std::string prefs_file() const { return base_dir() + "/prefs.json"; }

    std::string font_file() const { return "/usr/share/fonts/truetype/fonts-japanese-gothic.ttf";}

    std::string theme_file(bool dark) const { return QCoreApplication::applicationDirPath().toStdString() + "/data" + (dark ? "/themes/dark.qss" : "/themes/light.qss");}
    std::string user_theme_file(bool dark) const { return dark ? base_dir() + "/themes/user_dark.qss" : base_dir() + "/themes/user_light.qss";}

    bool exists(std::string f) const { QFileInfo finfo(f.c_str()); return finfo.exists() && finfo.isFile(); }

    bool copyData(const std::string &src);
private:
    bool copyRecursively(const QString &srcPath, const QString &dstPath);
    int getVersion(const QString &dir);
};

#endif
