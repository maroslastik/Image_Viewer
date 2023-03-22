#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}

ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}

void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete painter;
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}

bool ViewerWidget::isEmpty()
{
	if (img == nullptr) {
		return true;
	}

	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete painter;
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}

void ViewerWidget::setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a)
{
	r = r > 255 ? 255 : (r < 0 ? 0 : r);
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = b;
	data[startbyte + 1] = g;
	data[startbyte + 2] = r;
	data[startbyte + 3] = a;
}

void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{
	valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = static_cast<uchar>(255 * valB);
	data[startbyte + 1] = static_cast<uchar>(255 * valG);
	data[startbyte + 2] = static_cast<uchar>(255 * valR);
	data[startbyte + 3] = static_cast<uchar>(255 * valA);
}

void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (color.isValid()) {
		size_t startbyte = y * img->bytesPerLine() + x * 4;

		data[startbyte] = color.blue();
		data[startbyte + 1] = color.green();
		data[startbyte + 2] = color.red();
		data[startbyte + 3] = color.alpha();
	}
}

void ViewerWidget::setPixels_c(int x, int y, const QColor& color)
{
	setPixel(  x,  y, color);
	setPixel(  x, -y, color);
	setPixel( -x, -y, color);
	setPixel( -x,  y, color);
	setPixel(  y,  x, color);
	setPixel(  y, -x, color);
	setPixel( -y, -x, color);
	setPixel( -y,  x, color);
}

//Draw functions
void ViewerWidget::drawLineDDA(QPoint start, QPoint end, QColor color)
{
	double m;
	if ((double)(end.x() - start.x()) == 0) m = DBL_MAX;
	else m = (end.y() - start.y()) / (double)(end.x() - start.x());

	//riadiaca os je y
	if (abs(m) >= 1)
	{
		if (start.y() > end.y())
			swap_points(start, end);
		// prvy bod, y suradnica je zaokruhlena, x zaokruhlim az pri vykresleni
		double i[2] = { (double)start.x(), (int)(start.y() + 0.5) };

		setPixel((int)(i[0] + 0.5), i[1], color);
		while (i[1] != end.y())
		{
			i[0] += 1 / m;
			i[1]++;
			setPixel((int)(i[0] + 0.5), i[1], color);
		}
	}
	else
	{
		if (start.x() > end.x())
			swap_points(start, end);
		// prvy bod, x suradnica je zaokruhlena, y zaokruhlim az pri vykresleni
		double i[2] = { (int)(start.x() + 0.5), (double)start.y() };

		setPixel(i[0], (int)(i[1] + 0.5), color);
		while (i[0] != end.x())
		{
			i[0]++;
			i[1] += m;
			setPixel(i[0], (int)(i[1] + 0.5), color);
		}
	}
	update();
}

void ViewerWidget::drawLineBres(QPoint start, QPoint end, QColor color)
{
	double m;
	if ((double)(end.x() - start.x())==0) m = DBL_MAX;
	else m = (end.y() - start.y()) / (double)(end.x() - start.x());

	//riadiaca os je y
	if (m >= 1)
	{
		if (start.y() > end.y())
			swap_points(start, end);

		int k1 = 2 * (end.x() - start.x()),
			k2 = 2 * (end.x() - start.x() - end.y() + start.y()),
			p = 2 * (end.x() - start.x()) - end.y() + start.y();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], color);

		while (i[1] < end.y())
		{
			i[1]++;
			if (p > 0)
			{
				i[0]++;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], color);
		}
	}
	else if (m <= -1)
	{
		if (start.y() > end.y())
			swap_points(start, end);

		int k1 = 2 * (end.x() - start.x()),
			k2 = 2 * (end.x() - start.x() + end.y() - start.y()),
			p = 2 * (end.x() - start.x()) + end.y() - start.y();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], color);

		while (i[1] < end.y())
		{
			i[1]++;
			if (p < 0)
			{
				i[0]--;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], color);
		}
	}
	else if (m >= 0 && m < 1)
	{
		if (start.x() > end.x())
			swap_points(start, end);

		int k1 = 2 * (end.y() - start.y()),
			k2 = 2 * (end.y() - start.y() - end.x() + start.x()),
			p = 2 * (end.y() - start.y()) - end.x() + start.x();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], color);

		while (i[0] < end.x())
		{
			i[0]++;
			if (p > 0)
			{
				i[1]++;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], color);
		}
	}
	else
	{
		if (start.x() > end.x())
			swap_points(start, end);

		int k1 = 2 * (end.y() - start.y()),
			k2 = 2 * (end.y() - start.y() + end.x() - start.x()),
			p = 2 * (end.y() - start.y()) + end.x() - start.x();

		int i[2] = { start.x(), start.y() };
		setPixel(i[0], i[1], color);

		while (i[0] < end.x())
		{
			i[0]++;
			if (p < 0)
			{
				i[1]--;
				p += k2;
			}
			else
			{
				p += k1;
			}
			setPixel(i[0], i[1], color);
		}
	}

	update();
}

