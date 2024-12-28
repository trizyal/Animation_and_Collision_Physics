///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	main.cpp
//	------------------------
//
///////////////////////////////////////////////////

#include <QtWidgets/QApplication>
#include "SceneModel.h"
#include "AnimationCycleWidget.h"
#include <iostream>
#include <string>

int main(int argc, char **argv)
{ // main()
    // initialize QT
    QApplication app(argc, argv);


    //	create a window
    try
    { // try block
        // we want a single instance of the scene model
        SceneModel theScene;

        // create the widget with no parent
        AnimationCycleWidget animationWindow(NULL, &theScene);

        // 	set the initial size
        animationWindow.resize(1280, 720);

        // show the window
        animationWindow.show();

        // set QT running
        return app.exec();
    } // try block
    catch (std::string errorString)
    { // catch block
        std::cout << "Unable to run application." << errorString << std::endl;
    } // catch block

    // paranoid return value
    exit(0);

} // main()
