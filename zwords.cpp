/*
 * ZWords -- HighHigh School Adventure Words Dictionary
 */

#include <zwords.h>
#include <cctype>
#include <algorithm>
#include <QFile>

ZWord::ZWord(const char b[])
   : _word(""), _id(-1)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        uint8_t z = (b[i] & 0xff);
        if (z == 0) break;
        _word += (char)(z - 1);
    }
    _id = (int8_t)b[4];
}

bool
ZWord::match(const std::string &v) const
{
    std::string z = v + "    ";
    z = z.substr(0,4);
    if (std::equal(z.begin(), z.end(), _word.begin(), 
        [](char a, char b){ 
            return std::toupper(static_cast<uint8_t>(a)) == std::toupper(static_cast<uint8_t>(b));
        }))
    {
        return true;
    }
    return false;
}

ZWords::ZWords(const std::string &file)
{
    QFile fi(file.c_str());
    if (!fi.open(QIODevice::ReadOnly)) return;

    int len = 0;
    int p = 0;
    while (true)
    {
        QByteArray data = fi.read(5);
        if (data.isEmpty()) break;
        len += data.size();
        if (len >= 0x200 || data.constData()[0] == 0) break;
        _verbs[p++] = ZWord(data.constData());
    }
    fi.seek(0x200);
    len = 0;
    p = 0;
    while (true)
    {
        QByteArray data = fi.read(5);
        if (data.isEmpty()) break;
        len += data.size();
        if (len >= 0x200 || data.constData()[0] == 0) break;
        _objs[p++] = ZWord(data.constData());
    }
    fi.close();
}

ZWords::ZWords(const ZWords &x)
{
    for (int i = 0 ; i < 100 ; i++)
    {
        if (x._verbs[i].valid()) _verbs[i] = x._verbs[i];
        if (x._objs[i].valid()) _objs[i] = x._objs[i];
    }
}

int
ZWords::findVerb(const std::string &s) const
{
    for (int i = 0 ; i < 100 ; i++)
    {
        if (_verbs[i].valid() && _verbs[i].match(s))
        {
            return _verbs[i].id();
        }
    }
    return -1;
}

int
ZWords::findObjs(const std::string &s) const
{
    for (int i = 0 ; i < 100 ; i++)
    {
        if (_objs[i].valid() && _objs[i].match(s))
        {
            return _objs[i].id();
        }
    }
    return -1;
}
