#ifndef POLYGON_H
#define POLYGON_H
#include <QVector>
#include <QSet>
//���࣬����2Dƽ���ϵĵ�
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

//���������
class Grid
{
public:
    Grid():m_totalOverlayCount(0){}
    int m_totalOverlayCount;//�����ܵı����Ǵ���
	QSet<int> m_stripSet;//ÿ���������ֵĴ����ֵ�
    //QVector<QString> m_stripOnPoint;//�����ڵ��ϵ���������
	
};
enum Mode
{
	ADD_MODE = 1,
	JUDGE_MODE = 2,
	DECREASE_MODE = 3,
	AREA_MODE = 0,
	REMOVE_VISIBLE = 4
	

};
//������࣬һϵ�е�ļ���
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
