#include "zsystem.h"
#include "./ui_zsystem.h"

const std::string ZSystem::_credit[] = {
    "ハイハイスクールアドベンチャー",
    "Copyright(c)1995-2024",
    "ZOBplus",
    "hiro"
};

ZSystem::ZSystem(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZSystem)
    , _core(nullptr), _dict(nullptr), _zmap(nullptr), _obj(nullptr), _user(nullptr), _msg(nullptr), _rules(nullptr)
{
    ui->setupUi(this);
    _cv = new Canvas(0, 0, ui->_gv->width(), ui->_gv->height());
    _files = new Files();
    _files->base_dir(getenv("HOME"));
    _core = new ZCore();

    ui->_gv->setScene(&_scene);
    QObject::connect(ui->_le, SIGNAL(editingFinished()), this, SLOT(parse()));
    QObject::connect(ui->_btnStart, SIGNAL(clicked()), this, SLOT(start()));
    QObject::connect(ui->_btnTitle, SIGNAL(clicked()), this, SLOT(title()));
}

ZSystem::~ZSystem()
{
    if (_dict) delete _dict;
    if (_zmap) delete _zmap;
    if (_obj) delete _obj;
    if (_user) delete _user;
    if (_msg) delete _msg;
    if (_rules) delete _rules;

    delete _core;
    delete _files;
    delete _cv;
    delete ui;
}

bool
ZSystem::loadDict(const std::string &file)
{
    _dict = new ZWords(file);
    return _dict != nullptr;
}

bool
ZSystem::loadMap(const std::string &file)
{
    _zmap = new ZMapRoot(file);
    return _zmap != nullptr;
}

bool
ZSystem::loadObjs(const std::string &file)
{
    _obj = new ZObjRoot(file);
    return _obj != nullptr;
}

bool
ZSystem::loadUser(const std::string &file)
{
    _user = new ZUserData(file);
    return _user != nullptr;
}

bool
ZSystem::loadMsgs(const std::string &file)
{
    _msg = new ZMessage(file);
    return _msg != nullptr;
}

bool
ZSystem::loadRules(const std::string &file)
{
    _rules = new ZRules(file);
    return _msg != nullptr;
}

void
ZSystem::paintEvent(QPaintEvent *ev)
{
    _scene.addPixmap(QPixmap::fromImage(*(_cv->image())));
    ui->_gv->invalidateScene(ev->rect());
}

void
ZSystem::parse()
{
    QString cmd_line = ui->_le->text();
    // empty check is necessary because ui->_le->clear() looks invoking SIGNAL(ediingFinished()) with empty string.
    if (cmd_line.isEmpty()) return; // no need to parse
    ui->_le->clear();

    ui->_mv->appendPlainText(">>> " + cmd_line);
    std::string l = cmd_line.toStdString();
    int i = l.find(' ');
    _core->cmdId(_dict->findVerb(l.substr(0, i)));
    if (i < 0)
    {
        _core->objId(-1);
    }
    else
    {
        while(i < l.length() && l[i] == ' ') i++;
        if (i < l.length())
        {
            _core->objId(_dict->findObjs(l.substr(i)));
        }
        else
        {
            _core->objId(-1);
        }
    }
    // interpret
    time_elapsed();
    if (_mode == GameOver) return;
    interpreter();
    if (_mode == GameOver) return;
    check_teacher();
    draw_screen(true);
}

void
ZSystem::title()
{
    _cv->paint(0,0,Canvas::BLUE,Canvas::BLUE);
    _zmap->setCursor(76);
    _zmap->curMapData().draw(_cv);
    _cv->invalidate();
    ui->_mv->clear();
    for (int i = 0 ; i < 4 ; i++)
    {
        ui->_mv->appendPlainText(QString(_credit[i].c_str()));
    }
    ui->_le->hide();
    ui->_prompt->hide();
    ui->_btnTitle->hide();
    ui->_btnStart->show();
    ui->centralwidget->update();

    _mode = Title;
}

void ZSystem::game_over(void)
{
    ui->_le->hide();
    ui->_prompt->hide();
    ui->_btnTitle->show();
    ui->_btnStart->hide();

    _mode = GameOver;
}

