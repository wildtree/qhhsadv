#include "zsystem.h"
#include "./ui_zsystem.h"
#include <QFile>
#include <qstylehints.h>
#if QT_VERSION >=QT_VERSION_CHECK(6,5,0)
#include <QStyleHints>
#endif
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QDir>
#include "ZCreditRoll.h"

const std::string ZSystem::_credit[] = {
    "ハイハイスクールアドベンチャー",
    "Copyright(c)1995-2025",
    "ZOBplus",
    "hiro"
};

const QString opening_credit = R"(<center><h3>ストーリー</h3></center>
                               <font size="2">
                               <p>2019年神奈山県立ハイ高等学校は地盤が弱く校舎の老朽化も進んだため、とうとう廃校にする以外方法がなくなってしまった。</p>
                               <p>ところで大変な情報を手に入れた。<br/>
                               それは、<br/><center>「ハイ高校にＡＴＯＭＩＣ ＢＯＭＢが仕掛けられている。」</center><br/>と、いうものだ。</p>
                               <p>どうやらハイ高が廃校になった時、気が狂った理科の先生がＡＴＯＭＩＣ ＢＯＭＢを、学校のどこかに仕掛けてしまったらしい。</p>
                               <p>お願いだ。我が母校のコナゴナになった姿を見たくはない。<br/>
                               早くＡＴＯＭＩＣ ＢＯＭＢを取り除いてくれ……！！</p>
                               <p>行動は英語で、&quot;&lt;動詞&gt;&quot;或いは、&quot;&lt;動詞&gt;&quot;+&quot;&lt;目的語&gt;&quot;のように入れていただきたい。<br/>
                               例えば、&quot;look room&quot;と入れれば部屋の様子を見ることが出来るという訳だ。</p>
                               <p>それでは Ｇｏｏｄ Ｌｕｃｋ！！！............</p></font>)";

const QString ending_credit = R"(<<center><font size="2"><b>High High School Adventure</b>
<br />
PalmOS version: hiro © 2002-2004<br />
Android version: hiro © 2011-2025<br />
M5 version: hiro © 2023-2025<br />
Qt version: hiro © 2024<br />
PicoCalc version: hiro © 2025<br />
SDL version: hiro © 2025<br />
Windows version: hiro © 2025<br />
<br />
<b>- Project ZOBPlus -</b><br />
Hayami &lt;hayami@zob.jp&gt;<br />
Exit &lt;exit@zob.jp&gt;<br />
ezumi &lt;ezumi@zob.jp&gt;<br />
Ogu &lt;ogu@zob.jp&gt;<br />
neopara &lt;neopara@zob.jp&gt;<br />
hiro &lt;hiro@zob.jp&gt;<br />
<br />
<b>--- Original Staff ---</b><br />
<br />
<b>- Director -</b><br />
HIRONOBU NAKAGUCHI<br />
<br />
<b>- Graphic Designers -</b><br />
<br />
NOBUKO YANAGITA<br />
YUMIKO HOSONO<br />
HIRONOBU NAKAGUCHI<br />
TOSHIHIKO YANAGITA<br />
TOHRU OHYAMA<br />
<br />
MASANORI ISHII<br />
YASUSHI SHIGEHARA<br />
HIDETOSHI SUZUKI<br />
TATSUYA UCHIBORI<br />
MASAKI NOZAWA<br />
<br />
TOMOKO OHKAWA<br />
FUMIKAZU SHIRATSUCHI<br />
YASUNORI YAMADA<br />
MUNENORI TAKIMOTO<br />
<br />
<b>- Message Converters -</b><br />
TATSUYA UCHIBORI<br />
HIDETOSHI SUZUKI<br />
YASUSHI SHIGEHARA<br />
YASUNORI YAMADA<br />
<br />
<b>- Floppy Disk Converters -</b><br />
HIRONOBU NAKAGUCHI<br />
<br />
<b>- Music -</b><br />
MASAO MIZOBE<br />
<br />
<b>- Special Thanks To -</b><br />
HIROSHI YAMAMOTO<br />
TAKAYOSHI KASHIWAGI<br />
<br />
<b>- Cooperate with -</b><br />
Furniture KASHIWAGI<br />
<br />
ZAMA HIGH SCHOOL MICRO COMPUTER CIRCLE</font></center>)";

