#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QColor globalColor;
	QSettings settings;
	QMessageBox msgBox;

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);
	bool openVTK(ViewerWidget* w, QString filename);

	void draw_Polygon(ViewerWidget* w, QMouseEvent* e);
	void redraw_Polygon(ViewerWidget* w, QVector<VERTEX> polyg, QColor color);

	void draw_circle(ViewerWidget* w, QMouseEvent* e);
	void redraw_circle(ViewerWidget* w, QPoint centre, QPoint radius);

	void redraw_object();

private slots:
	void on_rotateButton_clicked();
	void on_scaleButton_clicked();
	void on_symmX_clicked();
	void on_symmY_clicked();
	void on_shearDXbutton_clicked();

	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionExit_triggered();
	void on_actionImage_triggered();
	void on_actionVTKfile_triggered();
	void on_resetButton_clicked();
	void on_zenitSlider_valueChanged();
	void on_azimutSlider_valueChanged();
	void on_szSlider_valueChanged();
	void onOsvetleneButtonToggled();
	void onWireframeButtonToggled();

	//Tools slots
	void on_pushButtonSetColor_clicked();
};