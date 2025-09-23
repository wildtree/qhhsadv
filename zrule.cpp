/*
 * ZRule
 */

#include <zrule.h>
#include <zuserdata.h>
#include <random>
#include <QFile>

const int ZCore::packed_size = 8;

ZRuleBlock::ZRuleBlock()
{
    memset(_block._b, 0, sizeof(_block));
}

ZRuleBlock::ZRuleBlock(const uint8_t b[])
{
    memcpy(_block._b, b, sizeof(_block));
}

ZRuleBlock::ZRuleBlock(const ZRuleBlock &x)
{
    memcpy(_block._b, x._block._b, sizeof(_block));
}

ZRuleBlock &
ZRuleBlock::operator =(const ZRuleBlock &x)
{
    memcpy(_block._b, x._block._b, sizeof(_block));
    return *this;
}

uint8_t
ZRuleBlock::body_type() const
{
    body_offset_t tmp;
    tmp._v = offset();
    return tmp.bits._body_type;
}

uint8_t
ZRuleBlock::body_id() const
{
    body_offset_t tmp;
    tmp._v = offset();
    return tmp.bits._body_id;
}

uint8_t
ZRuleBlock::getOp1(ZCore *c, ZUserData *u) const
{
    uint8_t v = 0;
    switch (type())
    {
    case typNone:
        break;
    case typFact:
        v = u->getFact(id());
        break;
    case typPlace:
        v = u->getPlace(id());
        break;
    case typSystem:
        c->rand(0);
        v = c->getValue(id());
        break;
    case typVector:
        v = u->getMap(offset() - 1).get(id());
        break;
    default:
        break;
    }
    return v;
}

uint8_t
ZRuleBlock::getOp2(ZCore *c, ZUserData *u) const
{
    int v = value();
    if (body_type() != typNone && type() != typVector)
    {
        switch(body_type())
        {
        case typFact:
            v = u->getFact(body_id());
            break;
        case typPlace:
            v = u->getPlace(body_id());
            break;
        case typSystem:
            c->rand(0);
            v = c->getValue(body_id());
            break;
        default:
            break;
        }
    }
    return v;
}

bool
ZRuleBlock::doCmp(ZCore *c, ZUserData *u) const
{
    bool ok = false;
    uint8_t v1 = getOp1(c, u);
    uint8_t v2 = getOp2(c, u);
    switch(op())
    {
    case cmpEq: ok = (v1 == v2); break;
    case cmpNe: ok = (v1 != v2); break;
    case cmpGt: ok = (v1 > v2); break;
    case cmpGe: ok = (v1 >= v2); break;
    case cmpLt: ok = (v1 < v2); break;
    case cmpLe: ok = (v1 <= v2); break;
    default:
        break;
    }
    return ok;
}

bool
ZRuleBlock::doAct(ZCore *c, ZUserData *u) const
{
    bool ok = false;
    switch (op())
    {
    case actMove:
        if (u->getMap(c->mapId() - 1).get(value()) != 0)
        {
            c->mapId(u->getMap(c->mapId() - 1).get(value()));
            ok = true;
        }
        else
        {
            // check teacher
            if (u->getFact(1) == c->mapId() && c->rand() > 85)
            {
                // put message #0xb5 先生につかまった!
                c->push(ZCommand::Message, 0xb5);
                c->push(ZCommand::GameOver, 1);
                u->setFact(1,0);
                // change color to Sepia ???
                // game over
                ok = false;
            }
            else
            {
                // put message #0xb6 移動できない!
                c->push(ZCommand::Message, (uint8_t)0xb6); // 移動できない
                ok = true;
            }
            
        }
        break;
    case actAsgn:
        switch (type())
        {
        case typFact:
            u->setFact(id(),getOp2(c,u));
            break;
        case typPlace:
            u->setPlace(id(),getOp2(c,u));
            break;
        case typSystem:
            c->setValue(id(), (uint8_t)(getOp2(c,u) & 0xff));
            if (id() == 5)
            {
                c->rand(0);
            }
            break;
        case typVector:
            u->getMap(offset() - 1).setLink(id(),getOp2(c,u));
            break;
        default:
            break;
        }
        ok = true;
        break;
    case actMesg:
        c->push(ZCommand::Message, value());
        ok = true;
        break;
    case actDlog:
        // show dialog
        c->push(ZCommand::Dialogue, value());
        ok = true;
        break;
    case actLook:
        if (value() == 0)
        {
            c->mapId(c->mapView()); // back;
            c->mapView(0);
        }
        else
        {
            c->mapView(c->mapId());
            c->mapId(value());
        }
        ok = true;
        break;
    case actSnd:
        // play sound
        c->push(ZCommand::Sound, value());
        ok = true;
        break;
    case actOver:
        switch(value())
        {
        case 0:
            u->setFact(1,0); // teacher has gone
            c->push(ZCommand::Message,0xee);
            c->push(ZCommand::GameOver,1);
            break;
        case 1:
            // color code to sepia
            c->push(ZCommand::Message,0xee);
            c->push(ZCommand::GameOver,2);
            break;
        case 2:
            // game clear
            c->push(ZCommand::Message,0xef);
            c->push(ZCommand::GameOver,3);
            break;
        }
        // game over
        ok = true;
        break;
    default:
        break;
    }
    return ok;
}

ZCore::ZCore()
    : _mt({ std::random_device{}() })
{
    memset(_stat.b, 0, sizeof(_stat));

}

