#include "polygon.h"

Polygon::Polygon()
{

}
Polygon::Polygon(QVector<Point> polygon)
{
    m_polygon = polygon;
}
int Polygon::GetPolyCount()
{
    return m_polygon.size();
}
