#ifndef ZSYSTEM_H
#define ZSYSTEM_H

#include <QMainWindow>
#include <QGraphicsScene>>
#include <QPixmap>
#include <QScrollBar>
#include <QtMultimedia/QMediaPlayer>
#include <graph.h>
#include <zmap.h>
#include <zwords.h>
#include <zrule.h>
#include <zuserdata.h>
#include <zobjdata.h>
#include <files.h>
#include <dialog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE
namespace Ui { class ZSystem; }
QT_END_NAMESPACE

class ZSystem : public QMainWindow
{
    Q_OBJECT

public:
    ZSystem(QWidget *parent = nullptr);
    ~ZSystem();

    bool loadDict(const std::string &file);
    bool loadMap(const std::string &file);
    bool loadObjs(const std::string &file);
    bool loadUser(const std::string &file);
    bool loadMsgs(const std::string &file);
    bool loadRules(const std::string &file);

    void load_game(int f);
    void save_game(int f);
    void playing(void) { setMode(Play); }
    void game_over(void);
    void time_elapsed(void);
    void check_teacher(void);
    bool check_darkness(void);
    void draw_screen(bool);
    void prompt(void) const;
    void interpreter(void);
    void loop(SDL_Event &ev);
    void play_media(int n) const;
    const Files *files() const { return _files; }
public slots:
    void parse();
    void title();
    void start();
private:
    Ui::ZSystem *ui;
    QGraphicsScene _scene;

    Canvas *_cv;
    ZCore *_core;
    ZWords *_dict;
    ZMapRoot *_zmap;
    ZObjRoot *_obj;
    ZUserData *_user;
    ZMessage *_msg;
    ZRules *_rules;
    Files *_files;

    static const std::string _credit[];
    enum Mode {
        Title = 0,
        Play,
        GameOver,
    } _mode;

    Mode getMode(void) const { return _mode; }
    void setMode(Mode m) { _mode = m; }
    void dialog(uint8_t id);

    void paintEvent(QPaintEvent *ev) override;
//    void keyReleaseEvent(QKeyEvent *ev) override;
};
#endif // ZSYSTEM_H