uint8_t
ZCore::rand() 
{
    rand(0);
    return _stat.attr._random;
}

void
ZCore::rand(uint8_t seed)
{
    std::uniform_int_distribution<int> dist(0, 255);
    if (seed != 0)
    {
        //randomSeed(seed);
    }
    _stat.attr._random = dist(_mt);
}

ZCommand
ZCore::pop()
{
    ZCommand tmp = _queue.front();
    _queue.pop();
    return tmp;
}

void
ZCore::flush()
{
    while(!_queue.empty()) _queue.pop();
}

void
ZCore::unpack(const uint8_t buf[])
{
    memcpy(_stat.b, buf, sizeof(_stat));
}

uint8_t *
ZCore::pack() const
{
    return (uint8_t*)_stat.b;
}

ZRuleBase::ZRuleBase(void)
    :_cmd_id(end_of_rule_mark), _obj_id(end_of_rule_mark), _map_id(end_of_rule_mark)
{
    _rules = nullptr;
}
ZRuleBase::ZRuleBase(const uint8_t *buf)
    :_cmd_id(end_of_rule_mark), _obj_id(end_of_rule_mark), _map_id(end_of_rule_mark)
{
    _rules = new ZRuleBlock[rule_block_length];
    uint8_t rb[4];
    _map_id = buf[0] & 0xff;
    _cmd_id = buf[1] & 0xff;
    _obj_id = buf[2] & 0xff;
    // Serial.printf("Loading (map_id, cmd_id, obj_id) = (%d, %d, %d)\r\n", _map_id, _cmd_id, _obj_id);
    for (int i = 0 ; i < rule_block_length ; i++)
    {
        memcpy(rb, &buf[4*i + 4], 4);
        _rules[i] = ZRuleBlock(rb);
    }
}

ZRuleBase::ZRuleBase(const ZRuleBase &x)
    :_map_id(x._map_id), _cmd_id(x._cmd_id), _obj_id(x._obj_id)
{
    _rules = new ZRuleBlock[rule_block_length];
    for (int i = 0 ; i < rule_block_length ; i++)
    {
        _rules[i] = x._rules[i];
    }
}

ZRuleBase::~ZRuleBase()
{
    if (_rules) delete[] _rules;
}

ZRuleBase
ZRuleBase::operator =(const ZRuleBase &x)
{
    _map_id = x._map_id;
    _cmd_id = x._cmd_id;
    _obj_id = x._obj_id;
    if (_rules != nullptr) delete[] _rules;

    _rules = new ZRuleBlock[rule_block_length];
    for (int i = 0 ; i < rule_block_length ; i++)
    {
        _rules[i] = x._rules[i];
    }
    return *this;
}

bool
ZRuleBase::about(int map_id, int cmd_id, int obj_id) const
{
    return (map_id == _map_id || _map_id == 0)&&(cmd_id == _cmd_id || _cmd_id == 0)&&(obj_id == _obj_id||_obj_id == 0);
}

bool
ZRuleBase::run(ZCore *core, ZUserData *user) const
{
    if (about(core))
    {
        bool cond_ok = true;
        bool act_ok = true;
        int i = 0;
        while (_rules[i].actCmp())
        {
            cond_ok = _rules[i++].doCmp(core, user);
            if (!cond_ok) return false; // condition check failed.
        }
        if (cond_ok) // all clear. go action phase
        {
            while(_rules[i].op() != ZRuleBlock::actNop)
            {
                act_ok = _rules[i++].doAct(core, user) && act_ok;
            }
        }
        return true;
    }
    return false;
}

ZRules::ZRules(const std::string &file)
{
    QFile fi(file.c_str());
    uint8_t *buf = new uint8_t [ZRuleBase::file_block_size];
    int i = 0;
    if (fi.open(QIODevice::ReadOnly))
    {
        while(true)
        {
            QByteArray data = fi.read(ZRuleBase::file_block_size);
            if (data.isEmpty()) break;
            _rules[i++] = ZRuleBase(reinterpret_cast<const uint8_t*>(data.constData()));
        }
        fi.close();
    }
}


ZRulesIterator
ZRules::begin() const
{
    return ZRulesIterator(this, 0);
}

ZRulesIterator
ZRules::end() const
{
    int p = 0;
    for (p = 0 ; !_rules[p].end_of_rules() ; p++) {}
    return ZRulesIterator(this, p);
}

ZRulesIterator::ZRulesIterator() : _ptr(0), _r(nullptr) {}
ZRulesIterator::ZRulesIterator(const ZRulesIterator &x) : _ptr(x._ptr), _r(x._r) {}

ZRulesIterator::ZRulesIterator(const ZRules *r, int p) : _ptr(p), _r(r) {}

ZRulesIterator&
ZRulesIterator::operator =(const ZRulesIterator &x)
{
    _ptr = x._ptr;
    _r = x._r;
    return *this;
}

ZRulesIterator &
ZRulesIterator::operator ++()
{
    _ptr++;
    return *this;
}

ZRulesIterator
ZRulesIterator::operator ++(int)
{
    ZRulesIterator tmp = *this;
    ++*this;
    return tmp;
}

const ZRuleBase &
ZRulesIterator::operator *()
{
    return _r->_rules[_ptr];
}

bool
ZRulesIterator::operator ==(const ZRulesIterator &x) const
{
    return (_r == x._r && _ptr== x._ptr);
}

bool
ZRulesIterator::operator !=(const ZRulesIterator &x) const
{
    return !this->operator==(x);
}
