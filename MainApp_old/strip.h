#ifndef STRIP_H
#define STRIP_H
#include <QVector>
#include <QColor>
#include "polygon.h"


class Strip
{
public:
    Strip();
    inline Polygon& GetBound(){return m_bound;}
    inline QVector<Grid*> GetInnerGrid(){return m_innerGrids;}
    inline QColor GetStripColor(){return m_stripColor;}
    inline int GetID(){return m_ID;}
	inline void SetID(int id) { m_ID = id; };
    Strip& operator=(const Strip& strip)
    {
        m_ID == strip.m_ID;
        m_stripColor = strip.m_stripColor;
        m_bound = strip.m_bound;
        m_innerGrids = strip.m_innerGrids;
        return *this;
    }
public:
    Polygon m_bound;//构成条带的多边形
    QVector<Grid*> m_innerGrids;//条带内的网格
    QColor m_stripColor;//条带填充颜色
    int m_ID;//条带编号

};

class InstantStrip: public Strip
{
public:
    int m_parentStripID;
};

#endif // STRIP_H
