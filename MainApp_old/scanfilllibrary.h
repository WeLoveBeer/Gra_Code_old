#ifndef SCANFILLLIBRARY_H
#define SCANFILLLIBRARY_H
#include<QVector>
#include <QList>
#include "polygon.h"
#include <QMainWindow>

//边表结点类
class Edge
{
public:
    //x坐标
    double xi;
    //斜率的倒数
    double dx;
    //线段y的上界
    double ymax;
    bool operator <(const Edge& rhs) const // 升序排序时必须写的函数
    {
        return xi < rhs.xi;
    }
};

//扫描线算法类
class ScanFillLibrary
{
public:

    ScanFillLibrary(Polygon polygon,double precision,QVector<QVector<Grid*>>& map,QVector<Grid*>& areaMap);
    //执行算法
    void ProcessScanLineFill(int mode );
    //判断是否一次覆盖
    bool isFullCoverageOnce();
	//设置多边形
    inline void SetPolygon(const Polygon& polygon)
    {
        m_polygon = polygon;
		m_RawPolygon = polygon;
    }
	inline void SetDeletePolygon(const Polygon& polygon)
	{
		m_toDeletePolygon = polygon;
	}
	inline Polygon GetPolygon()
	{
		return m_RawPolygon;
	}
	//设置网格精度 
    inline void SetPrecision(double precision)
    {
        m_precision = precision;
    }
	//重新规整多边形边界
    void RestructureBounds();
public:
    QVector<Point> tempPoint;//存放扫描到的点，用于2D窗口的绘制
	int m_areaPointCount;//扫描到的区域点数量
	//得到条带的id
	inline void SetStripID(int id)
	{
		m_stripID = id;
	};
private:
    QList<Edge> m_activeEdgeTable;//活动边表
    QVector<QList<Edge>> m_newEdgeTable;//新边表
    Polygon m_polygon;//多边形对象
    double m_precision;//网格精度,默认0.125
    double m_xmax,m_xmin;//多边形x边界
    double m_ymax,m_ymin;//多边形y边界
    double m_restructedMaxY;//归整后Y的上界
    double m_boundPointScale;//对边界点进行放大
    double m_boundMax;//区域度数尺寸
    QVector<Grid*>& m_pAreaMap;//区域的数组
    QVector<QVector<Grid*>>& m_pMap;//最大的数组
    bool m_isFirst;

	int m_stripID;//条带ID
	Polygon m_RawPolygon;//原始经纬度条带，m_polygon会被规整
	Polygon m_toDeletePolygon;//需要从区域减去的条带
	

private:
    //获取最大最小的Y值
    void GetPolygonMinMax(bool isFirst);
    //初始化新边表
    void InitScanLineNewEdgeTable();
    //清空类对象
    void Clean();
	



};

#endif // SCANFILLLIBRARY_H
