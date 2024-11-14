#include "OpenGlWidget.h"
#include "GraphicsSynchronizer.h"

GraphicsSynchronizer::GraphicsSynchronizer(OpenGlWidget* left, OpenGlWidget* right)
    : left(left), right(right)
{
    connect(left, &OpenGlWidget::viewChange, this, &GraphicsSynchronizer::synchronize);
    connect(right, &OpenGlWidget::viewChange, this, &GraphicsSynchronizer::synchronize);
}

GraphicsSynchronizer::~GraphicsSynchronizer()
{
}

void GraphicsSynchronizer::synchronize(float zoomLevel, QVector3D rotation, QVector2D panOffset)
{
    QObject* sender = QObject::sender();
    if (left == sender)
    {
        right->sync(zoomLevel, rotation, panOffset);
    }
    else if (right == sender)
    {
        left->sync(zoomLevel, rotation, panOffset);
    }
}