void
ZSystem::time_elapsed(void)
{
  // battery
  if (_user->getFact(3) > 0 && _user->getFact(7) == 1)
  {
    // light is ON
    _user->decFact(3); // consume battery life
    if (_user->getFact(3) < 8 && _user->getFact(3) > 0)
    {
      // low battery
      _user->setFact(6,1); // dim mode
      ui->_mv->appendPlainText(QString(_msg->getMessage(0xd9).c_str()));
    }
    if (_user->getFact(3) == 0)
    {
      // battery ware out
      _user->setFact(7, 0); // light off
      ui->_mv->appendPlainText(QString(_msg->getMessage(0xc0).c_str()));
    }
  }
  // dynamite
  if (_user->getFact(11) > 0)
  {
    if (_user->decFact(11) == 0)
    {
      ui->_mv->appendPlainText(QString(_msg->getMessage(0xd8).c_str()));
      if (_user->getPlace(7) == 48)
      {
        _user->getMap(75 - 1).setLink(ZMapLink::North, 77);
        _user->getMap(68-  1).setLink(ZMapLink::West, 77);
        ui->_mv->appendPlainText(QString(_msg->getMessage(0xda).c_str()));
      }
      else if (_user->getPlace(7) == 255 || _user->getPlace(7) == _zmap->getCursor())
      {
        // screen to red.
        ui->_mv->appendPlainText(QString(_msg->getMessage(0xcf).c_str()));
        ui->_mv->appendPlainText(QString(_msg->getMessage(0xcb).c_str()));
        game_over();
      }
      else
      {
        _user->setPlace(7,0); // lost dynamite in somewhere else
      }
    }
  }
}

void
ZSystem::check_teacher(void)
{
  if (_mode == GameOver || _user->getFact(1) == _core->mapId()) return;
  int rd = 100 + _core->mapId() + ((_user->getFact(1) > 0) ? 1000 : 0);
  int rz = random() / 715827; // [0,3000) random number
  _user->setFact(1, (rd < rz) ? 0 : _core->mapId());
  switch (_core->mapId())
  {
    case 1:
    case 48:
    case 50:
    case 51:
    case 52:
    case 53:
    case 61:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 83:
    case 86:
      _user->setFact(1,0);
      break;
  }
}

bool
ZSystem::check_darkness(void)
{
    bool dim = false;
    switch (_core->mapId())
    {
        case 47:
        case 48:
        case 49:
        case 61:
        case 64:
        case 65:
        case 67:
        case 68:
        case 69:
        case 71:
        case 74:
        case 75:
        case 77:
            if (_user->getFact(7) != 0)
            {
                if (_user->getFact(6) != 0)
                {
                    // dark mode. (color in blue)
                    _cv->setColorFilter(Canvas::blueFilter);
                    dim = true;
                }
            }
            else
            {
                // black-out
                _core->mapView(_core->mapId());
                _core->mapId(84);
                _zmap->setCursor(84);
            }
            break;
        default:
            if (_user->getFact(6) != 0)
            {
                // back to normal (remove color filter)
                _cv->resetColorFilter();
            }
            break;
    }
    return dim;
}

void
ZSystem::draw_screen(bool with_msg)
{
    check_darkness();
    _zmap->curMapData().draw(_cv);
    _cv->colorFilter();
    _cv->invalidate();
    if (with_msg && _mode != GameOver)
    {
        if (_zmap->curMapData().isBlank())
        {
            ui->_mv->appendPlainText(QString(_msg->getMessage(0xcc).c_str()));
        }
        else
        {
            ui->_mv->appendPlainText(QString(_zmap->curMapData().getMessage().c_str()));
        }
    }
    for (int i = 0 ; i < ZUserData::items ; i++)
    {
        if (_user->getPlace(i) == _zmap->getCursor())
        {
            if (i == 1 && _user->getFact(0) != 1)
            {
                _obj->draw(_cv, i + 1, true);
            }
            else
            {
                _obj->draw(_cv, i + 1);
            }
            if (with_msg && _mode != GameOver)
            {
                ui->_mv->appendPlainText(QString(_msg->getMessage(0x96 + i).c_str()));
            }
        }

    }
    if (_user->getFact(1) == _zmap->getCursor())
    {
        _obj->draw(_cv, 14); // draw teacher
        if (with_msg && _mode != GameOver)
        {
            ui->_mv->appendPlainText(QString(_msg->getMessage(0xb4).c_str()));
        }
    }
    ui->centralwidget->update();
}