ZSystem::ZSystem(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZSystem)
    , _pi(nullptr), _core(nullptr), _dict(nullptr), _zmap(nullptr), _obj(nullptr), _user(nullptr), _msg(nullptr), _rules(nullptr)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("WildTreeJP");
    QCoreApplication::setApplicationName("QHHSAdv");

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    _pi = _scene.addPixmap(QPixmap(gwidth, gheight));
    _cv = new Canvas(0, 0, gwidth, gheight);
    _files = new Files();
    _files->base_dir(path.toStdString());
    _files->copyData(QDir::current().filePath("data").toStdString());
    _core = new ZCore();
    _prefs = new Prefs();
    _prefs->setFontSizeHandler([this](int size){
        QFont f = ui->_mv->font();
        f.setPointSize(size);
        ui->_mv->setFont(f) ;
    });
    _prefs->setThemeHandler([this](ThemeType theme){
        bool dark = false;
        if (theme == ThemeType::System)
        {
#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
            dark = qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#endif
        }
        else
        {
            dark = (theme == ThemeType::Dark);
        }
        this->applyTheme(dark);
    });

    _prefs->setPrefsFilename(_files->prefs_file());
    if (_files->exists(_files->prefs_file())) _prefs->load();
    if(_files->exists(_files->icon_file()))
    {
        this->setWindowIcon(QIcon(QPixmap(_files->icon_file().c_str())));
    }
    _scene.setSceneRect(0, 0, gwidth, gheight);
    ui->_gv->setScene(&_scene);
    ui->_gv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->_gv->setAlignment(Qt::AlignCenter);
    QObject::connect(ui->_le, SIGNAL(editingFinished()), this, SLOT(parse()));
    QObject::connect(ui->_btnStart, SIGNAL(clicked()), this, SLOT(start()));
    QObject::connect(ui->_btnTitle, SIGNAL(clicked()), this, SLOT(title()));

    QObject::connect(ui->action_Q, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->action_2, SIGNAL(triggered()), this, SLOT(help()));
    QObject::connect(ui->action_4, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->action_P, SIGNAL(triggered()), _prefs, SLOT(dialog()));

    ui->_btnStart->setMinimumHeight(48);
    ui->_btnTitle->setMinimumHeight(48);
    ui->_prompt->setMinimumHeight(48);
}

ZSystem::~ZSystem()
{
    if (_dict) delete _dict;
    if (_zmap) delete _zmap;
    if (_obj) delete _obj;
    if (_user) delete _user;
    if (_msg) delete _msg;
    if (_rules) delete _rules;

    delete _prefs;
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
    //_scene.addPixmap(QPixmap::fromImage(*(_cv->image())));
    _pi->setPixmap(QPixmap::fromImage(*(_cv->image())));
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
    size_t i = l.find(' ');
    _core->cmdId(_dict->findVerb(l.substr(0, i)));
    if (i == std::string::npos)
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
  //int rz = random() / 715827; // [0,3000) random number
  int rz = QRandomGenerator::global()->bounded(3000); // [0,3000) random number
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
    QFile fo(filename.c_str());
    if (fo.open(QIODevice::WriteOnly))
    {
        fo.write(reinterpret_cast<const char*>(_core->pack()), ZCore::packed_size);
        fo.write(reinterpret_cast<const char*>(_user->pack()), ZUserData::packed_size);
        fo.close();
    }
}

void
ZSystem::load_game(int f)
{
    const std::string filename = files()->user_file(f);
    QFile fi(filename.c_str());
    if (fi.open(QIODevice::ReadOnly))
    {
        uint8_t *buf = new uint8_t [std::max(ZCore::packed_size, ZUserData::packed_size)];
        fi.read(reinterpret_cast<char*>(buf), ZCore::packed_size);
        _core->unpack(buf);
        fi.read(reinterpret_cast<char*>(buf), ZUserData::packed_size);
        _user->unpack(buf);
        fi.close();
        delete[] buf;
    }
}

