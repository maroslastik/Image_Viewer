#pragma once
#include <QtWidgets>

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

	bool drawing_polygon = false;
	bool polygon_drawn = true;

	bool line_was_moved = false;
	QPoint lastMousePosition;

	QVector<QPoint> polygon;

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	//Draw functions
	void drawLineDDA(QPoint start, QPoint end, QColor color);
	void drawLineBres(QPoint start, QPoint end, QColor color);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }

	// Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }
	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	// drawing polygon
	void setdrawing_polygon(bool new_b) { drawing_polygon = new_b; }
	void add_to_polygon(QPoint new_p) { polygon.append(new_p); }
	void setpolygon_drawn(bool new_b) { polygon_drawn = new_b; }
	void set_polygon_point(int i, QPoint new_p) { polygon[i].setX(new_p.x()); polygon[i].setY(new_p.y()); }
	void set_polygon(const QVector<QPoint>& points) { polygon = points; }
	QVector<QPoint> get_polygon() { return polygon; }
	bool getdrawing_polygon() { return drawing_polygon; }
	QPoint get_point_polygon(int i) { return polygon[i]; }
	bool getpolygon_drawn() { return polygon_drawn; }
	int get_polygon_length() { return polygon.size(); }

	// moving polygon
	QPoint getLastMousePosition() const { return lastMousePosition; }
	void setLastMousePosition(const QPoint& pos) { lastMousePosition = pos; }
	bool get_line_was_moved() { return line_was_moved; }
	void set_line_was_moved(bool new_s) { line_was_moved = new_s; }
	
	// transformations of polygon
	void rotate_polygon(float angle, QVector<QPoint> polyg);
	void scale_polygon(float scalar_x, float scalar_y);
	void shear_polygon(float dx);
	QVector<QPoint> trim_line();
	QVector<QPoint> trim_polygon();
	QVector<QPoint> trim_left_side(int xmin, QVector<QPoint> V);
	void fill_polygon();

	// helping functions
	void swap_points(QPoint& one, QPoint& two);
	float dot_product(QPoint& one, QPoint& two) { return one.x() * two.x() + one.y() * two.y(); }
	double max(double& one, double& two); 
	double min(double& one, double& two);
	bool is_polygon_inside(QVector<QPoint> P);

	void clear();
	void clear_canvas();

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};