void
ZSystem::save_game(int f)
{
    const std::string filename = files()->user_file(f);
    FILE *fp = fopen(filename.c_str(), "w");
    fwrite(_core->pack(), 1, ZCore::packed_size, fp);
    fwrite(_user->pack(), 1, ZUserData::packed_size, fp);
    fclose(fp);
}

void
ZSystem::load_game(int f)
{
    const std::string filename = files()->user_file(f);
    uint8_t *buf = new uint8_t [std::max(ZCore::packed_size, ZUserData::packed_size)];
    FILE *fp = fopen(filename.c_str(), "r");
    if (fread(buf, 1, ZCore::packed_size, fp) > 0)
    {
        _core->unpack(buf);
    }
    if (fread(buf, 1, ZUserData::packed_size, fp) > 0)
    {
        _user->unpack(buf);
    }
    fclose(fp);
    delete[] buf;
}

void
ZSystem::play_media(int n) const
{
    std::string sound_names[] = {
        "highschool", "charumera", "explosion", "in_toilet", "acid",
    };
    if (n < 0 || n > 4) return;
    struct stat ss;
    if (stat(files()->mp3_file(sound_names[n]).c_str(), &ss) == 0 && S_ISREG(ss.st_mode))
    {
        auto mp = new QMediaPlayer();
        mp->setMedia(QUrl::fromLocalFile(files()->mp3_file(sound_names[n]).c_str()));
        mp->setVolume(100);
        mp->play();
        mp->deleteLater();
    }
}
void
ZSystem::dialog(uint8_t id)
{
    Dialog *dialog = new Dialog(this);
    int r;
    switch(id)
    {
        case 0: // boy or girl?
            _user->setFact(0, 1); // default (boy)
            dialog->setTitle("男子/女子");
            dialog->setMessage(_msg->getMessage(0xe7));
            dialog->button("男子","女子",std::string());
            _user->setFact(0, dialog->draw());
            _core->mapId(3);
            _zmap->setCursor(3);
            break;
        case 1: // file select
            dialog->setTitle("ファイルを選んでください。");
            dialog->setMessage(_msg->getMessage(0xe8));
            dialog->button("1","2","3");
            if (_core->cmdId() != 0x0f)
            {
                // load game
                bool file_exists = false;
                struct stat ss;
                if (stat(files()->user_file(1).c_str(), &ss) == 0 && S_ISREG(ss.st_mode))
                {
                    file_exists = true;
                }
                else
                {
                    dialog->btnA()->hide();
                }
                if (stat(files()->user_file(2).c_str(), &ss) == 0 && S_ISREG(ss.st_mode))
                {
                    file_exists = true;
                }
                else
                {
                    dialog->btnB()->hide();
                }
                if (stat(files()->user_file(3).c_str(), &ss) == 0 && S_ISREG(ss.st_mode))
                {
                    file_exists = true;
                }
                else
                {
                    dialog->btnC()->hide();
                }
                if (!file_exists)
                {
                    dialog->setTitle("ファイルがありません");
                    dialog->setMessage("セーブデータが存在しません。");
                    dialog->button(std::string(), "了解", std::string());
                    dialog->btnB()->show();
                    dialog->draw();
                    break;
                }
            }
            r = dialog->draw();
            if (_core->cmdId() == 0x0f)
            {
                // save
                save_game(r);
            }
            else
            {
                // load
                load_game(r);
            }
            break;
        case 2: // show items
            dialog->setTitle("持ち物");
            // 持ち物リストを作成
            dialog->setMessage(_user->itemList());
            dialog->button(std::string(), "OK", std::string());
            dialog->draw();
            break;
        case 3: // cut calbe (yellow or red)
            dialog->setTitle("どちらを切りますか？");
            dialog->setMessage(_msg->getMessage(0xe9));
            dialog->button("黄","赤", std::string());
            r = dialog->draw();
            if (_user->getPlace(11) != 0xff)
            {
                ui->_mv->appendPlainText(QString(_msg->getMessage(0xe0).c_str()));
            }
            if (r == 1 || _user->getPlace(11) != 0xff)
            {
                // gameover
                _cv->setColorFilter(Canvas::redFilter);
                draw_screen(false);
                ui->_mv->appendPlainText(QString(_msg->getMessage(0xc7).c_str()));
                ui->_mv->appendPlainText(QString(_msg->getMessage(0xee).c_str()));
                game_over();
            }
            else
            {
                // gameclear
                _user->setPlace(11,0);
                _core->mapId(74);
                _zmap->setCursor(74);
                // play sound #3
                play_media(3);
            }
            break;
        default:
            break;
    }
    delete dialog;
    QScrollBar *vs = ui->_mv->verticalScrollBar();
    vs->setValue(vs->maximum());
}

