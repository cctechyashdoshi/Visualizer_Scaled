#include "Transformation.h"
#include "Matrix4x4.h"
#include <map>
#include <cmath>

#define PI 3.14159265
#define TOLERANCE 0.0000001

using namespace Transformer;
using namespace Geometry;
using namespace std;

bool Transformation::Transformation::operator()(double a, double b) const
{
    return fabs(a - b) > TOLERANCE ? a < b : false;
}

Triangulation Transformer::Transformation::scale(Triangulation& triangulation, double xCoord, double yCoord, double zCoord)
{
    vector<vector<double>> mat;
    mat = {
        {xCoord, 0.0, 0.0, 0.0},
        {0.0, yCoord, 0.0, 0.0},
        {0.0, 0.0, zCoord, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    Triangulation output;
    for (auto triangle : triangulation.Triangles)
    {
        std::vector<Point> outputPts;
        map<double, int> uniqueMap;
        Point ptArray[3] = { triangle.P1(), triangle.P2(), triangle.P3() };
        for (int p = 0; p < 3; p++)
        {
            vector<double> pointVec = { (double)ptArray[p].X(), (double)ptArray[p].Y(), (double)ptArray[p].Z(), 1.0 };
            Matrix4x4 matObj;
            vector<double> scaledPt = matObj.multiply(mat, pointVec);
            int pt[3];
            for (int i = 0; i < 3; i++)
            {
                auto pair = uniqueMap.find(scaledPt[i]);
                if (pair == uniqueMap.end())
                {
                    output.UniqueNumbers.push_back(scaledPt[i]);
                    uniqueMap[scaledPt[i]] = static_cast<int>(output.UniqueNumbers.size() - 1);
                    pt[i] = output.UniqueNumbers.size() - 1;
                }
                else
                {
                    pt[i] = pair->second;
                }
            }
            outputPts.push_back(Point(pt[0], pt[1], pt[2]));
        }
        output.Triangles.push_back(Triangle(triangle.Normal(), outputPts[0], outputPts[1], outputPts[2]));
    }
    return output;
}

Triangulation Transformer::Transformation::translate(Triangulation& triangulation, double xCoord, double yCoord, double zCoord)
{
    vector<vector<double>> mat = {
        {1.0, 0.0, 0.0, xCoord},
        {0.0, 1.0, 0.0, yCoord},
        {0.0, 0.0, 1.0, zCoord},
        {0.0, 0.0, 0.0, 1.0}
    };

    Triangulation output;
    Matrix4x4 matObj;

    for (auto& triangle : triangulation.Triangles)
    {
        std::vector<Point> outputPts;
        map<tuple<int, int, int>, int> uniqueMap;
        Point ptArray[3] = { triangle.P1(), triangle.P2(), triangle.P3() };

        for (int p = 0; p < 3; p++)
        {
            vector<double> pointVec = { (double)ptArray[p].X(), (double)ptArray[p].Y(), (double)ptArray[p].Z(), 1.0 };
            vector<double> translatedPt = matObj.multiply(mat, pointVec);

            // Use a tuple of rounded x, y, z for unique mapping to avoid floating-point precision issues
            auto key = make_tuple(static_cast<int>(round(translatedPt[0] * 1000)), static_cast<int>(round(translatedPt[1] * 1000)), static_cast<int>(round(translatedPt[2] * 1000)));
            if (uniqueMap.find(key) == uniqueMap.end())
            {
                output.UniqueNumbers.push_back(translatedPt[0]);
                output.UniqueNumbers.push_back(translatedPt[1]);
                output.UniqueNumbers.push_back(translatedPt[2]);
                uniqueMap[key] = static_cast<int>(output.UniqueNumbers.size() / 3 - 1);
            }

            int index = uniqueMap[key];
            outputPts.push_back(Point(output.UniqueNumbers[index * 3], output.UniqueNumbers[index * 3 + 1], output.UniqueNumbers[index * 3 + 2]));
        }

        output.Triangles.push_back(Triangle(triangle.Normal(), outputPts[0], outputPts[1], outputPts[2]));
    }

    return output;
}


Triangulation Transformer::Transformation::rotate(Triangulation& triangulation, double theta, double axis[3])
{
    vector<vector<double>> xMatrix;
	vector<vector<double>> yMatrix;
	vector<vector<double>> zMatrix;
    Triangulation output;
    Matrix4x4 matObj;

    xMatrix = {
		{1.0, 0.0, 0.0, 0.0},
		{0.0, cos((theta*PI)/180)*axis[0], -sin(theta * PI) * axis[0], 0.0},
		{0.0, sin(theta * PI) * axis[0], cos(theta * PI) * axis[0], 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

	yMatrix = {
		{cos(theta * PI) * axis[1], 0.0, sin(theta * PI) * axis[1], 0.0},
		{0.0, 1.0, 0.0, 0.0},
		{-sin(theta * PI) * axis[1], 0.0, cos(theta * PI) * axis[1], 0.0},
		{0.0, 0.0, 0.0, 1.0}
	};

    zMatrix = {
        {cos(theta * PI) * axis[2], -sin(theta * PI) * axis[2], 0.0, 0.0},
        {sin(theta * PI) * axis[2], cos(theta * PI) * axis[2], 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0}
    };

    vector<vector<double>> matrixOne = matObj.multiply(xMatrix, yMatrix);
    vector<vector<double>> matrixTwo = matObj.multiply(matrixOne, yMatrix);

    for (auto triangle : triangulation.Triangles)
    {
        std::vector<Point> outputPts;
        map<double, int> uniqueMap;
        Point ptArray[3] = { triangle.P1(), triangle.P2(), triangle.P3() };
        for (int p = 0; p < 3; p++)
        {
            vector<double> pointVec = { (double)ptArray[p].X(), (double)ptArray[p].Y(), (double)ptArray[p].Z(), 1.0 };
            vector<double> scaledPt = matObj.multiply(matrixTwo, pointVec);
            int pt[3];
            for (int i = 0; i < 3; i++)
            {
                auto pair = uniqueMap.find(scaledPt[i]);
                if (pair == uniqueMap.end())
                {
                    output.UniqueNumbers.push_back(scaledPt[i]);
                    uniqueMap[scaledPt[i]] = static_cast<int>(output.UniqueNumbers.size() - 1);
                    pt[i] = output.UniqueNumbers.size() - 1;
                }
                else
                {
                    pt[i] = pair->second;
                }
            }
            outputPts.push_back(Point(pt[0], pt[1], pt[2]));
        }
        output.Triangles.push_back(Triangle(triangle.Normal(), outputPts[0], outputPts[1], outputPts[2]));
    }
    return output;
}
