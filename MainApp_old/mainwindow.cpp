#define PRECISION 0.015625
#define MAP_WIDTH 1001
#define AREA_TO_DELETE 1000
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QVector>
#include <polygon.h>

#include <QDebug>
#include "existstrips.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_calcStatus(0),
	m_scan(Polygon(), PRECISION, Map, AreaMap)
{
	ui->setupUi(this);
	//生成整个画面，暂时是1000*1000的
	for (int i = 0; i < MAP_WIDTH; i++)
	{
		QVector<Grid*> temp;
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			Grid* grid = new Grid();
			temp.push_back(grid);
		}
		Map.push_back(temp);
	}
	//执行算法
	//生成区域目标
	m_baseStrip = GenStripPolygon(3);
	m_baseStrip.SetID(0);
	Polygon polygon(m_baseStrip.GetBound().GetPloygonVector());
	m_scan.SetPolygon(polygon);
	//以area模式对区域进行扫描
	m_scan.ProcessScanLineFill(Mode::AREA_MODE);
	//表现层添加区域目标
	QPair<QVector<Point>, QColor> pair(QVector<Point>(m_scan.tempPoint), m_baseStrip.GetStripColor());
	m_stripsToPaint.insert(m_baseStrip.GetID(), pair);
	GenVisibleStrips();

	/*Point p1(120.08654255, 27.04708864);
	Point p2(117.79874050, 24.70077925);
	Point p3(121.42469093, 19.35119385);
	Point p4 = GenStripRec(p1,p2,p3);
	qDebug() << p4.x() << p4.y();
	QVector<Polygon> instant = GenInstantStrips(p1, p2, p3, p4);
	QColor color(10, 50, 200);
	Strip strip;
	strip.m_bound = instant[6];
	strip.m_stripColor = color;
	strip.m_ID = 6;

	m_allStrips.push_back(strip);*/
	//开始覆盖计算
	if (ProcessCoverage())
	{
		qDebug() << "Success!";
	}
	else
	{
		qDebug() << "Failed!";

	}
}

MainWindow::~MainWindow()
{
    delete ui;
}
//绘制事件
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for(auto i:m_stripsToPaint.keys())
    {
        //反走样
       painter.setRenderHint(QPainter::Antialiasing, true);
        //设置画笔颜色、宽度
       painter.setPen(QPen(m_stripsToPaint[i].second, 1));
        //设置画刷颜色
       painter.setBrush(QColor(0,255,0));
       //设置透明度
       painter.setOpacity(0.4);
       for(int j = 0;j < m_stripsToPaint[i].first.size();j++)
       {
           painter.drawPoint(m_stripsToPaint[i].first[j].y(),
                             m_stripsToPaint[i].first[j].x());
       }
    }


}
void MainWindow::SetColor(const QColor& color)
{
    m_color = color;
}
//生成多边形区域
Strip MainWindow::GenStripPolygon(int num)
{
    switch(num)
    {
        case 0:
        {
            Point point1(148,30);
            Point point2(141,42);
            Point point3(115,28);
            Point point4(125,18);
            QVector<Point> poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon polygon(poly);
            QColor color(255,10,10);
            Strip strip;
            strip.m_bound = polygon;
            strip.m_stripColor = color;
            strip.m_ID = num;
            return strip;
            break;
        }
        case 1:
        {
            Point point1(131,19);
            Point point2(124,32);
            Point point3(100,19);
            Point point4(108,7);
            QVector<Point> poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon polygon(poly);
            QColor color(215,10,10);
            Strip strip;
            strip.m_bound = polygon;
            strip.m_stripColor = color;
            strip.m_ID = num;
            return strip;
            break;
        }
        case 2:
        {
            Point point1(110,10);
            Point point2(125,10);
            Point point3(127,30);
            Point point4(105,31);


            QVector<Point> poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon polygon(poly);
            QColor color(255,10,10);
            Strip strip;
            strip.m_bound = polygon;
            strip.m_stripColor = color;
            strip.m_ID = num;
            return strip;
            break;
        }
        //area9 测试用例
    case 3:
    {
        //一个一个点挨着存放
            Point p1(120.953, 25.0605);
            Point p2(120.021, 23.3486);
            Point p3(120.271, 22.598);
            Point p4(120.889, 21.9);
            Point p5(122.11, 25.2701);

            QVector<Point> poly;
            poly.push_back(p1);
            poly.push_back(p2);
            poly.push_back(p3);
            poly.push_back(p4);
            poly.push_back(p5);
            Polygon polygon(poly);

            QColor color(10,10,255);
            Strip strip;
            strip.m_bound = polygon;
            strip.m_stripColor = color;
            strip.m_ID = 0;
            return strip;
            break;
    }

    }

}