void
ZSystem::interpreter(void)
{
    bool ok = false;
    for (ZRulesIterator i = _rules->begin() ; i != _rules->end() ; i++)
    {
        // Serial.printf("%03d: rule check\r\n", j++);
        if ((*i).run(_core, _user))
        {
            _zmap->setCursor(_core->mapId());
            while (!_core->is_empty())
            {
                ZCommand c = _core->pop();
                uint8_t op = c.getOperand();
                switch (c.getCommand())
                {
                    case ZCommand::Nop:
                        break;
                    case ZCommand::Message:
                        ui->_mv->appendPlainText(QString(QString((op & 0x80) ? _msg->getMessage(op).c_str() : _zmap->curMapData().find(_core->cmdId(), _core->objId()).c_str())));
                        break;
                    case ZCommand::Sound:
                        play_media(op);
                        break;
                    case ZCommand::Dialogue:
                        dialog(op);
                        break;
                    case ZCommand::GameOver:
                        switch(op)
                        {
                            case 1: // arrested by the teacher
                                _cv->setColorFilter(Canvas::sepiaFilter);
                                draw_screen(false);
                                break;
                            case 2: // died due to invloved the explosion
                                _cv->setColorFilter(Canvas::redFilter);
                                draw_screen(false);
                                break;
                            case 3: // game clear
                                break;
                            default:
                                break;
                        }
                        game_over();
                        break;
                    default:
                        break;
                }

            }
            if (_mode == GameOver) return;
            ui->_mv->appendPlainText(QString(_msg->getMessage(0xed).c_str())); // O.K.
            ok = true;
            break;
        }
    }
    _zmap->setCursor(_core->mapId());
    if (!ok)
    {
        std::string m = _zmap->curMapData().find(_core->cmdId(), _core->objId());
        if (m.empty())
        {
            m = _msg->getMessage(0xec); // "ダメ";
        }
        ui->_mv->appendPlainText(QString(m.c_str()));
    }
    if (_zmap->getCursor() == 74)
    {
        int m_id = 0;
        switch(_user->incFact(13))
        {
            case 4:  m_id = 0xe2; break;
            case 6:  m_id = 0xe3; break;
            case 10: m_id = 0xe4; break;
        }
        if (m_id != 0)
        {
            ui->_mv->appendPlainText(QString(_msg->getMessage(m_id).c_str()));
        }
    }
}

void
ZSystem::start()
{
    ui->_le->show();
    ui->_prompt->show();
    ui->_btnTitle->hide();
    ui->_btnStart->hide();
    ui->_mv->clear();
    QScrollBar *vs = ui->_mv->verticalScrollBar();
    vs->setValue(vs->maximum());
    ui->_le->setFocus();
    _user->init(files()->init_file());
    _cv->resetColorFilter();
    _zmap->setCursor(1);
    _core->mapId(1);
    draw_screen(true);
    _mode = Play;
}
