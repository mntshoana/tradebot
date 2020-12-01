#ifndef Chart_hpp
#define Chart_hpp

#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include <QtWidgets/QRubberBand>

#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>

#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

class QGestureEvent;

QT_CHARTS_USE_NAMESPACE

//--------------------ChartView --------------

class ChartView : public QChartView {
public:
    ChartView (QChart *chart, QWidget *parent = 0);
    
protected:
    void wheelEvent (QWheelEvent *event);
    void keyPressEvent (QKeyEvent *event);
};

//----------------------Chart --------------

class Chart : public QChart {
public:
    explicit Chart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});
    
    void prepCandleStickSeries(QCandlestickSeries*& ptr, const char* name);
protected:
    bool sceneEvent(QEvent *event);
private:
    bool gestureEvent(QGestureEvent *event);
    void zoomEvent(qreal zoomFactor);
};
//------------------------------------------
#endif /* Chart_hpp */
