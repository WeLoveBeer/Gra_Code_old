#include "scanfilllibrary.h"
#include <QtAlgorithms>
#include <QPainter>
#include <qglobal.h>
#define BOUND_MAX 30.0
double areaMinX = 0.0;
double areaMinY = 0.0;
ScanFillLibrary::ScanFillLibrary(Polygon polygon,double precision,QVector<QVector<Grid*>>& map,QVector<Grid*>& areaMap)
    :m_pMap(map),
      m_pAreaMap(areaMap),
    //m_boundPointScale(10),
    m_boundMax(BOUND_MAX),
    m_isFirst(true),
	m_areaPointCount(0)
{
    m_xmax = 0.0;
    m_xmin = 0.0;
    m_ymax = 0.0;
    m_ymin = 0.0;
    m_polygon = polygon;
    m_precision = precision;
    //�±߱���ʱ10000��С
    m_newEdgeTable.resize(5000);


}
//��ȡ�߽������Сֵ
void ScanFillLibrary::GetPolygonMinMax(bool isFirst)
{
    if(m_polygon.GetPolyCount()==0)
        return;
    double ymin_temp = INT_MAX;
    double ymax_temp = INT_MIN;
    double xmin_temp = INT_MAX;
    double xmax_temp = INT_MIN;

    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].y()<ymin_temp)
            ymin_temp = m_polygon[i].y();
        if(m_polygon[i].y()>ymax_temp)
            ymax_temp = m_polygon[i].y();
        if(m_polygon[i].x()<xmin_temp)
            xmin_temp = m_polygon[i].x();
        if(m_polygon[i].x()>xmax_temp)
            xmax_temp = m_polygon[i].x();

    }
    if(isFirst)
    {
        m_ymin = ymin_temp;
        m_ymax = ymax_temp;
        m_xmax = xmax_temp;
        m_xmin = xmin_temp;
        areaMinX = m_xmin;
        areaMinY = m_ymin;
        isFirst = false;
    }
    else
    {
        m_ymin = areaMinY;
        m_ymax = ymax_temp;
        m_xmax = xmax_temp;
        m_xmin = areaMinX;
    }

}
//��ʼ���±߱�
void ScanFillLibrary::InitScanLineNewEdgeTable()
{
    Edge e;
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        const Point& curPoint = m_polygon[i];//��ǰ��
        const Point& nextPoint = m_polygon[(i + 1) % m_polygon.GetPolyCount()];//��һ����
        const Point& pss = m_polygon[(i - 1 + m_polygon.GetPolyCount()) %m_polygon.GetPolyCount()];//��һ����
        const Point& pee = m_polygon[(i + 2) % m_polygon.GetPolyCount()];//���¸���

        if(nextPoint.y() != curPoint.y()) //������ˮƽ��
        {
            //б�ʣ����б�ʵ���
            e.dx = double(nextPoint.x() - curPoint.x()) / double(nextPoint.y() - curPoint.y());
            if(nextPoint.y() > curPoint.y())
            {
                e.xi = curPoint.x();
                if(pee.y() >= nextPoint.y())
                    e.ymax = nextPoint.y() - 1;
                else
                    e.ymax = nextPoint.y() ;

                m_newEdgeTable[curPoint.y()].push_back(e);
            }
            else
            {
                e.xi = nextPoint.x();
                if(pss.y() >= curPoint.y())
                    e.ymax = curPoint.y() - 1;
                else
                    e.ymax = curPoint.y();

                m_newEdgeTable[nextPoint.y()].push_back(e);
            }
        }
    }

}
//�����㷨
void ScanFillLibrary::ProcessScanLineFill(int mode)
{
    Clean();
    GetPolygonMinMax(m_isFirst);
    if(m_isFirst)
        m_isFirst = false;
	//�����߽�
    RestructureBounds();
	//��ʼ���±߱�
    InitScanLineNewEdgeTable();
    for(int y = 0;y < m_restructedMaxY;y+=1)
    {
        //QVector<Grid> linePoints;
        //ɾ��������Ҫ��ı߱�ڵ�
        for(int i=m_activeEdgeTable.size()-1;i>=0;i--)
        {
            if(m_activeEdgeTable[i].ymax<y)
            {
                m_activeEdgeTable.removeAt(i);
            }
        }
        //���ӵ���������

        if(y >= 0 && m_newEdgeTable[y].size()!=0)
        {
            for(int i=0;i<m_newEdgeTable[y].size();i++)
            {
                 m_activeEdgeTable.push_back(m_newEdgeTable[y][i]);
            }
       }

        //���򣬸��ݽ����±�ֵ
        qSort(m_activeEdgeTable);
        //ɨ��
        for(int i=0;i<m_activeEdgeTable.size();i+=2)
        {
            for(int j=m_activeEdgeTable[i].xi;j<m_activeEdgeTable[i+1].xi;j+=1)
            {
				if (j >= 1000 || y >= 1000)
					continue;
				switch (mode)
				{
				case Mode::AREA_MODE:
					//��ɨ�赽�ĵ�ŵ�tempPoint��
					tempPoint.push_back(Point(y, j));

					m_pMap[j][y]->m_totalOverlayCount = 0;
					m_pAreaMap.push_back(m_pMap[j][y]);
					break;
				case Mode::ADD_MODE:
					tempPoint.push_back(Point(y, j));
					m_pMap[j][y]->m_totalOverlayCount += 1;
					m_pMap[j][y]->m_stripSet.insert(m_stripID);
										
					break;
				case Mode::DECREASE_MODE:
				{
					if (qFind(m_pAreaMap, m_pMap[j][y]) != m_pAreaMap.end())
					{
						m_pMap[j][y]->m_totalOverlayCount -= 1;

						//_ASSERTE(m_pMap[j][y]->m_totalOverlayCount >= 0);
						m_pMap[j][y]->m_stripSet.remove(m_stripID);
						auto index = qFind(m_pAreaMap, m_pMap[j][y]);
						auto index1 = const_cast<Grid**>(index);
						m_pAreaMap.erase(index1);
						tempPoint.push_back(Point(y, j));
						
					}
					
					break;
				}
					
				case Mode::JUDGE_MODE:
					if ((m_pMap[j][y]->m_totalOverlayCount == 1) && qFind(m_pAreaMap, m_pMap[j][y]) != m_pAreaMap.end())
						m_areaPointCount += 10000;
					else if ((m_pMap[j][y]->m_totalOverlayCount == 2) && qFind(m_pAreaMap, m_pMap[j][y]) != m_pAreaMap.end())
						m_areaPointCount += 1;
					break;

				
				case Mode::REMOVE_VISIBLE:
					tempPoint.push_back(Point(y, j));
					m_pMap[j][y]->m_totalOverlayCount -= 1;
					break;
                }

            }
        }
        //���½���ֵ
        for(int i=0;i<m_activeEdgeTable.size();i++)
        {
            m_activeEdgeTable[i].xi += 1*m_activeEdgeTable[i].dx;
        }

    }
}
//�ж��Ƿ�ȫ����
bool ScanFillLibrary::isFullCoverageOnce()
{
    for(int i=0;i<m_pAreaMap.size();i++)
    {
        if(m_pAreaMap[i]->m_totalOverlayCount==0)
            return false;
    }
    return true;
}
void ScanFillLibrary::Clean()
{
    m_activeEdgeTable.clear();
    m_newEdgeTable.clear();
    tempPoint.clear();
    m_newEdgeTable.resize(5000);
	m_areaPointCount = 0;
}

