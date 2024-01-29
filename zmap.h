/*
 * Map data class
 */

#ifndef ZMAP_H
#define ZMAP_H

#include <sys/types.h>
#include <SDL2/SDL.h>
#include <graph.h>
#include <string>

class ZMapMessage
{
protected:
    uint8_t _verb;
    uint8_t _obj;
    std::string _msg;
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    ZMapMessage() : _verb(0), _obj(0) {}
    ZMapMessage(uint8_t v, uint8_t o, const std::string &s) : _verb(v), _obj(o), _msg(s) {}
    ZMapMessage(const ZMapMessage &x) : _verb(x._verb), _obj(x._obj), _msg(x._msg) {}
    bool match(uint8_t v, uint8_t o) const { return v == _verb && o == _obj; }
    const std::string &getMessage() const { return _msg; }
    bool invalid() const { return _verb == 0 && _obj == 0; }
};

class ZMapData
{
protected:
    const int vectorSize = 0x400;
    const int relationSize = 0x100;
    const int messageSize = 0x500;
    const int maxMapElements = 0x100;

    uint8_t *_v;
    ZMapMessage *_msg;
    std::string _m, _b;
    bool _blank;

    int _drawOutline(Canvas *cv, int ofst, uint16_t c);
    void _draw(Canvas *cv);
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    ZMapData();
    ZMapData(const ZMapData &x);
    ZMapData(const uint8_t *b);
    ~ZMapData();
    ZMapData &operator = (const ZMapData &x);
    const bool isBlank() const { return _blank; }
    const std::string &find(uint8_t v, uint8_t o) const;
    void setBlank(const std::string &msg);
    void setBlank() { _blank = true; }
    void resetBlank() { _blank = false; }
    const std::string &getMessage() const;
    void draw(Canvas *cv);

    const int fileBlockSize = 0xa00;
};

class ZMapRoot
{
protected:
    const int maxRooms = 100;
    const int fileBlockSize = 0xa00;
    ZMapData *_map;
    int _p, _l, _v;
    std::string _file;
    bool _blank;
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    ZMapRoot(const std::string &file);
    ~ZMapRoot();

    int setCursor(int p) { return _p = p; };
    int getCursor() const { return _p; }

    bool isBlank() const { return _blank; }

    int look(int p) { _v = _p; return _p = p; }
    int back(void) { return _p = _v; }

    ZMapData &curMapData();
};

class ZMessage
{
protected:
    static const int MAX_MESSAGE = 127;
    std::string _msgs[ZMessage::MAX_MESSAGE];
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    ZMessage(const std::string &file);
    ~ZMessage();

    const std::string& getMessage(int id) const { return _msgs[id & 0x7f]; }
};

#endif /* ZMAPP_H */