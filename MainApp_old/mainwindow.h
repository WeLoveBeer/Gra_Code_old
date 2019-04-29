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

    bool ProcessCoverage();//ִ�и�������
    int processInstantCoverage(QVector<Strip>& tempStrip);//ִ��˲ʱ����
private:
    Ui::MainWindow *ui;
	int m_calcStatus;
    void SetColor(const QColor& color);
    Strip GenStripPolygon(int num);//���ɶ����
    void GenVisibleStrips();//���ɿ�������
	Point GenStripRec(Point p1,Point p2,Point p3);
	QVector<Polygon> GenInstantStrips(Point p1, Point p2, Point p3,Point p4);

protected:
    void paintEvent(QPaintEvent *);//�����¼�
	QHash<int, QPair<QVector<Point>, QColor>> m_stripsToPaint;
    QColor m_color;
    QVector<Grid*> AreaMap;//���������е�
    QVector<QVector<Grid*> > Map;//��������
    ScanFillLibrary m_scan;//ɨ�����㷨����
    QVector<Strip> m_allStrips;//��������

	Strip m_baseStrip;
};

#endif // MAINWINDOW_H
