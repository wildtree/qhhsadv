//
// files
//

#include <string>
#include <sys/stat.h>

#if !defined(FILES_H)
#define FILES_H

class Files
{
protected:
    std::string _base_dir;
public:
    Files() {}
    ~Files() {}

    void base_dir(std::string dir) { _base_dir = dir; }
    std::string base_dir() const { return _base_dir; }

    std::string map_file() const { return base_dir() + "/.HHSAdv/map.dat"; }
    std::string dict_file() const { return base_dir() + "/.HHSAdv/highds.com"; }
    std::string obj_file() const { return base_dir() + "/.HHSAdv/thin.dat"; }
    std::string rule_file() const { return base_dir() + "/.HHSAdv/rule.dat"; }
    std::string init_file() const { return base_dir() + "/.HHSAdv/data.dat"; }
    std::string mesg_file() const { return base_dir() + "/.HHSAdv/msg.dat"; }
    std::string user_file(int i) const { return base_dir() + "/.HHSAdv/" + std::to_string(i) + ".dat"; }
    std::string mp3_file(const std::string &s) const { return base_dir() + "/.HHSAdv/" + s + ".mp3"; }
    std::string icon_file() const { return base_dir() + "/.HHSAdv/icon.png"; }
    std::string prefs_file() const { return base_dir() + "/.HHSAdv/prefs.dat"; }

    std::string font_file() const { return "/usr/share/fonts/truetype/fonts-japanese-gothic.ttf";}

    bool exists(std::string f) const { struct stat ss; return stat(f.c_str(), &ss) == 0 && S_ISREG(ss.st_mode); }
};

#endif
