#ifndef UTILS_H
#define UTILS_H

class Position
{
    // NOTE: The Position is (y, x) instead of (x, y)
    //       This is to match the (line, col) system used by ncurses
public:
    union
    {
        int y;
        int l; // line
        int h; // height
    };
    union
    {
        int x;
        int c; // col
        int w; // width
    };

public:
    Position() : y(0), x(0) {}
    Position(int ny, int nx) : y(ny), x(nx) {}
    Position operator+(const Position &p) const { return Position(y + p.y, x + p.x); };
    Position operator-(const Position &p) const { return Position(y - p.y, x - p.x); };
    Position operator*(int n) const { return Position(y * n, x * n); };
    Position operator/(int n) const { return n != 0 ? Position(y / n, x / n) : Position(0, 0); }
    bool operator==(const Position &p) const { return y == p.y && x == p.x; };
};
typedef Position Size;

#endif
