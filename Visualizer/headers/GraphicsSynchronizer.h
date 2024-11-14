#pragma once

#include <QObject>
#include <QVector3D>
#include <QVector2D>

class OpenGlWidget;

class GraphicsSynchronizer: public QObject
{
    Q_OBJECT

public:
    GraphicsSynchronizer(OpenGlWidget* left, OpenGlWidget* right);
    ~GraphicsSynchronizer();

private slots:
    void synchronize(float zoomLevel, QVector3D rotation, QVector2D panOffset);

private:

    OpenGlWidget* left;
    OpenGlWidget* right;
};