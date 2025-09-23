/*
 * User Data
 */

#include <zuserdata.h>
#include <QFile>

const int ZUserData::links = 87;
const int ZUserData::items = 12;
const int ZUserData::flags = 15;

const int ZUserData::packed_size = ZUserData::links * sizeof(ZMapLink) + ZUserData::items + ZUserData::flags;

const std::string ZUserData::item_labels[] = {
    "ネクタイ",
    "制服",
    "鍵",
    "懐中電灯",
    "乾電池",
    "ビデオテープ",
    "ファイル",
    "ダイナマイト",
    "塩酸",
    "ジャッキ",
    "マッチ",
    "ペンチ",
};

ZMapLink::ZMapLink()
{
    memset(_link.l, 0, sizeof(_link));
}

ZMapLink::ZMapLink(const uint8_t b[])
{
    memcpy(_link.l, b, sizeof(_link));
}

ZMapLink::ZMapLink(const ZMapLink &x)
{
    memcpy(_link.l, x._link.l, sizeof(_link));
}

ZMapLink
&ZMapLink::operator =(const uint8_t b[])
{
    memcpy(_link.l, b, sizeof(_link));
    return *this;
}

void
ZMapLink::setLink(int id, uint8_t value)
{
    _link.l[id] = value;
}

ZUserData::ZUserData()
{
    _map = new ZMapLink [links];
    _places = new uint8_t [items];
    _flags = new uint8_t [flags];
    _buf = new uint8_t [items + flags + links * sizeof(ZMapLink)];
}

ZUserData::ZUserData(const ZUserData &x)
{
    _map = new ZMapLink [links];
    _places = new uint8_t [items];
    _flags = new uint8_t [flags];
    _buf = new uint8_t [items + flags +links * sizeof(ZMapLink)];

    memcpy(_map, x._map, links * sizeof(ZMapLink));
    memcpy(_places, x._places, items);
    memcpy(_flags, x._flags, flags);
}

ZUserData::ZUserData(const std::string &file)
{
    _map = new ZMapLink [links];
    _places = new uint8_t [items];
    _flags = new uint8_t [flags];
    _buf = new uint8_t [items + flags + links * sizeof(ZMapLink)];
    this->init(file);
}

ZUserData::~ZUserData()
{
    delete [] _map;
    delete [] _places;
    delete [] _flags;
    delete [] _buf;
}

void
ZUserData::init(const std::string &file)
{
    QFile fi(file.c_str());
    if (fi.open(QIODevice::ReadOnly))
    {
        uint8_t *buf = new uint8_t [sizeof(ZMapLink)];
        for (int i = 0 ; i < links ; i++)
        {
            if (fi.read(reinterpret_cast<char*>(buf), sizeof(ZMapLink)) < sizeof(ZMapLink)) break;
            _map[i] = buf;
        }
        delete[] buf;
        fi.seek(0x301);
        fi.read(reinterpret_cast<char*>(_places), items);
        fi.seek(0x311);
        fi.read(reinterpret_cast<char*>(_flags), flags);
        fi.close();
    }
}

void
ZUserData::unpack(const uint8_t buf[])
{
    for (int i = 0 ; i < links ; i++)
    {
        getMap(i).unpack(&buf[i * sizeof(ZMapLink)]);
    }
    memcpy(_places, &buf[links * sizeof(ZMapLink)], items);
    memcpy(_flags, &buf[items + links * sizeof(ZMapLink)], flags);
}

uint8_t *
ZUserData::pack() const
{
    for (int i = 0 ; i < links ; i++)
    {
        memcpy(&_buf[i * sizeof(ZMapLink)], _map[i].pack(), sizeof(ZMapLink));
    }
    memcpy(&_buf[links * sizeof(ZMapLink)], _places, items);
    memcpy(&_buf[items + links * sizeof(ZMapLink)], _flags, flags);

    return _buf;
}

std::string ZUserData::itemList() const
{
    std::string s;
    for (int i = 0 ; i < items ; i++)
    {
        if (getPlace(i) == 0xff) s += ZUserData::item_labels[i] + " ";
    }
    return s;
}
