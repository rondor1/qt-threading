#include "block.h"

Block::Block()
{

}

Block::Block(const Block &block)
{
    m_color = block.m_color;
    m_rect = block.m_rect;
}

Block::~Block()
{

}

Block::Block(const QRect &rect, const QColor &color)
{
    m_rect = rect;
    m_color = color;
}

QColor Block::color() const
{
    return m_color;
}

QRect Block::rect() const
{
    return m_rect;
}
