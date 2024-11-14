#include "Matrix4x4.h"

using namespace Geometry;

Matrix4x4::Matrix4x4() : identityMatrix({ {1.0,0.0,0.0,0.0} ,{0.0,1.0,0.0,0.0} ,{0.0,0.0,1.0,0.0} ,{0.0,0.0,0.0,1.0} })
{
}

Matrix4x4::~Matrix4x4()
{
}

vector<double> Matrix4x4::multiply(vector<vector<double>> transformationMatrix, vector<double> Point)
{
	vector<double> transformedPoint(4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transformedPoint[j] += transformationMatrix[i][j] * Point[j];
		}
	}
	return transformedPoint;
}

vector<vector<double>> Matrix4x4::multiply(vector<vector<double>> firstMatrix, vector<vector<double>> secondMatrix) 
{
	vector<vector<double>> resultMatrix{ {0.0,0.0,0.0,0.0}, { 0.0,0.0,0.0,0.0 }, { 0.0,0.0,0.0,0.0 }, { 0.0,0.0,0.0,0.0 } };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				resultMatrix[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
			}
		}
	}
	return resultMatrix;
}
