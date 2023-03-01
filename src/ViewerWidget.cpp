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

void ViewerWidget::clear()
{
	img->fill(Qt::white);
	polygon.clear();
	setpolygon_drawn(true);
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}