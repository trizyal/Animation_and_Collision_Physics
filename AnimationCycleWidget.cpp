///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	AnimationCycleWidget.h
//	------------------------
//	
//	The main widget
//	
///////////////////////////////////////////////////

#ifdef slots
#undef slots
#endif

#ifdef signals
#undef signals
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define slots Q_SLOTS
#define signals Q_SIGNALS

#include "AnimationCycleWidget.h"
#include "SceneModel.h"

// constructor
AnimationCycleWidget::AnimationCycleWidget(QWidget *parent, SceneModel *TheScene)
    : QOpenGLWidget(parent),
    theScene(TheScene)
    { // constructor

	// we want to create a timer for forcing animation
	animationTimer = new QTimer(this);
	// connect it to the desired slot
	connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	// set the timer to fire 60 times a second
	// animationTimer->start((double)16.6667);

	// set the timer to fire 24 times a second
	animationTimer->start((double)1000/24);
    } // constructor

// destructor
AnimationCycleWidget::~AnimationCycleWidget()
	{ // destructor
	// nothing yet
	} // destructor																	

// called when OpenGL context is set up
void AnimationCycleWidget::initializeGL()
	{ // AnimationCycleWidget::initializeGL()
	} // AnimationCycleWidget::initializeGL()

// called every time the widget is resized
void AnimationCycleWidget::resizeGL(int w, int h)
	{ // AnimationCycleWidget::resizeGL()
	// reset the viewport
	glViewport(0, 0, w, h);
	
	// set projection matrix based on zoom & window size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// compute the aspect ratio of the widget
	float aspectRatio = (float) w / (float) h;
	
	// we want a 90 degree vertical field of view, as wide as the window allows
	// and we want to see from just in front of us to 100km away
	gluPerspective(90.0, aspectRatio, 0.1, 100000);

	// set model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	} // AnimationCycleWidget::resizeGL()
	
// called every time the widget needs painting
void AnimationCycleWidget::paintGL()
	{ // AnimationCycleWidget::paintGL()
	// call the scene to render itself
	theScene->Render();
	} // AnimationCycleWidget::paintGL()

// called when a key is pressed
void AnimationCycleWidget::keyPressEvent(QKeyEvent *event)
	{ // keyPressEvent()
	// just do a big switch statement
	switch (event->key())
		{ // end of key switch
		// exit the program
		case Qt::Key_X:
			exit(0);
			break;
		case Qt::Key_Escape:
			exit(0);
	
		// camera controls
		case Qt::Key_W:
			theScene->EventCharacterForward();
			break;

		case Qt::Key_S:
			theScene->EventCharacterBackward();
			break;
		case Qt::Key_Space:
			theScene->ResetGame();
			break;
		case Qt::Key_L:
			theScene->SwitchLand();
			break;
		case Qt::Key_M:
			theScene->SwitchModel();
			break;
				
		// just in case
		default:
			break;
		} // end of key switch
	} // keyPressEvent()

void AnimationCycleWidget::nextFrame()
	{ // nextFrame()
	// each time this gets called, we will update the scene
    theScene->Update();

	// now force an update
	update();
	} // nextFrame()