void ViewerWidget::rotate_polygon(float angle, QVector<QPoint> polyg)
{
	if (angle > 0)
	{
		for (int i = 1; i < get_polygon_length(); i++)
		{
			set_polygon_point(i, QPoint
				(
					(polyg[i].x() - polyg[0].x()) * cos(angle) - (polyg[i].y() - polyg[0].y()) * sin(angle) + polyg[0].x(),
					(polyg[i].x() - polyg[0].x()) * sin(angle) + (polyg[i].y() - polyg[0].y()) * cos(angle) + polyg[0].y()
				)
			);
		}
	}
	else
	{
		angle -= angle*2;
		for (int i = 1; i < get_polygon_length(); i++)
		{
			set_polygon_point(i, QPoint
				(
					(polyg[i].x() - polyg[0].x()) * cos(angle) + (polyg[i].y() - polyg[0].y()) * sin(angle) + polyg[0].x(),
					-(polyg[i].x() - polyg[0].x()) * sin(angle) + (polyg[i].y() - polyg[0].y()) * cos(angle) + polyg[0].y()
				)
			);
		}
	}
}

void ViewerWidget::scale_polygon(float scalar_x, float scalar_y)
{
	for (int i = 0; i < get_polygon_length(); i++)
	{
		QPoint S = polygon[0];
		polygon[i] -= S;

		set_polygon_point(i, QPoint
			(
				(polygon[i].x()) * scalar_x,
				(polygon[i].y()) * scalar_y
			)
		);

		polygon[i] += S;
	}
}

void ViewerWidget::shear_polygon(float dx)
{
	dx *= -1;
	for (int i = 0; i < polygon.size(); i++)
	{
		QPoint S = polygon[0];
		polygon[i] -= S;
		set_polygon_point(i,
			QPoint(
				polygon[i].x() + (int)(dx * (polygon[i].y() - polygon[0].y())+0.5),
				polygon[i].y()
			)
		);
		polygon[i] += S;
	}
}

QVector<QPoint> ViewerWidget::trim_line()
{
	if (isInside(polygon[0].x(), polygon[0].y()) && isInside(polygon[1].x(), polygon[1].y()))
	{
		return polygon;
	}

	int width = this->width()-1;
	int height = this->height()-1;
	double tl = 0, tu = 1;
	QPoint D(polygon[1] - polygon[0]);
	QPoint E[] = { QPoint(0, 0), QPoint(0, height), QPoint(width, height), QPoint(width, 0) };
	QVector<QPoint> new_line;
	new_line.resize(2);

	for (int i = 0; i < 4; i++)
	{
		QPoint W = (polygon[0]) - (E[i]);
		QPoint N = QPoint(E[(i+1)%4].y() - E[i].y(), E[i].x() - E[(i + 1) % 4].x());
		double DdotN = QPoint::dotProduct(D, N);
		if (DdotN != 0)
		{
			double t = -QPoint::dotProduct(N,W) / DdotN;

			if (DdotN > 0 && t <= 1) 
			{
				if (tl < t)
					tl = t;
			}
			else if(DdotN < 0 && t >= 0)
			{
				if (tu > t)
					tu = t;
			}
		}	
	}
	
	if (tl == 0 && tu == 1)  return polygon;
	else if (tl < tu)
	{
		new_line[0] = polygon[0] + D * tl;
		new_line[1] = polygon[0] + D * tu;
	}
	// ked presiahnem hranicu x=0 alebo y=0 tak su hodnoty rovnake, ak x=xmax, y=ymax tak tl=0 tu=1
	return new_line;
}

