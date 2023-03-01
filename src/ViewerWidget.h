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

	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }
	void setdrawing_polygon(bool new_b) { drawing_polygon = new_b; }
	void add_to_polygon(QPoint new_p) { polygon.append(new_p); }
	void setpolygon_drawn(bool new_b) { polygon_drawn = new_b; }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };
	bool getdrawing_polygon() { return drawing_polygon; }
	QPoint get_point_polygon(int i) { return polygon[i]; }
	bool getpolygon_drawn() { return polygon_drawn; }

	void swap_points(QPoint& one, QPoint& two);

	void clear();

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};