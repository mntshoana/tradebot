#include "Chart.hpp"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QGesture>

//--------------------ChartView --------------

ChartView::ChartView (QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::wheelEvent (QWheelEvent *event) {
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart()->axes(Qt::Vertical).at(0));
    QPoint numDegrees = event->angleDelta() / 16;
    numDegrees.rx() *= -1;
    if (axisY->min() + numDegrees.ry() < 0)
        numDegrees.ry() = 0;
    if (numDegrees.rx() > -2 && numDegrees.rx() < 2 && numDegrees.ry() > -2 && numDegrees.ry() < 2)
        return;
    chart()->scroll(numDegrees.rx(), numDegrees.ry());
}

void ChartView::keyPressEvent(QKeyEvent *event) {
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart()->axes(Qt::Vertical).at(0));
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        if (axisY->min() - 10 > 0)
            chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

//----------------------Chart --------------

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    setAnimationOptions(QtCharts::QChart::NoAnimation);
}

void Chart::prepCandleStickSeries(QCandlestickSeries*& ptr, const char* name){
    QCandlestickSeries* candleSeries = new QCandlestickSeries();
    candleSeries->setName(name);
    candleSeries->setIncreasingColor(QColor(110, 182, 139)); // Green
    candleSeries->setDecreasingColor(QColor(218, 89, 96)); // Red
    candleSeries->setBodyWidth(.8);
    candleSeries->setMinimumColumnWidth(0.1);
    addSeries(candleSeries);
    ptr = candleSeries;
}
bool Chart::sceneEvent(QEvent *event) {
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent *>(event));
    return QChart::event(event);
}

bool Chart::gestureEvent(QGestureEvent *event) {
    if (QGesture *gesture = event->gesture(Qt::PanGesture)) {
        QPanGesture *pan = static_cast<QPanGesture *>(gesture);
        QChart::scroll(-(pan->delta().x()), pan->delta().y());
    }

    if (QGesture *gesture = event->gesture(Qt::PinchGesture)) {
        QPinchGesture *pinch = static_cast<QPinchGesture *>(gesture);
        if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
            QChart::zoom(pinch->scaleFactor());
    }
    return true;
}
//------------------------------------------
