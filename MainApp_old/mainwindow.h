#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <polygon.h>
#include <scanfilllibrary.h>
#include "strip.h"
namespace Ui {
class MainWindow;
class QPinter;

}
class InstantStrip;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool ProcessCoverage();//执行覆盖流程
    int processInstantCoverage(QVector<Strip>& tempStrip);//执行瞬时覆盖
private:
    Ui::MainWindow *ui;
	int m_calcStatus;
    void SetColor(const QColor& color);
    Strip GenStripPolygon(int num);//生成多边形
    void GenVisibleStrips();//生成可视条带
	Point GenStripRec(Point p1,Point p2,Point p3);
	QVector<Polygon> GenInstantStrips(Point p1, Point p2, Point p3,Point p4);

protected:
    void paintEvent(QPaintEvent *);//绘制事件
	QHash<int, QPair<QVector<Point>, QColor>> m_stripsToPaint;
    QColor m_color;
    QVector<Grid*> AreaMap;//区域里所有点
    QVector<QVector<Grid*> > Map;//整个画布
    ScanFillLibrary m_scan;//扫描线算法对象
    QVector<Strip> m_allStrips;//条带集合

	Strip m_baseStrip;
};

#endif // MAINWINDOW_H
