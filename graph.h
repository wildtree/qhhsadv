/*
 * Graphic Library for SDL2
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <sys/types.h>
#include <queue>
#include <QtGui/QImage>
class Canvas
{
protected:
    uint16_t _ox, _oy;
    uint16_t _w, _h;
    const uint16_t _col[8] = {
        BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, WHITE,
    };
    float *_colorFilter;
    uint16_t applyFilter(uint16_t);
    QImage *_px;
public:
    Canvas(uint16_t x = 0, uint16_t y = 0, uint16_t w = 320, uint16_t h = 240);
    Canvas(const Canvas &x);
    virtual ~Canvas();

    virtual void cls(uint16_t c = BLACK);
    void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t col);
    virtual void pset(uint16_t x, uint16_t y, uint16_t col);
    uint16_t pget(uint16_t x, uint16_t y) const { return *((uint16_t*)_px->bits() + (y * _w + x)); }
    void paint(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc);
    void tonePaint(const uint8_t tone[], bool tiling = false);
    uint16_t getColor(int c) const { return _col[c]; }
    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);
    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);

    void colorFilter(void);
    void setColorFilter(const float filter[]) { _colorFilter = (float*)filter; }
    void resetColorFilter() { setColorFilter(nullptr); }

    virtual void invalidate() const ;
    virtual QImage *image() const { return _px; }

    static const float blueFilter[], redFilter[], sepiaFilter[];

    static const uint16_t BLACK   = 0x0000;
    static const uint16_t BLUE    = 0x001f;
    static const uint16_t RED     = 0xf800;
    static const uint16_t MAGENTA = 0xf81f;
    static const uint16_t GREEN   = 0x07e0;
    static const uint16_t CYAN    = 0x07ff;
    static const uint16_t YELLOW  = 0xffe0;
    static const uint16_t WHITE   = 0xffff;
    static const uint16_t MAROON  = 0x7800; 
};

class Point
{
public:
    uint16_t x, y;
    Point(uint16_t x_ = 0, uint16_t y_ = 0) : x(x_), y(y_) {}
    Point(const Point &p) : x(p.x), y(p.y) {}
};

#endif /* GRAPH_H */