QVector<QPoint> ViewerWidget::trim_polygon()
{
	QVector<QPoint>	W = this->get_polygon(),
	E = {	QPoint(0, 0),	
			QPoint(width() - 1, 0), 
			QPoint(width() - 1, height() - 1), 
			QPoint(0, height() - 1) };

	for (int i = 0; i < 4; i++)
	{
		W = trim_left_side(E[i].x(),W);

		for (int i = 0; i < W.size(); i++)
		{
			W[i] = QPoint(W[i].y(), -W[i].x());
		}
		for (int i = 0; i < E.size(); i++)
		{
			E[i] = QPoint(E[i].y(), -E[i].x());
		}
	}
	return W;
}

QVector<QPoint> ViewerWidget::trim_left_side(int xmin, QVector<QPoint> V)
{
	int	width = this->width() - 1,
		height = this->height() - 1,
		n = V.size();
	QPoint S = V[n - 1];
	QVector<QPoint> W = {};
	for (int i = 0; i < n; i++)
	{
		if (V[i].x() >= xmin)
		{
			if (S.x() >= xmin)
			{
				W.push_back(V[i]);
			}
			else
			{
				QPoint P = QPoint(
					xmin,
					S.y() + ((xmin - S.x()) * ((V[i].y() - S.y()) / (V[i].x() - S.x())))
				);
				W.push_back(P);
				W.push_back(V[i]);
			}
		}
		else
		{
			if (S.x() >= xmin)
			{
				QPoint P = QPoint(
					xmin,
					S.y() + ((xmin - S.x()) * ((V[i].y() - S.y()) / (V[i].x() - S.x())))
				);
				W.push_back(P);
			}
		}
		S = V[i];
	}
	return W;
}

void ViewerWidget::fill_polygon()
{
	if (polygon.size() == 3)
	{
		QVector<QPoint> T = polygon;
		//std::sort(T.begin(), T.end(), pointLessThan);
		if (T[0].y() == T[1].y())
		{

		}
		else if (T[1].y() == T[2].y())
		{

		}
		else
		{
			double m = (T[2].y()- T[0].y()) / (T[2].x() - T[0].x());
			QPoint P = QPoint();
		}
	}
}

double ViewerWidget::max(double& one, double& two)
{ 
	if (one > two) 
		return one; 
	else 
		return two; 
}

double ViewerWidget::min(double& one, double& two) 
{
	if (one < two) 
		return one; 
	else 
		return two; 
}

void ViewerWidget::swap_points(QPoint& one, QPoint& two)
{
	QPoint tmp;
	tmp.setX(one.x());
	tmp.setY(one.y());
	one.setX(two.x());
	one.setY(two.y());
	two.setX(tmp.x());
	two.setY(tmp.y());
}

bool ViewerWidget::is_polygon_inside(QVector<QPoint> P)
{
	for (int i = 0; i < P.size(); i++)
		if (!isInside(P[i].x(), P[i].y()))
			return false;
		else
			return true;
}

void ViewerWidget::clear()
{
	polygon.clear();
	clear_canvas();
	set_object_drawn(true);
	set_c_drawn(0, false);
	set_c_drawn(1, false);
}

void ViewerWidget::clear_canvas()
{
	img->fill(Qt::white);
	update();
}

void ViewerWidget::drawCircle(QPoint centre, QPoint radius, QColor color)
{
	double r = get_c_length();
	double p1 = 1 - r;
	double x = 0, y = r;
	double dvaX = 3, dvaY = 2 * r + 2;
	while (x <= y)
	{
		int X = (int)(x + 0.5), Y = (int)(y+0.5);
		setPixel( X + centre.x(),  Y + centre.y(), color);
		setPixel( Y + centre.x(),  X + centre.y(), color);
		setPixel( X + centre.x(), -Y + centre.y(), color);
		setPixel(-Y + centre.x(),  X + centre.y(), color);
		setPixel(-X + centre.x(), -Y + centre.y(), color);
		setPixel(-X + centre.x(),  Y + centre.y(), color);
		setPixel( Y + centre.x(), -X + centre.y(), color);
		setPixel(-Y + centre.x(), -X + centre.y(), color);
		
		if (p1 > 0)
		{
			p1 -= dvaY;
			y -= 1;
			dvaY -= 2;
		}
		p1 += dvaX;
		dvaX += 2;
		x += 1;
	}
}

void ViewerWidget::scale_circle(float scalar)
{
	
	QPoint S = circle[0];
	circle[1] -= S;

	set_c_radius(QPoint
		(
			(circle[1].x()) * scalar,
			(circle[1].y()) * scalar
		)
	);

	circle[1] += S;
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}