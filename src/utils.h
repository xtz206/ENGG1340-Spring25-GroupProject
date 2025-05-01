/**
 * @file utils.h
 * @brief Utility classes and structures for handling positions, sizes, and attributed strings in a text-based interface.
 *
 * This header file defines the following:
 * - The `Position` class, which represents a 2D position with utility operators for arithmetic operations and comparisons.
 * - The `Size` type alias, which is equivalent to `Position` and represents dimensions.
 * - The `AttrString` structure, which represents a string with associated text attributes.
 * - The `VAttrString` type alias, which is a vector of `AttrString` objects.
 *
 * @note The `Position` class uses (y, x) coordinates to align with the (line, col) system used by ncurses.
 */

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <ncurses.h>
/**
 * @class Position
 * @brief Represents a 2D position with coordinates (y, x), where y corresponds to the vertical axis and x to the horizontal axis.
 *
 * @note The Position class uses (y, x) instead of (x, y) to align with the (line, col) system used by ncurses.
 *       It provides utility operators for arithmetic operations and comparisons.
 */
class Position
{
public:
    /**
     * @union
     * @brief Provides multiple aliases for the vertical coordinate.
     *
     * Members:
     * - y: Represents the vertical position.
     * - l: Alias for line, used in the context of text-based systems.
     * - h: Alias for height, used in the context of dimensions.
     */
    union
    {
        int y;
        int l; // line
        int h; // height
    };

    /**
     * @union
     * @brief Provides multiple aliases for the horizontal coordinate.
     *
     * Members:
     * - x: Represents the horizontal position.
     * - c: Alias for column, used in the context of text-based systems.
     * - w: Alias for width, used in the context of dimensions.
     */
    union
    {
        int x;
        int c; // col
        int w; // width
    };

    Position(void) : y(0), x(0) {};
    Position(int ny, int nx) : y(ny), x(nx) {};

    Position operator+(const Position &p) const { return Position(y + p.y, x + p.x); };
    Position operator-(const Position &p) const { return Position(y - p.y, x - p.x); };
    Position operator*(int n) const { return Position(y * n, x * n); };
    Position operator/(int n) const { return n != 0 ? Position(y / n, x / n) : Position(0, 0); };
    bool operator==(const Position &p) const { return y == p.y && x == p.x; };
};

/**
 * @typedef Size
 * @brief An alias for the Position type, used to represent dimensions or sizes.
 */
typedef Position Size;

/**
 * @struct AttrString
 * @brief Represents a string with associated text attributes for use in a text-based interface.
 *
 * The `AttrString` structure combines a string with an attribute, allowing for text styling
 * such as bold, underline, or color when displayed in an ncurses-based interface.
 *
 * @var AttrString::str
 * The string content to be displayed.
 *
 * @var AttrString::attr
 * The text attribute (e.g., A_BOLD, A_UNDERLINE) applied to the string.
 *
 * @note The default attribute is `A_NORMAL`, which represents normal text without any styling.
 *
 * @see attr_t for more information on ncurses text attributes.
 */
struct AttrString
{
    std::string str;
    attr_t attr;
    AttrString(const char *s, attr_t a = A_NORMAL) : str(s), attr(a) {};
    AttrString(const std::string &s, attr_t a = A_NORMAL) : str(s), attr(a) {};
};
typedef std::vector<AttrString> VAttrString;
#endif
