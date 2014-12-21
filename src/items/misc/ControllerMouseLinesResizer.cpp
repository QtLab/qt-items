#include "ControllerMouseLinesResizer.h"
#include "space/Lines.h"
#include "cache/CacheView.h"

#include <QRubberBand>

namespace Qi
{

const int ToleranseZone = 3;
const int MinLineSize = 5;

ControllerMouseColumnsResizer::ControllerMouseColumnsResizer(const QSharedPointer<Lines>& columns, ControllerMousePriority priority)
    : ControllerMouseCaptured(priority),
      m_columns(columns),
      m_delta(0),
      m_position(0),
      m_trackPosition(0),
      m_columnIndex(InvalidIndex),
      m_rubberBand(nullptr)
{
}

bool ControllerMouseColumnsResizer::processMouseMove(QMouseEvent* event)
{
    if (m_rubberBand)
    {
        QRect rect = activationState().context.widget->rect();
        rect.setLeft(event->x());
        rect.setRight(event->x());
        m_rubberBand->setGeometry(rect);

        return true;
    }

    return ControllerMouseCaptured::processMouseMove(event);
}

bool ControllerMouseColumnsResizer::acceptImpl(const ActivationInfo& activationInfo) const
{
    m_delta = activationInfo.cache.cacheView.rect().right() - activationInfo.context.point.x();
    if (abs(m_delta) <= ToleranseZone)
        return true;

    return false;
}

void ControllerMouseColumnsResizer::activateImpl(const ActivationInfo& activationInfo)
{
    ControllerMouseCaptured::activateImpl(activationInfo);

    m_position = activationInfo.cache.itemRect.topLeft().x();
    m_trackPosition = activationInfo.context.point.x();
    m_columnIndex = activationInfo.item().column;

    m_oldCursor = activationState().context.widget->cursor();
    activationState().context.widget->setCursor(Qt::SplitHCursor);
    qDebug() << "ControllerMouseColumnsResizer::activateImpl" << m_oldCursor << QCursor(Qt::SplitHCursor);
}

void ControllerMouseColumnsResizer::deactivateImpl()
{
    m_position = 0;
    m_trackPosition = 0;
    m_columnIndex = InvalidIndex;
    activationState().context.widget->setCursor(m_oldCursor);
    qDebug() << "ControllerMouseColumnsResizer::deactivateImpl" << m_oldCursor;

    ControllerMouseCaptured::deactivateImpl();
}

void ControllerMouseColumnsResizer::startCapturingImpl()
{
    ControllerMouseCaptured::startCapturingImpl();

    Q_ASSERT(!m_rubberBand);
    auto widget = activationState().context.widget;

    m_rubberBand = new QRubberBand(QRubberBand::Line, widget);

    QRect rect = widget->rect();
    rect.setLeft(activationState().point().x());
    rect.setRight(activationState().point().x());
    m_rubberBand->setGeometry(rect);
    m_rubberBand->show();
}

void ControllerMouseColumnsResizer::stopCapturingImpl()
{
    Q_ASSERT(m_rubberBand);
    delete m_rubberBand;
    m_rubberBand = nullptr;

    ControllerMouseCaptured::stopCapturingImpl();
}

void ControllerMouseColumnsResizer::applyImpl()
{
    Q_ASSERT(isActive());
    Q_ASSERT(m_columnIndex != InvalidIndex);

    int size = std::max<int>(MinLineSize, activationState().point().x() - m_position + m_delta);
    m_columns->setLineSize(m_columnIndex, size);
}

ControllerMouseRowsResizer::ControllerMouseRowsResizer(const QSharedPointer<Lines>& rows, ControllerMousePriority priority)
    : ControllerMouseCaptured(priority),
      m_rows(rows),
      m_delta(0),
      m_position(0),
      m_trackPosition(0),
      m_rowIndex(InvalidIndex),
      m_rubberBand(nullptr)
{
}

bool ControllerMouseRowsResizer::processMouseMove(QMouseEvent* event)
{
    if (m_rubberBand)
    {
        QRect rect = activationState().context.widget->rect();
        rect.setTop(event->y());
        rect.setBottom(event->y());
        m_rubberBand->setGeometry(rect);

        return true;
    }

    return ControllerMouseCaptured::processMouseMove(event);
}

bool ControllerMouseRowsResizer::acceptImpl(const ActivationInfo& activationInfo) const
{
    m_delta = activationInfo.cache.cacheView.rect().bottom() - activationInfo.context.point.y();
    if (abs(m_delta) < ToleranseZone)
        return true;

    return false;
}

void ControllerMouseRowsResizer::activateImpl(const ActivationInfo& activationInfo)
{
    ControllerMouseCaptured::activateImpl(activationInfo);

    m_position = activationInfo.cache.itemRect.topLeft().y();
    m_trackPosition = activationInfo.context.point.y();
    m_rowIndex = activationInfo.item().row;

    m_oldCursor = activationState().context.widget->cursor();
    activationState().context.widget->setCursor(Qt::SplitVCursor);
    qDebug() << "ControllerMouseRowsResizer::activateImpl" << m_oldCursor << QCursor(Qt::SplitVCursor);
}

void ControllerMouseRowsResizer::deactivateImpl()
{
    m_position = 0;
    m_trackPosition = 0;
    m_rowIndex = InvalidIndex;
    activationState().context.widget->setCursor(m_oldCursor);

    qDebug() << "ControllerMouseRowsResizer::deactivateImpl" << m_oldCursor;

    ControllerMouseCaptured::deactivateImpl();
}

void ControllerMouseRowsResizer::startCapturingImpl()
{
    ControllerMouseCaptured::startCapturingImpl();

    Q_ASSERT(!m_rubberBand);
    auto widget = activationState().context.widget;

    m_rubberBand = new QRubberBand(QRubberBand::Line, widget);

    QRect rect = widget->rect();
    rect.setTop(activationState().point().y());
    rect.setBottom(activationState().point().y());
    m_rubberBand->setGeometry(rect);
    m_rubberBand->show();
}

void ControllerMouseRowsResizer::stopCapturingImpl()
{
    Q_ASSERT(m_rubberBand);
    delete m_rubberBand;
    m_rubberBand = nullptr;

    ControllerMouseCaptured::stopCapturingImpl();
}

void ControllerMouseRowsResizer::applyImpl()
{
    Q_ASSERT(isActive());
    Q_ASSERT(m_rowIndex != InvalidIndex);

    int size = std::max<int>(MinLineSize, activationState().point().y() - m_position + m_delta);
    m_rows->setLineSize(m_rowIndex, size);
}

} // end namespace Qi