//�Ѿ�γ��ӳ�䵽2d�����ϣ��ֱ���x=0,y=0,x=m_boundMax,y=m_boundMax�и�����
void ScanFillLibrary::RestructureBounds()
{
    //��������0��ʼ
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        m_polygon[i].setX(m_polygon[i].x() - m_xmin);
        m_polygon[i].setY(m_polygon[i].y() - m_ymin);

    }
    //���������õ�
    //����y = 0
    bool isInnerSide = false;//�Ƿ���x�Ϸ���y�ҷ�
    /*****************************************************************************��*/
    Polygon tempPolygon;
    //ȷ����һ�����λ��
    if(m_polygon[0].y() >=0)
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    else
    {
        isInnerSide = false;

    }
    //�������һ����
    m_polygon.GetPloygonVector().push_back(m_polygon[0]);
    for(int i=1;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].y() >= 0 && isInnerSide)
        {
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            continue;
        }
        else if(m_polygon[i].y() < 0 && !isInnerSide)
        {
            continue;
        }
        //���´�����
        else if(m_polygon[i].y() >= 0 && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double x = last.x()-(last.y()*(last.x()-cur.x()))/(last.y() - cur.y()+0.0);
            tempPolygon.GetPloygonVector().push_back(Point(x,0));
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            isInnerSide = true;
        }
        //���ϴ�����
        else if(m_polygon[i].y() <= 0 && isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double x = last.x()-(last.y()*(last.x()-cur.x()))/(last.y() - cur.y()+0.0);

            tempPolygon.GetPloygonVector().push_back(Point(x,0));
            isInnerSide = false;
        }
    }
    m_polygon = tempPolygon;
	if (m_polygon.GetPolyCount() == 0)return;
    /************************************************************************��*/
    tempPolygon.GetPloygonVector().clear();
    //ȷ����һ�����λ��
    if(m_polygon[0].x() >=0)
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    else
    {
        isInnerSide = false;

    }
    //�������һ����
    m_polygon.GetPloygonVector().push_back(m_polygon[0]);
    for(int i=1;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].x() >= 0 && isInnerSide)
        {
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            continue;
        }
        else if(m_polygon[i].x() < 0 && !isInnerSide)
        {
            continue;
        }
        //���󴩵���
        else if(m_polygon[i].x() >= 0 && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double y = last.y()-last.x()*((last.y()-cur.y())/(last.x()-cur.x()));
            tempPolygon.GetPloygonVector().push_back(Point(0,y));
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            isInnerSide = true;
        }
        //���Ҵ�����
        else if(m_polygon[i].x() < 0 && isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double y = last.y()-last.x()*((last.y()-cur.y())/(last.x()-cur.x()));

            tempPolygon.GetPloygonVector().push_back(Point(0,y));
            isInnerSide = false;
        }

    }
    m_polygon = tempPolygon;
	if (m_polygon.GetPolyCount() == 0)return;
    /****************************************************************************��*/
    tempPolygon.GetPloygonVector().clear();
    //ȷ����һ�����λ��
    if(m_polygon[0].y() >= m_boundMax)
    {
        isInnerSide = false;
    }
    else
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    //�������һ����
    m_polygon.GetPloygonVector().push_back(m_polygon[0]);
    for(int i=1;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].y() < m_boundMax && isInnerSide)
        {
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            continue;
        }
        else if(m_polygon[i].y() >= m_boundMax && !isInnerSide)
        {
            continue;
        }
        //���ϴ�����
        else if(m_polygon[i].y() < m_boundMax && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double k = (last.y()-cur.y())/(last.x()-cur.x());
            double y = (m_boundMax-last.y())/k + last.x();
            tempPolygon.GetPloygonVector().push_back(Point(y,m_boundMax));
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            isInnerSide = true;
        }
        //���´�����
        else if(m_polygon[i].y() >= m_boundMax && isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double k = (last.y()-cur.y())/(last.x()-cur.x());
            double y = (m_boundMax-last.y())/k + last.x();

            tempPolygon.GetPloygonVector().push_back(Point(y,m_boundMax));
            isInnerSide = false;
        }
    }
    m_polygon = tempPolygon;
	if (m_polygon.GetPolyCount() == 0)return;
    /*************************************************************************��*/
    tempPolygon.GetPloygonVector().clear();
    //ȷ����һ�����λ��
    if(m_polygon[0].x() >= m_boundMax)
    {
        isInnerSide = false;
    }
    else
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    //�������һ����
    m_polygon.GetPloygonVector().push_back(m_polygon[0]);
    for(int i=1;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].x() < m_boundMax && isInnerSide)
        {
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            continue;
        }
        else if(m_polygon[i].x() >= m_boundMax && !isInnerSide)
        {
            continue;
        }
        //��to��
        else if(m_polygon[i].x() < m_boundMax && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double k = (last.y()-cur.y())/(last.x()-cur.x());
            double y = k*(m_boundMax-last.x())+ last.y();

            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            tempPolygon.GetPloygonVector().push_back(Point(m_boundMax,y));
            isInnerSide = true;
        }
        //��to��
        else if(m_polygon[i].x() >= m_boundMax && isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double k = (last.y()-cur.y())/(last.x()-cur.x());
            double y = k*(m_boundMax-last.x())+ last.y();
            tempPolygon.GetPloygonVector().push_back(Point(m_boundMax,y));

            isInnerSide = false;
        }
    }
    m_polygon = tempPolygon;
	if (m_polygon.GetPolyCount() == 0)return;
    /**************************************************************/

    //����������
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        if((int(m_polygon[i].x()* 10000)%int(m_precision* 10000))/10000.0>m_precision/2)
        {
            m_polygon[i].setX(int(m_polygon[i].x()* 10000)/int(m_precision* 10000)+1);
        }
        else
        {
            m_polygon[i].setX(int(m_polygon[i].x()* 10000)/int(m_precision* 10000));
        }

        if((int(m_polygon[i].y()* 10000)%int(m_precision* 10000))/10000.0>m_precision/2)
        {
            m_polygon[i].setY(int(m_polygon[i].y()* 10000)/int(m_precision* 10000)+1);
        }
        else
        {
            m_polygon[i].setY(int(m_polygon[i].y()* 10000)/int(m_precision* 10000));
        }
    }
    //�Զ��������߽����зŴ�
    int max = 0;
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        //m_polygon[i].setX(qMin(m_polygon[i].x()*m_boundPointScale,999.0) );
        //m_polygon[i].setY(qMin(m_polygon[i].y()*m_boundPointScale,999.0));
        if(max<m_polygon[i].y())
        {
            max = m_polygon[i].y();
        }
    }
    m_restructedMaxY = max;
    /*****************************************/
    tempPolygon.GetPloygonVector().clear();
    Point last(m_polygon[0].x(),m_polygon[0].y());
    tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    for(int i=1;i<m_polygon.GetPolyCount();i++)
    {
        if(m_polygon[i].x() == last.x() && m_polygon[i].y() == last.y())
            continue;
        else
        {
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            last.setX(m_polygon[i].x());
            last.setY(m_polygon[i].y());
        }
    }
    if(m_polygon[0].x() == last.x() && m_polygon[0].y() == last.y())
        tempPolygon.GetPloygonVector().pop_back();
    m_polygon = tempPolygon;
}
















