#pragma once
#include "Point.h"
#include<vector>
using namespace std;

namespace Geometry {
	class Matrix4x4
	{
		vector<vector<double>> identityMatrix;
	public:
		Matrix4x4();
		~Matrix4x4();
		vector<double> multiply(vector<vector<double>> transformationMatrix, vector<double> Point);
		vector<vector<double>> multiply(vector<vector<double>> firstMatrix, vector<vector<double>> secondMatrix);
	};
}