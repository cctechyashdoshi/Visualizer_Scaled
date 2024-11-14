#include <QFileDialog>
#include <QGridLayout>
#include "Visualizer.h"
#include "STLReader.h"
#include "OBJReader.h"
#include "OBJWriter.h"
#include "STLWriter.h"
#include "DataWriter.h"
#include "Transformation.h"
#include "Matrix4x4.h"
int Vcount = 0;

Visualizer::Visualizer(QWidget* parent) : QMainWindow(parent)
{
    setupUi();
    connect(loadFile, &QPushButton::clicked, this, &Visualizer::onLoadFileClick);
    connect(translate, &QPushButton::clicked, this, &Visualizer::onTranslateClick);
    connect(exportFile, &QPushButton::clicked, this, &Visualizer::onExportClick);
}

Visualizer::~Visualizer()
{
}

void Visualizer::setupUi()
{
    loadFile = new QPushButton("Load File", this);
    translate = new QPushButton("Translate", this);
    exportFile = new QPushButton("Export", this);
    openglWidgetInput = new OpenGlWidget(this);
    openglWidgetOutput = new OpenGlWidget(this);
    progressBar = new QProgressBar(this);

    graphicsSynchronizer = new GraphicsSynchronizer(openglWidgetInput, openglWidgetOutput);

    QString buttonStyle = "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    font-size: 16px;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3e8e41;"
        "}";

    QString widgetStyle = "QWidget {"
        "    background-color: #333;"
        "    color: #f5f5f5;"
        "}";

    loadFile->setStyleSheet(buttonStyle);
    translate->setStyleSheet(buttonStyle);
    exportFile->setStyleSheet(buttonStyle);

    this->setStyleSheet(widgetStyle);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(loadFile, 0, 0, 1, 2);
    layout->addWidget(translate, 0, 2, 1, 2);
    layout->addWidget(exportFile, 0, 4, 1, 2);
    layout->addWidget(openglWidgetInput, 1, 0, 1, 3);
    layout->addWidget(openglWidgetOutput, 1, 3, 1, 3);
    layout->addWidget(progressBar, 2, 0, 1, 6);
    layout->setContentsMargins(20, 20, 20, 20);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void Visualizer::onLoadFileClick()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("files (*.stl *.obj)"));
    if (!fileName.isEmpty())
    {
        inputFilePath = fileName;
        triangulation = readFile(inputFilePath);
        OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(triangulation);
        openglWidgetInput->setData(data);
    }
}

void Visualizer::onTranslateClick()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        exportFileName = dir + (inputFilePath.endsWith(".stl", Qt::CaseInsensitive) ? "/output.obj" : "/output.stl");
        Transformer::Transformation TransObj;
        Geometry::Matrix4x4 matObj;
		//-----------------Scaling-------------------
        //Triangulation scaledTriangulation = TransObj.scale(triangulation, 100.0, 2.0, 3.0);
        //writeFile(exportFileName, scaledTriangulation);
        //outputTriangulation = readFile(exportFileName);
        //OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(outputTriangulation);
        //openglWidgetOutput->setData(data);
		//-----------------Translation-------------------
        Triangulation translatedtriangulation = TransObj.translate(triangulation, 1.0, 2.0, 3.0);
        writeFile(exportFileName, translatedtriangulation);
        outputTriangulation = readFile(exportFileName);
        OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(outputTriangulation);
        openglWidgetOutput->setData(data);
		//-----------------Rotation-------------------
        //double arr[3] = {0, 1, 2};
        //Triangulation translatedtriangulation = TransObj.rotate(triangulation, 45.0, arr);
        //writeFile(exportFileName, translatedtriangulation);
        //outputTriangulation = readFile(exportFileName);
        //OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(outputTriangulation);
        //openglWidgetOutput->setData(data);
    }
}

void Visualizer::onExportClick()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", inputFilePath.endsWith(".stl", Qt::CaseInsensitive) ? tr("files (*.obj)") : tr("files (*.stl)"));
    if (!fileName.isEmpty())
    {
        writeFile(fileName, outputTriangulation);
    }

    QFile tempFile(exportFileName);
    QFile file(fileName);
    if (file.exists())
    {
        tempFile.remove();
    }
}

Triangulation Visualizer::readFile(const QString& filePath)
{
    Triangulation tri;
    if (filePath.endsWith(".stl", Qt::CaseInsensitive))
    {
        STLReader reader;
        reader.read(filePath.toStdString(), tri);
    }
    else if (filePath.endsWith(".obj", Qt::CaseInsensitive))
    {
        OBJReader reader;
        reader.read(filePath.toStdString(), tri);
    }
    return tri;
}

void Visualizer::writeFile(const QString& filePath, const Triangulation& tri)
{
    if (filePath.endsWith(".stl", Qt::CaseInsensitive))
    {
        STLWriter writer;
        writer.Write(filePath.toStdString(), tri, progressBar);
    }
    else if (filePath.endsWith(".obj", Qt::CaseInsensitive))
    {
        ObjWriter writer;
        writer.Write(filePath.toStdString(), tri, progressBar);
    }
}

OpenGlWidget::Data Visualizer::convertTrianglulationToGraphicsObject(const Triangulation& inTriangulation)
{
    OpenGlWidget::Data data;
    for each (Triangle triangle in inTriangulation.Triangles)
    {
        for each (Point point in triangle.Points())
        {
            data.vertices.push_back(inTriangulation.UniqueNumbers[point.X()]);
            data.vertices.push_back(inTriangulation.UniqueNumbers[point.Y()]);
            data.vertices.push_back(inTriangulation.UniqueNumbers[point.Z()]);
        }

        Point normal = triangle.Normal();
        for (size_t i = 0; i < 3; i++)
        {
            data.normals.push_back(inTriangulation.UniqueNumbers[normal.X()]);
            data.normals.push_back(inTriangulation.UniqueNumbers[normal.Y()]);
            data.normals.push_back(inTriangulation.UniqueNumbers[normal.Z()]);
        }
        progressBar->setValue(Vcount);
        progressBar->setRange(0, inTriangulation.Triangles.size()-1);
        Vcount++;
    }
    return data;
}
