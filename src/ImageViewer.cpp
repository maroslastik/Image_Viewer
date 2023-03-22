#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	globalColor = Qt::blue;
	QString style_sheet = QString("background-color: #%1;").arg(globalColor.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}

void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (w->get_object_drawn())
	{
		if (ui->polygonButton->isChecked())
		{
			draw_Polygon(w, e);
			w->set_object_type('p');
		}
		else if (ui->circleButton->isChecked())
		{
			draw_circle(w, e);
			w->set_object_type('c');
		}
	}
	else
	{
		w->setLastMousePosition(e->pos());
	}
}

void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}

void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (e->buttons() == Qt::LeftButton && !w->get_object_drawn()) {

		QPoint displacement = e->pos() - w->getLastMousePosition();
		// if its line
		if (w->get_object_type() == 'c')
		{
			w->set_c_centre(w->get_c_centre() + displacement);
			w->set_c_radius(w->get_c_radius() + displacement);

			redraw_circle(w,  w->get_c_centre(),w->get_c_radius());
		}
		else if (w->get_object_type() == 'p' && w->get_polygon_length() == 2)
		{
			w->set_polygon_point(0, w->get_point_polygon(0) + displacement);
			w->set_polygon_point(1, w->get_point_polygon(1) + displacement);

			if (!w->isInside(w->get_point_polygon(0).x(), w->get_point_polygon(0).y()) && 
				!w->isInside(w->get_point_polygon(1).x(), w->get_point_polygon(1).y()))
			{
				return;
			}
			
			redraw_Polygon(vW, w->trim_line());
		}
		else if(w->get_object_type() == 'p')
		{
			for (int i = 0; i < w->get_polygon_length(); i++)
			{
				w->set_polygon_point(i, w->get_point_polygon(i) + displacement);
			}

			bool trim = false;
			for (int j = 0; j < vW->get_polygon_length(); j++)
			{
				if (!w->isInside(w->get_point_polygon(j).x(), w->get_point_polygon(j).y()))
					trim = true;
			}

			if (trim)
			{
				QVector<QPoint> X = w->trim_polygon();
				redraw_Polygon(vW, X);
			}
			else
				redraw_Polygon(vW, w->get_polygon());
		}

		w->setLastMousePosition(e->pos());
	}
}

void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	QPoint delta = wheelEvent->angleDelta();
	if (vW->get_object_type() == 'c')
	{
		if (delta.y() > 0)
			vW->scale_circle(1.1);
		else if (delta.y() < 0)
			vW->scale_circle(0.9);
		redraw_circle(vW, vW->get_c_centre(), vW->get_c_radius());
	}
	else if (vW->get_object_type() == 'p')
	{
		if (delta.y() > 0)
			vW->scale_polygon(1.1, 1.1);
		else if (delta.y() < 0)
			vW->scale_polygon(0.9, 0.9);

		if (vW->get_polygon_length() == 2)
			redraw_Polygon(vW, vW->trim_line());
		else
			redraw_Polygon(vW, vW->trim_polygon());
	}
}

//ImageViewer Event
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}

bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

void ImageViewer::draw_Polygon(ViewerWidget* w, QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (w->getDrawLineActivated())
		{
			if (ui->comboBoxLineAlg->currentIndex() == 0)
				w->drawLineDDA(w->getDrawLineBegin(), e->pos(), globalColor);
			else
				w->drawLineBres(w->getDrawLineBegin(), e->pos(), globalColor);
			w->add_to_polygon(e->pos());
			w->setDrawLineBegin(e->pos());
		}
		else
		{
			if (!w->get_drawing_object())
				w->set_drawing_object(true);
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), globalColor);
			w->update();
			w->add_to_polygon(e->pos());
		}
	}
	else if (e->button() == Qt::RightButton && ui->polygonButton->isChecked())
	{
		w->setDrawLineActivated(false);
		w->set_drawing_object(false);
		if (ui->comboBoxLineAlg->currentIndex() == 0)
			w->drawLineDDA(w->getDrawLineBegin(), w->get_point_polygon(0), globalColor);
		else
			w->drawLineBres(w->getDrawLineBegin(), w->get_point_polygon(0), globalColor);
		w->set_object_drawn(false);
		w->setLastMousePosition(e->pos());
	}
	w->set_object_type('p');
}

void ImageViewer::redraw_Polygon(ViewerWidget* w, QVector<QPoint> polyg)
{
	w->clear_canvas();
	for (int i = 0; i < w->get_polygon_length(); i++)
	{
		w->drawLineDDA(polyg[i], polyg[(i + 1) % polyg.size()], globalColor);
	}
}

void ImageViewer::draw_circle(ViewerWidget* w, QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (!w->get_c_drawn(0))
		{
			w->set_drawing_object(true);
			w->set_c_centre(e->pos());
			w->set_c_drawn(0, true);
		}
		else if (!w->get_c_drawn(1))
		{
			w->set_c_radius(e->pos());
			w->set_drawing_object(false);
			w->set_object_drawn(false);
			w->set_c_drawn(1, true);
			w->setLastMousePosition(e->pos());
			redraw_circle(w, w->get_c_centre(), w->get_c_radius());
		}
	}
	w->set_object_type('c');
}

void ImageViewer::redraw_circle(ViewerWidget* w, QPoint centre, QPoint radius)
{
	if (w->get_c_centre().x() - w->get_c_length() < 0 ||
		w->get_c_centre().y() - w->get_c_length() < 0 ||
		w->get_c_centre().x() + w->get_c_length() > 500 ||
		w->get_c_centre().y() + w->get_c_length() > 500)
		qDebug() << "kruh mimo";
	else
	{
		w->clear_canvas();
		w->drawCircle(centre, radius, globalColor);
	}
	
}

//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}

void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}

void ImageViewer::on_actionClear_triggered()
{
	vW->clear();
}

void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;
	}
}

void ImageViewer::on_rotateButton_clicked() 
{
	vW->rotate_polygon(ui->spinBox_angle->value() * (M_PI / 180), vW->get_polygon()); 
	
		if (vW->get_polygon_length() == 2)
			redraw_Polygon(vW, vW->trim_line());
		else
			redraw_Polygon(vW, vW->trim_polygon());
}

void ImageViewer::on_scaleButton_clicked() 
{ 
	if (vW->get_object_type() == 'p')
	{
		vW->scale_polygon(ui->spinBox_scalar_x->value(), ui->spinBox_scalar_y->value());
		if (vW->get_polygon_length() == 2)
			redraw_Polygon(vW, vW->trim_line());
		else
			redraw_Polygon(vW, vW->trim_polygon());
	}
	else if (vW->get_object_type() == 'c')
	{
		vW->scale_circle(ui->spinBox_scalar_x->value());
		redraw_circle(vW, vW->get_c_centre(), vW->get_c_radius());
	}
	
}

void ImageViewer::on_symmX_clicked()
{
	vW->scale_polygon(-1, 1);
	if (vW->get_polygon_length() == 2)
		redraw_Polygon(vW, vW->trim_line());
	else
		redraw_Polygon(vW, vW->trim_polygon());
}

void ImageViewer::on_symmY_clicked()
{
	vW->scale_polygon(1, -1);
	if (vW->get_polygon_length() == 2)
		redraw_Polygon(vW, vW->trim_line());
	else
		redraw_Polygon(vW, vW->trim_polygon());
}

void ImageViewer::on_shearDXbutton_clicked()
{
	vW->shear_polygon(ui->shearDX->value());
	if (vW->get_polygon_length() == 2)
		redraw_Polygon(vW, vW->trim_line());
	else
		redraw_Polygon(vW, vW->trim_polygon());
}