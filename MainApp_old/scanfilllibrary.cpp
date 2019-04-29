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
    //新边表暂时10000大小
    m_newEdgeTable.resize(5000);


}
//获取边界最大最小值
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
//初始化新边表
void ScanFillLibrary::InitScanLineNewEdgeTable()
{
    Edge e;
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        const Point& curPoint = m_polygon[i];//当前点
        const Point& nextPoint = m_polygon[(i + 1) % m_polygon.GetPolyCount()];//下一个点
        const Point& pss = m_polygon[(i - 1 + m_polygon.GetPolyCount()) %m_polygon.GetPolyCount()];//上一个点
        const Point& pee = m_polygon[(i + 2) % m_polygon.GetPolyCount()];//下下个点

        if(nextPoint.y() != curPoint.y()) //不处理水平线
        {
            //斜率，存放斜率倒数
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
//核心算法
void ScanFillLibrary::ProcessScanLineFill(int mode)
{
    Clean();
    GetPolygonMinMax(m_isFirst);
    if(m_isFirst)
        m_isFirst = false;
	//规整边界
    RestructureBounds();
	//初始化新边表
    InitScanLineNewEdgeTable();
    for(int y = 0;y < m_restructedMaxY;y+=1)
    {
        //QVector<Grid> linePoints;
        //删除不满足要求的边表节点
        for(int i=m_activeEdgeTable.size()-1;i>=0;i--)
        {
            if(m_activeEdgeTable[i].ymax<y)
            {
                m_activeEdgeTable.removeAt(i);
            }
        }
        //增加到活性链表

        if(y >= 0 && m_newEdgeTable[y].size()!=0)
        {
            for(int i=0;i<m_newEdgeTable[y].size();i++)
            {
                 m_activeEdgeTable.push_back(m_newEdgeTable[y][i]);
            }
       }

        //排序，根据交点下标值
        qSort(m_activeEdgeTable);
        //扫描
        for(int i=0;i<m_activeEdgeTable.size();i+=2)
        {
            for(int j=m_activeEdgeTable[i].xi;j<m_activeEdgeTable[i+1].xi;j+=1)
            {
				if (j >= 1000 || y >= 1000)
					continue;
				switch (mode)
				{
				case Mode::AREA_MODE:
					//把扫描到的点放到tempPoint中
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
        //更新交点值
        for(int i=0;i<m_activeEdgeTable.size();i++)
        {
            m_activeEdgeTable[i].xi += 1*m_activeEdgeTable[i].dx;
        }

    }
}
//判断是否全覆盖
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

//把经纬度映射到2d画布上，分别用x=0,y=0,x=m_boundMax,y=m_boundMax切割条带
void ScanFillLibrary::RestructureBounds()
{
    //规整到从0开始
    for(int i=0;i<m_polygon.GetPolyCount();i++)
    {
        m_polygon[i].setX(m_polygon[i].x() - m_xmin);
        m_polygon[i].setY(m_polygon[i].y() - m_ymin);

    }
    //规整条带用的
    //处理y = 0
    bool isInnerSide = false;//是否在x上方或y右方
    /*****************************************************************************下*/
    Polygon tempPolygon;
    //确定第一个点的位置
    if(m_polygon[0].y() >=0)
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    else
    {
        isInnerSide = false;

    }
    //处理最后一条边
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
        //从下穿到上
        else if(m_polygon[i].y() >= 0 && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double x = last.x()-(last.y()*(last.x()-cur.x()))/(last.y() - cur.y()+0.0);
            tempPolygon.GetPloygonVector().push_back(Point(x,0));
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            isInnerSide = true;
        }
        //从上穿到下
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
    /************************************************************************左*/
    tempPolygon.GetPloygonVector().clear();
    //确定第一个点的位置
    if(m_polygon[0].x() >=0)
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    else
    {
        isInnerSide = false;

    }
    //处理最后一条边
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
        //从左穿到右
        else if(m_polygon[i].x() >= 0 && !isInnerSide)
        {
            Point last = m_polygon[i-1];
            Point cur = m_polygon[i];
            double y = last.y()-last.x()*((last.y()-cur.y())/(last.x()-cur.x()));
            tempPolygon.GetPloygonVector().push_back(Point(0,y));
            tempPolygon.GetPloygonVector().push_back(m_polygon[i]);
            isInnerSide = true;
        }
        //从右穿到左
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
    /****************************************************************************上*/
    tempPolygon.GetPloygonVector().clear();
    //确定第一个点的位置
    if(m_polygon[0].y() >= m_boundMax)
    {
        isInnerSide = false;
    }
    else
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    //处理最后一条边
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
        //从上穿到下
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
        //从下穿到上
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
    /*************************************************************************右*/
    tempPolygon.GetPloygonVector().clear();
    //确定第一个点的位置
    if(m_polygon[0].x() >= m_boundMax)
    {
        isInnerSide = false;
    }
    else
    {
        isInnerSide = true;
        tempPolygon.GetPloygonVector().push_back(m_polygon[0]);
    }
    //处理最后一条边
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
        //右to左
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
        //左to右
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

    //规整到整数
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
    //对多边形区域边界点进行放大
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
















