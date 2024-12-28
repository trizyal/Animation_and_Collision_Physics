///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	AnimationCycleWidget.h
//	------------------------
//	
//	The main widget that shows the geometry
//	
///////////////////////////////////////////////////

#ifndef _GEOMETRIC_WIDGET_H
#define _GEOMETRIC_WIDGET_H

#ifdef slots
#undef slots
#endif

#include <QtGlobal>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QTimer>
#include <QMouseEvent>

#define slots Q_SLOTS
#define signals Q_SIGNALS

#include "SceneModel.h"

class AnimationCycleWidget final : public QOpenGLWidget
	{ // class AnimationCycleWidget
	Q_OBJECT
	public:	
	// we have a single model encapsulating the scene
	SceneModel *theScene;

	// a timer for animation
	QTimer *animationTimer;

	// constructor
	explicit AnimationCycleWidget(QWidget *parent, SceneModel *TheScene);
	
	// destructor
	~AnimationCycleWidget() override;
			
	protected:
	// called when OpenGL context is set up
	void initializeGL() override;
	// called every time the widget is resized
	void resizeGL(int w, int h) override;
	// called every time the widget needs painting
	void paintGL() override;

	// called when a key is pressed
	void keyPressEvent(QKeyEvent *event) override;

	public slots:
	// slot that gets called when it's time for the next frame
	void nextFrame();
	}; // class AnimationCycleWidget

#endif