void
ZSystem::play_media(int n)
{
    std::string sound_names[] = {
        "highschool", "charumera", "explosion", "",  "in_toilet", "acid",
    };
    if (!_prefs->getSound()) return;
    if (n < 0 || n > 5) return;
    QFileInfo qfinfo(files()->mp3_file(sound_names[n]).c_str());
    if (qfinfo.exists() && qfinfo.isFile())
    {
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        _mp.setMedia(QUrl::fromLocalFile(files()->mp3_file(sound_names[n]).c_str()));
        _mp.setVolume(100);
        _mp.play();
#else
        _mp.setAudioOutput(&_audio);
        _mp.setSource(QUrl::fromLocalFile(files()->mp3_file(sound_names[n]).c_str()));
        _audio.setVolume(100);
        _mp.play();
#endif
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
                QFileInfo qfinfo1(files()->user_file(1).c_str()), qfinfo2(files()->user_file(2).c_str()), qfinfo3(files()->user_file(3).c_str());
                bool file_exists = false;
                if (qfinfo1.exists() && qfinfo1.isFile())
                {
                    file_exists = true;
                }
                else
                {
                    dialog->btnA()->hide();
                }
                if (qfinfo2.exists() && qfinfo2.isFile())
                {
                    file_exists = true;
                }
                else
                {
                    dialog->btnB()->hide();
                }
                if (qfinfo3.exists() && qfinfo3.isFile())
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
                                play_media(0);
                                {
                                    ZCreditRoll *cr = new ZCreditRoll(this, ending_credit);
                                    cr->startRoll(35000, this);
                                }
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
    if (!_prefs->getSkipOpening())
    {
        ZCreditRoll *cr = new ZCreditRoll(this,opening_credit);
        cr->startRoll(20000, this);
    }
    //
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

void
ZSystem::about()
{
    ScrollMessageBox::about(this, "このゲームについて",
                           "<h3>ハイハイスクールアドベンチャー</h3>"
                           "<p>"
                           "&nbsp;PalmOS version: hiro © 2002-2004<br/>"
                           "&nbsp;Android version: hiro © 2011-2025<br/>"
                           "&nbsp;M5 version: hiro © 2023-2024<br/>"
                           "&nbsp;Qt version: hiro © 2024-2025<br/>"
                            "&nbsp;PicoCalc version: hiro © 2025<br />"
                            "&nbsp;SDL version: hiro © 2025<br />"
                            "&nbsp;Windows version: hiro © 2025<br />"
                           "</p><p>"
                           "<b>Project ZOBPlus</b><br/>"
                           "&nbsp;Hayami &lt;hayami@zob.jp&gt;<br/>"
                           "&nbsp;Exit &lt;exit@zob.jp&gt;<br/>"
                           "&nbsp;ezumi &lt;ezumi@zob.jp&gt;<br/>"
                           "&nbsp;Ogu &lt;ogu@zob.jp&gt;<br/>"
                           "&nbsp;neopara &lt;neopara@zob.jp&gt;<br/>"
                           "&nbsp;hiro &lt;hiro@zob.jp&gt;<br/>"
                           "</p><p>"
                           "<b>--- Original Staff ---</b><br/>"
                           "<b>Directed By HIRONOBU NAKAGUCHI</b><br/>"
                           "<b>Graphic Designers:</b><br/>"
                           "<table boder=0><tr>"
                           "<td>NOBUKO YANAGITA</td>"
                           "<td>YUMIKO HOSONO</td>"
                           "<td>HIRONOBU NAKAGUCHI</td>"
                           "<td>TOSHIHIKO YANAGITA</td>"
                           "<td>TOHRU OHYAMA</td>"
                           "</tr><tr>"
                           "<td>MASANORI ISHII</td>"
                           "<td>YASUSHI SHIGEHARA</td>"
                           "<td>HIDETOSHI SUZUKI</td>"
                           "<td>TATSUYA UCHIBORI</td>"
                           "<td>MASAKI NOZAWA</td>"
                           "</tr><tr>"
                           "<td>TOMOKO OHKAWA</td>"
                           "<td>FUMIKAZU SHIRATSUCHI</td>"
                           "<td>YASUNORI YAMADA</td>"
                           "<td>MUNENORI TAKIMOTO</td>"
                           "<td/>"
                           "</tr></table>"
                           "<b>Message Converters:</b><br/>"
                           "<table boder=0><tr>"
                           "<td>TATSUYA UCHIBORI</td>"
                           "<td>HIDETOSHI SUZUKI</td>"
                           "<td>YASUSHI SHIGEHARA</td>"
                           "<td>YASUNORI YAMADA</td>"
                           "</tr></table>"
                           "<b>Floppy Disk Converters:</b><br/>"
                           "&nbsp;HIRONOBU NAKAGUCHI<br/>"
                           "<b>Music:</b><br/>"
                           "&nbsp;MASAO MIZOBE<br/>"
                           "<b>Special Thanks To:</b><br/>"
                           "&nbsp;HIROSHI YAMAMOTO<br/>"
                           "&nbsp;TAKAYOSHI KASHIWAGI<br/>"
                           "<b>Cooperate with:</b><br/>"
                           "&nbsp;Furniture KASHIWAGI<br/>"
                           "<h4>ZAMA HIGH SCHOOL MICRO COMPUTER CIRCLE</h4>"
                           );
}

void
ZSystem::help()
{

    QMessageBox::about(this, "ストーリー",
                "<p>2019年神奈山県立ハイ高等学校は地盤が弱く校舎の老朽化も進んだため、とうとう廃校にする以外方法がなくなってしまった。</p>"
                "<p>ところで大変な情報を手に入れた。<br/>"
                "それは、<br/><center>「ハイ高校にＡＴＯＭＩＣ ＢＯＭＢが仕掛けられている。」</center><br/>と、いうものだ。</p>"
                "<p>どうやらハイ高が廃校になった時、気が狂った理科の先生がＡＴＯＭＩＣ ＢＯＭＢを、学校のどこかに仕掛けてしまったらしい。</p>"
                "<p>お願いだ。我が母校のコナゴナになった姿を見たくはない。<br/>"
                "早くＡＴＯＭＩＣ ＢＯＭＢを取り除いてくれ……！！</p>"
                "<p>行動は英語で、&quot;&lt;動詞&gt;&quot;或いは、&quot;&lt;動詞&gt;&quot;+&quot;&lt;目的語&gt;&quot;のように入れていただきたい。<br/>"
                "例えば、&quot;look room&quot;と入れれば部屋の様子を見ることが出来るという訳だ。</p>"
                "<p>それでは Ｇｏｏｄ Ｌｕｃｋ！！！............</p>"
    );
}

void
ZSystem::applyTheme(bool dark)
{
    QString fileName = QString(files()->theme_file(dark).c_str());
    QFile f(fileName);
    if (f.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(f.readAll());
        qApp->setStyleSheet(style);
    }
}

