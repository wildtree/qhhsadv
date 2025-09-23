//
// ZWords -- HighHigh School Adventure Words Dictionary
//
#ifndef ZWORDS_H
#define ZWORDS_H

#include <sys/types.h>
#include <string>

class ZWord
{
protected:
    std::string _word;
    int8_t _id;
public:
    ZWord() : _word(""), _id(-1) {}
    ZWord(const char b[]);
    ZWord(const ZWord &x) : _word(x._word), _id(x._id) {}
    ~ZWord() {}

    bool valid() const { return _id >= 0; }
    bool match(const std::string &v) const;
    int id() const { return (int)_id; }
};

class ZWords
{
protected:
    ZWord _verbs[100], _objs[100];
public:
    ZWords(const std::string &file);
    ZWords(const ZWords &x);
    ~ZWords() {}

    int findVerb(const std::string &w) const;
    int findObjs(const std::string &w) const;
};
#endif /* ZWORDS_H */
