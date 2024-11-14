#pragma once
#include "Triangulation.h"
#include "Matrix4x4.h"
#include <vector>
using namespace Geometry;

namespace Transformer
{
	class Transformation
	{
	public:
		bool operator()(double a, double b) const;
		Triangulation scale(Triangulation& triangulation, double xCoord, double yCoord, double zCoord);
		Triangulation translate(Triangulation& triangulation, double xCoord, double yCoord, double zCoord);
		Triangulation rotate(Triangulation& triangulation, double theta, double axis[3]);
	};
};
