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
    Polygon m_bound;//���������Ķ����
    QVector<Grid*> m_innerGrids;//�����ڵ�����
    QColor m_stripColor;//���������ɫ
    int m_ID;//�������

};

class InstantStrip: public Strip
{
public:
    int m_parentStripID;
};

#endif // STRIP_H
