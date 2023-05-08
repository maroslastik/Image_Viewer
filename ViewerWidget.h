#pragma once

#include "object.h"
#include <QtWidgets>
#include <cmath>
#include <string>


class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

	bool drawing_object = false;
	bool object_drawn = true;
	char object_type;

	bool line_was_moved = false;
	QPoint lastMousePosition;

	QVector<QPoint> polygon;
	QPoint circle[2];
	bool circle_drawn[2] = { false,false };

	

public:
	CAMERA camera;
	OBJECT object;
	double* z_buffer = nullptr;
	bool z_buffer_switch = true;
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255) {}
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.) {}
	void setPixel(int x, int y, const QColor& color) {}
	void setPixels_c(int x, int y, const QColor& color);
	void setPixel(int x, int y, float z, const QColor& color);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	// object stuff
	void setUpObject(QVector<VERTEX*> points, QVector<QVector<int>> polygons);
	void moveObjectToCentre(OBJECT object);
	void setObjectPoint(int i, VERTEX* new_p) {	object.vertexes[i] = new_p; }
	VERTEX getObjectPoint(int i) { return *object.vertexes[i]; }
	OBJECT getObject() { return object; }

	//Draw functions
	void drawLineDDA(VERTEX start, VERTEX end, QColor color);
	void drawLineBres(QPoint start, QPoint end, QColor color);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }
	void drawCircle(QPoint centre, QPoint radius, QColor color);

	// Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }
	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void set_drawing_object(bool new_b) { drawing_object = new_b; }
	bool get_drawing_object() { return drawing_object; }
	void set_object_drawn(bool new_b) { object_drawn = new_b; }
	bool get_object_drawn() { return object_drawn; }

	void set_object_type(char new_ch) { object_type = new_ch; }
	char get_object_type() { return object_type; }


	// drawing polygon
	void add_to_polygon(QPoint new_p) { polygon.append(new_p); }
	void set_polygon_point(int i, QPoint new_p) { polygon[i].setX(new_p.x()); polygon[i].setY(new_p.y()); }
	void set_polygon(const QVector<QPoint>& points) { polygon = points; }
	QVector<QPoint> get_polygon() { return polygon; }
	QPoint get_point_polygon(int i) { return polygon[i]; }
	int get_polygon_length() { return polygon.size(); }

	// moving polygon
	QPoint getLastMousePosition() const { return lastMousePosition; }
	void setLastMousePosition(const QPoint& pos) { lastMousePosition = pos; }
	bool get_line_was_moved() { return line_was_moved; }
	void set_line_was_moved(bool new_s) { line_was_moved = new_s; }
	
	// transformations of polygon
	QVector<QPoint> rotate_polygon(float angle, QVector<QPoint> polyg);
	QVector<QPoint> scale_polygon(float scalar_x, float scalar_y);
	QVector<QPoint> shear_polygon(float dx);
	QVector<QPoint> trim_line();
	QVector<VERTEX> trim_polygon(QVector<VERTEX> W);
	QVector<VERTEX> trim_left_side(int xmin, QVector<VERTEX> V);
	void fill_polygon(QVector<VERTEX> polygon, QColor color);
	void fill_triangle(QVector<VERTEX> T, QColor color);

	// drawing circle
	void set_c_centre(QPoint new_p) { circle[0] = new_p; }
	void set_c_radius(QPoint new_p) { circle[1] = new_p; }
	QPoint get_c_centre() { return circle[0]; }
	QPoint get_c_radius() { return circle[1]; }
	void set_c_drawn(int i, bool new_p) { circle_drawn[i] = new_p; }
	bool get_c_drawn(int i) { return circle_drawn[i]; }
	double get_c_length() { return sqrt(pow(circle[1].x() - circle[0].x(),2)+ pow(circle[1].y() - circle[0].y(), 2)); }

	// transformations of circle
	void scale_circle(float scalar);

	// helping functions
	void swap_points(VERTEX& one, VERTEX& two);
	float dot_product(QPoint& one, QPoint& two) { return one.x() * two.x() + one.y() * two.y(); }
	double max(double& one, double& two); 
	double min(double& one, double& two);
	bool is_polygon_inside(QVector<QPoint> P);
	void updateZBuffer(int x, int y, double z);

	void clear();
	void clear_canvas();

	

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};