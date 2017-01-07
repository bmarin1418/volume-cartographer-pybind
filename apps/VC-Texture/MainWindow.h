//----------------------------------------------------------------------------------------------------------------------------------------
// MainWindow.h file for MainWindow Class , (Implements QMainWindow)
// Purpose: Create a Main Window for the GUI
// Developer: Michael Royal - mgro224@g.uky.edu
// October 12, 2015 - Spring Semester 2016
// Last Updated 11/13/2015 by: Michael Royal

// Copyright 2015 (Brent Seales: Volume Cartography Research)
// University of Kentucky VisCenter
//----------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <vector>

#include <QApplication>
#include <QCloseEvent>
#include <QObject>
#include <QtWidgets>
#include <boost/filesystem.hpp>

#include "Global_Values.h"
#include "Segmentations_Viewer.h"
#include "Texture_Viewer.h"

class MainWindow : public QMainWindow
{
    // NOTICE THIS MACRO
    Q_OBJECT
    //

public:
    // Creates a Constructor for the MainWindow Class that takes in The
    // Global_Values as a *pointer and Segmentations_Viewer as a *pointer so
    // that it can have access to these Objects.
    MainWindow(Global_Values* globals);

public slots:
    void getFilePath();
    void saveTexture();
    void exportTexture();

private:
    void clearGUI();
    void create_Actions();
    void create_Menus();
    void closeEvent(QCloseEvent* closing);

    QMenu* fileMenu;

    QAction* actionGetFilePath;
    QAction* actionSave;
    QAction* actionExport;

    Global_Values* _globals;
    Segmentations_Viewer* _segmentations_Viewer;
};
