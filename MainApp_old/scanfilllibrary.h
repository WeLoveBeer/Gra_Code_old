#ifndef SCANFILLLIBRARY_H
#define SCANFILLLIBRARY_H
#include<QVector>
#include <QList>
#include "polygon.h"
#include <QMainWindow>

//�߱�����
class Edge
{
public:
    //x����
    double xi;
    //б�ʵĵ���
    double dx;
    //�߶�y���Ͻ�
    double ymax;
    bool operator <(const Edge& rhs) const // ��������ʱ����д�ĺ���
    {
        return xi < rhs.xi;
    }
};

//ɨ�����㷨��
class ScanFillLibrary
{
public:

    ScanFillLibrary(Polygon polygon,double precision,QVector<QVector<Grid*>>& map,QVector<Grid*>& areaMap);
    //ִ���㷨
    void ProcessScanLineFill(int mode );
    //�ж��Ƿ�һ�θ���
    bool isFullCoverageOnce();
	//���ö����
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
	//�������񾫶� 
    inline void SetPrecision(double precision)
    {
        m_precision = precision;
    }
	//���¹�������α߽�
    void RestructureBounds();
public:
    QVector<Point> tempPoint;//���ɨ�赽�ĵ㣬����2D���ڵĻ���
	int m_areaPointCount;//ɨ�赽�����������
	//�õ�������id
	inline void SetStripID(int id)
	{
		m_stripID = id;
	};
private:
    QList<Edge> m_activeEdgeTable;//��߱�
    QVector<QList<Edge>> m_newEdgeTable;//�±߱�
    Polygon m_polygon;//����ζ���
    double m_precision;//���񾫶�,Ĭ��0.125
    double m_xmax,m_xmin;//�����x�߽�
    double m_ymax,m_ymin;//�����y�߽�
    double m_restructedMaxY;//������Y���Ͻ�
    double m_boundPointScale;//�Ա߽����зŴ�
    double m_boundMax;//��������ߴ�
    QVector<Grid*>& m_pAreaMap;//���������
    QVector<QVector<Grid*>>& m_pMap;//��������
    bool m_isFirst;

	int m_stripID;//����ID
	Polygon m_RawPolygon;//ԭʼ��γ��������m_polygon�ᱻ����
	Polygon m_toDeletePolygon;//��Ҫ�������ȥ������
	

private:
    //��ȡ�����С��Yֵ
    void GetPolygonMinMax(bool isFirst);
    //��ʼ���±߱�
    void InitScanLineNewEdgeTable();
    //��������
    void Clean();
	



};

#endif // SCANFILLLIBRARY_H