bool MainWindow::ProcessCoverage()
{
    int maxCount = -1;
    InstantStrip resultStrip;
	QVector<Strip> tempStrip;
	//遍历所有条带
	for(int i = 0;i<m_allStrips.size();i++)
    {
		tempStrip.push_back(m_allStrips[i]);
        Polygon polygon(m_allStrips[i].m_bound.GetPloygonVector());
        m_scan.SetPolygon(polygon);
		m_scan.SetStripID(m_allStrips[i].GetID());
		//以添加模式扫描
        m_scan.ProcessScanLineFill(Mode::ADD_MODE);
		//表现层添加绘制
        QPair<QVector<Point>,QColor> pair(QVector<Point>(m_scan.tempPoint),m_allStrips[i].GetStripColor());
		m_stripsToPaint.insert(m_allStrips[i].GetID(),pair);
		//是否全覆盖
		/*if (i == 3)
			return true;*/
        while (m_scan.isFullCoverageOnce())
        {
			qDebug() << QStringLiteral("当前可视条带个数:") << tempStrip.size();
            qDebug()<< QStringLiteral("当前正计算的条带：")<<m_allStrips[i].GetID();//InstantStrip tempStrip = processInstantCoverage();
			//瞬时覆盖
			int val = processInstantCoverage(tempStrip);
			if (val ==1)
				return true;
			//如果area的点数减少到0，则覆盖完全
			if (AreaMap.size() == 0)
			{
				qDebug() << "Coverage complete!";
				return true;
			}
        }
        
    }
    return false;
}

