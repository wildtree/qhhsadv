/*
 * ZMapData
 */

#include <zmap.h>
#include <QFile>

const static std::string _emptyString = std::string();

ZMapData::ZMapData()
    : _blank(false), _m(""), _b("")
{
    _v = NULL;
    _msg = NULL;

}

ZMapData::ZMapData(const uint8_t *b)
    :_blank(false)
{
    _v = new uint8_t [vectorSize];
    _msg = new ZMapMessage [maxMapElements];
    memcpy(_v, b, vectorSize);
    int m = vectorSize + relationSize;
    int n = 0;
    std::vector<std::string> msg(maxMapElements);
    //std::string msg[maxMapElements];
    for (n = 0 ; m < fileBlockSize ; n++)
    {
        int len = ((int)(b[m++] & 0xff) << 8)|(int)(b[m++] & 0xff);
        if (len == 0) break;
        std::vector<char> bmsg(len + 1);
        for(int i = 0; i < len; i++) bmsg[i] = b[m + i];
        //char bmsg[len+1];
        //memcpy(bmsg, &b[m], len);
        bmsg[len] = 0;
        msg[n] = std::string(bmsg.data());
        //msg[n] = std::string(bmsg);
        m += len;
    }
    _m = msg[0];
    int j = vectorSize;
    for (int i = 0 ; i <= n ; i++)
    {
        uint8_t v = b[j++] & 0xff;
        if (v == 0) break;
        uint8_t o = b[j++] & 0xff;
        uint8_t idx = (b[j++] & 0xff) - 1;
        _msg[i] = ZMapMessage(v, o, msg[idx]);
    }
    //_msg[n + 1] = ZMapMessage();
}

ZMapData::ZMapData(const ZMapData &x)
    : _blank(x._blank), _m(x._m), _b(x._b)
{
    _v = new uint8_t [vectorSize];
    _msg = new ZMapMessage [maxMapElements];
    memcpy(_v, x._v, vectorSize);
    int i = 0;
    for (i = 0 ; ! x._msg[i].invalid() ; i++) _msg[i] = x._msg[i];
    //_msg[i] = ZMapMessage();
}

ZMapData::~ZMapData()
{
    if (_v != NULL) delete [] _v;
    if (_msg != NULL) delete [] _msg;
}

ZMapData &
ZMapData::operator = (const ZMapData &x)
{
    _blank = x._blank;
    _m = x._m;
    _b = x._b;
    if (_v == NULL) _v = new uint8_t [vectorSize];
    memcpy(_v, x._v, vectorSize);
    if (_msg == NULL) _msg = new ZMapMessage [maxMapElements];
    int i = 0;
    for (i = 0 ; ! x._msg[i].invalid() ; i++) _msg[i] = x._msg[i];
    //_msg[i] = ZMapMessage();
    return *this;
}

const std::string&
ZMapData::find(uint8_t v, uint8_t o) const
{
    for (int i = 0 ; ! _msg[i].invalid() ; i++)
    {
        if (_msg[i].match(v, o))
        {
            return _msg[i].getMessage();
        }
    }
    return _emptyString;
}

void
ZMapData::setBlank(const std::string &msg)
{
    _blank = true;
    _b = msg;
}

const std::string &
ZMapData::getMessage() const
{
    if (_blank) return _b;
    return _m;
}

void
ZMapData::draw(Canvas *cv)
{
    if (_blank) 
    {
        cv->cls();
        return;
    }
    _draw(cv);
}

void
ZMapData::_draw(Canvas *cv)
{
    int i = (int)(_v[0] & 0xff) * 3 + 1;
    cv->cls(Canvas::BLUE);
    i = _drawOutline(cv, i, Canvas::WHITE);
    int x0 = (int)(_v[i++] & 0xff);
    int y0 = (int)(_v[i++] & 0xff);
    while (x0 != 0xff || y0 != 0xff)
    {
        uint16_t c = cv->getColor((int)(_v[i++] & 0xff));
        cv->paint(x0, y0, c, Canvas::WHITE);
        x0 = (int)(_v[i++] & 0xff);
        y0 = (int)(_v[i++] & 0xff);
    }
    if ((_v[i] &0xff) == 0xff && ((_v[i + 1] & 0xff) == 0xff))
    {
        i += 2;
    }
    else
    {
        i = _drawOutline(cv, i, Canvas::WHITE);
    }
    if ((_v[i] &0xff) == 0xff && ((_v[i + 1] & 0xff) == 0xff))
    {
        i += 2;
    }
    else
    {
        i = _drawOutline(cv, i, Canvas::BLACK);
    }
    cv->tonePaint(_v);
}

int
ZMapData::_drawOutline(Canvas *cv, int ofst, uint16_t c)
{
    int x0, y0, x1, y1;
    int p = ofst;
    x0 = (int)(_v[p++] & 0xff);
    y0 = (int)(_v[p++] & 0xff);
    for(;;)
    {
        x1 = (int)(_v[p++] & 0xff);
        y1 = (int)(_v[p++] & 0xff);
        if (y1 == 0xff)
        {
            if (x1 == 0xff) break;
            x0 = (int)(_v[p++] & 0xff);
            y0 = (int)(_v[p++] & 0xff);
            continue;
        }
        cv->line(x0, y0, x1, y1, c);
        x0 = x1;
        y0 = y1;
    }
    return p;
}

ZMapRoot::ZMapRoot(const std::string &file)
    : _p(1), _l(0), _v(84), _file(file), _map(nullptr)
{
}

ZMapRoot::~ZMapRoot()
{
}

ZMapData &
ZMapRoot::curMapData()
{
    if(_p != _l)
    {
        QFile fi(_file.c_str());
        if (fi.open(QIODevice::ReadOnly))
        {
            uint8_t* buf = new uint8_t [fileBlockSize];
            fi.seek(_p * fileBlockSize);
            fi.read(reinterpret_cast<char*>(buf), fileBlockSize);
            fi.close();
            if (_map != nullptr) delete _map;
            _map = new ZMapData(buf);
            if (_p == 0 || _p == 84 || _p == 85)
            {
                _map->setBlank();
                // _map.setBlank(msg[0x4c]);
            }
            _l = _p;
            delete [] buf;
        }

    }
    return *_map;
}

ZMessage::ZMessage(const std::string &file)
{
    QFile fi(file.c_str());
    int i = 0;
    if (fi.open(QIODevice::ReadOnly))
    {
        char c;
        std::string tmp = std::string();
        while(true)
        {
            if (fi.read(&c, 1) == 0) break;
            if (c == 0)
            {
                _msgs[i++] = std::string(tmp);
                tmp = std::string();
                continue;
            }
            tmp += c;
        }
        fi.close();
    }

    while (i < ZMessage::MAX_MESSAGE)
    {
        _msgs[i++] = std::string();
    }
}

ZMessage::~ZMessage()
{
}
