#include <list>
#include <vector>
#include <algorithm>

#include "point.h"

point evaluate(float t, std::list<point> P)
{
	list<point> Q = P;

	while (Q.size() > 1)
	{
		list<point> R;
		std::list<point>::iterator p1 = Q.begin();
		std::list<point>::iterator p2 = ++Q.begin();

		while (p2 != Q.end())
		{
			point p = ((1 - t) * (*p1)) + (t * (*p2));
			R.push_back(p);
			p1++;
			p2++;
		}

		Q.clear();
		Q = R;
	}

	return Q.front();
}

std::vector<point> EvaluateBezierCurve(std::vector<point>ctrl_points, int num_evaluations)
{
	std::list<point> ps(ctrl_points.begin(), ctrl_points.end());
	std::vector<point> curve;

	float offset = 1.0 / num_evaluations;
	curve.push_back(ctrl_points[0]);

	for (int e = 0; e < num_evaluations; e++)
	{
		point p = evaluate(offset * (e + 1), ps);
		curve.push_back(p);
	}

	return curve;
}

float* MakeFloatsFromVector(std::vector<point> curve, int& num_verts, int& num_floats, float r, float g, float b)
{
	num_verts = curve.size();

	if (num_verts == 0)
	{
		return NULL;
	}

	num_floats = num_verts * 6;
	float* vert_attributes = (float*) malloc(num_floats * sizeof(float));
	int i = 0;

	for (point p : curve)
	{
		vert_attributes[i] = p.x;
		vert_attributes[i+1] = p.y;
		vert_attributes[i+2] = p.z;
		vert_attributes[i+3] = r;
		vert_attributes[i+4] = g;
		vert_attributes[i+5] = b;
		i += 6;
	}

	return vert_attributes;
}