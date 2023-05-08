#pragma once
#include <QtWidgets>
#include <cmath>

class FACE;

class VERTEX
{
public:
	int x, y;
	double z;
	VERTEX() {};
	VERTEX(int n_x, int n_y, double n_z) { x = n_x; y = n_y; z = n_z; }
};

class FACE
{
public:
	QVector<VERTEX*> vertexes;
	QColor f_color;
	FACE() {};
	FACE(QVector<VERTEX*> n_vertex) { vertexes = n_vertex; }
};

class OBJECT
{
public:
	QVector<VERTEX*> vertexes;
	QVector<FACE*> faces;
	OBJECT() {};
};

class CAMERA
{
public:
	double theta, fi;
	double distance;
};

struct EDGE
{
	VERTEX start;
	VERTEX end;
	int dy;
	double x;
	double w;
};