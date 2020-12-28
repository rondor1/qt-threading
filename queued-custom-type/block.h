#ifndef BLOCK_H
#define BLOCK_H

#include <QMetaType>

#include <QRect>
#include <QColor>

class Block
{
public:
    Block();
    Block(const Block& block);
    ~Block();

    Block(const QRect& rect, const QColor& color);

    QColor color() const;
    QRect rect() const;

private:
    QRect m_rect;
    QColor m_color;
};


Q_DECLARE_METATYPE(Block);

#endif // BLOCK_H