//瞬时条带扫描
int MainWindow::processInstantCoverage(QVector<Strip>& tempStrip)
{
	Polygon resultInstantStrip;
	int innerPointCount = -1;
	int stripID = 0;
	Strip StripToDel;
	int stripIndex;
	int degree;//选择的哪个瞬时条带
	//对于当前所有的可视条带
	for (auto curStrip : tempStrip)
	{
		//产生所有瞬时条带
		QVector<Polygon> curPoly = GenInstantStrips(curStrip.GetBound()[0],
			curStrip.GetBound()[1],
			curStrip.GetBound()[2],
			curStrip.GetBound()[3]);
		//判断哪条瞬时条带能够覆盖点数最多
		for (int k=0;k<curPoly.size();k++)
		{
			m_scan.SetPolygon(curPoly[k]);
			m_scan.ProcessScanLineFill(Mode::JUDGE_MODE);
			if (m_scan.m_areaPointCount > innerPointCount)
			{
				innerPointCount = m_scan.m_areaPointCount;
				resultInstantStrip = m_scan.GetPolygon();
				degree = k;
				stripID = curStrip.m_ID;
			}
		}

	}
	qDebug() << QStringLiteral("选择的可视条带为：") << stripID<< QStringLiteral("侧摆的角度为：") << degree;
	for (int i = 0; i<tempStrip.size(); i++)
	{
		if (tempStrip[i].m_ID == stripID)
		{
			StripToDel = tempStrip[i];
			stripIndex = i;
			break;
		}
	}
	//去除已经使用过的可视条带
	m_scan.SetPolygon(StripToDel.GetBound());
	m_scan.ProcessScanLineFill(Mode::REMOVE_VISIBLE);
	//减去区域内被瞬时条带覆盖的部分
	m_scan.SetPolygon(resultInstantStrip);
	m_scan.ProcessScanLineFill(Mode::DECREASE_MODE);
	QPair<QVector<Point>, QColor> pair(QVector<Point>(m_scan.tempPoint), QColor(10, 10, 255));
	m_stripsToPaint.insert(AREA_TO_DELETE, pair);
	auto areaToPaint = m_stripsToPaint[0];
	auto stripToDelPoint = m_stripsToPaint[AREA_TO_DELETE];
	//移除用过的条带
	tempStrip.remove(stripIndex);
	//表现层移除可视条带
	if (m_stripsToPaint.contains(stripID))
	{
		m_stripsToPaint.remove(stripID);
	}
	
	//表现层移除区域内被覆盖的点
	for (int i = areaToPaint.first.size() -1; i >=0 ; i--)
	{
		for (int j = 0; j < stripToDelPoint.first.size(); j++)
		{
			if (areaToPaint.first[i].x() == stripToDelPoint.first[j].x() &&
				areaToPaint.first[i].y() == stripToDelPoint.first[j].y())
			{
				areaToPaint.first.remove(i);
				break;
			}
		}
		
	}
	m_stripsToPaint[0] = areaToPaint;

	//
	if (m_stripsToPaint.contains(AREA_TO_DELETE))
	{
		m_stripsToPaint.remove(AREA_TO_DELETE);
	}
	return stripID;
}
//区域目标的ID为0；条带从1开始增长
void MainWindow::GenVisibleStrips()
{
    QVector<QString> stripVec;
    stripVec.push_back(strip1);
    
    stripVec.push_back(strip3);
	stripVec.push_back(strip2);
    stripVec.push_back(strip4);

	//stripVec.push_back(strip8);


    for(int i = 0;i < stripVec.size();i++)
    {
        QString curStrip = stripVec[i];
        QVector<Point> poly;
        QStringList list1 = curStrip.split(';');
        for(int j = 0;j < list1.size();j++)
        {
            double lon = list1[j].split(',')[0].toDouble();
            double lat = list1[j].split(',')[1].toDouble();
            Point P(lon,lat);
            poly.push_back(P);
        }
        Polygon polygon(poly);
        QColor color;
        switch(i)
        {
        case 0:
            color = QColor(255,0,0);break;
        case 1:
            color = QColor(100,0,200);break;
        case 2:
            color = QColor(0,255,0);break;
        case 3:
            color = QColor(255,255,0);break;
        case 4:
            color = QColor(0,255,255);break;
        }
        Strip strip;
        strip.m_bound = polygon;
        strip.m_stripColor = color;
        strip.m_ID = i+1;

        m_allStrips.push_back(strip);

    }

}

Point MainWindow::GenStripRec(Point p1, Point p2, Point p3)
{
	double k1 = (p1.y() - p2.y()) / (p1.x() - p2.x());
	double k2 = (p2.y() - p3.y()) / (p2.x() - p3.x());

	double x = (k1 * p3.x() - k2 * p1.x() + p1.y() - p3.y()) / (k1 - k2);
	double y = k2 * x - k2 * p1.x() + p1.y();
	return Point(x,y);
}

//根据可视条带生成瞬时条带集合
QVector<Polygon> MainWindow::GenInstantStrips(Point p1, Point p2, Point p3, Point p4)
{
	QVector<Polygon> result;
	double offsetX = (p2.x() - p1.x()) / 9;
	double offsetY = (p2.y() - p1.y()) / 9;

	

	for (int i = 0; i <= 6; i++)
	{
		Polygon polygon;
		polygon.GetPloygonVector().push_back(Point(p1.x() + offsetX*i, p1.y() + offsetY*i));
		polygon.GetPloygonVector().push_back(Point(p1.x() + offsetX*i + offsetX * 3, p1.y() + offsetY*i + offsetY * 3));
		polygon.GetPloygonVector().push_back(Point(p4.x() + offsetX*i + offsetX * 3, p4.y() + offsetY*i + offsetY * 3));
		polygon.GetPloygonVector().push_back(Point(p4.x() + offsetX*i, p4.y() + offsetY*i));
		result.push_back(polygon);
	}
	return result;
}







