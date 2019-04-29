#ifndef POLYGON_H
#define POLYGON_H
#include <QVector>
#include <QSet>
//点类，定义2D平面上的点
class Point
{
private:
    double px;
    double py;
public:
    Point(double xx,double yy):px(xx),py(yy){}
    Point(){}
    inline double x()const{return this->px;}
    inline double y()const{return this->py;}
    inline void setX(double xx){px = xx;}
    inline void setY(double yy){py = yy;}

};

//网格点类型
class Grid
{
public:
    Grid():m_totalOverlayCount(0){}
    int m_totalOverlayCount;//格网总的被覆盖次数
	QSet<int> m_stripSet;//每个条带出现的次数字典
    //QVector<QString> m_stripOnPoint;//覆盖在点上的条带集合
	
};
enum Mode
{
	ADD_MODE = 1,
	JUDGE_MODE = 2,
	DECREASE_MODE = 3,
	AREA_MODE = 0,
	REMOVE_VISIBLE = 4
	

};
//多边形类，一系列点的集合
class Polygon
{
public:
    Polygon(QVector<Point > polygon);
    Polygon();
    int GetPolyCount();
    inline Point& operator[](int n)
    {
        return m_polygon[n];
    }
    inline QVector<Point>& GetPloygonVector()
    {
        return m_polygon;
    }
    /*inline Polygon& operator=(const Polygon& polygon)
    {
        m_polygon = polygon.m_polygon;
		return m_polygon;
    }*/
private:
    QVector<Point> m_polygon;
};

#endif // POLYGON_H
