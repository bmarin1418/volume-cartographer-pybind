//----------------------------------------------------------------------------------------------------------------------------------------
// MainWindow.cpp file for MainWindow Class , (Implements QMainWindow)
// Purpose: Create a Main Window for the GUI
// Developer: Michael Royal - mgro224@g.uky.edu
// October 12, 2015 - Spring Semester 2016
// Last Updated 11/23/2015 by: Michael Royal

// Copy Right ©2015 (Brent Seales: Volume Cartography Research) - University of Kentucky Center for Visualization and Virtualization
//----------------------------------------------------------------------------------------------------------------------------------------

#include <mainwindow.h>

MainWindow::MainWindow(Global_Values *globals)
{
    _globals = globals; // Enables access to Global Values Object

    setWindowTitle("VC Texture");// Set Window Title

    //NOTE: Minimum Height and Width -------------------------
    // will be different on other display screens,
    // if Resolution is too small may cause distortion
    // of Buttons Visually when Program first Initiates
    //----------------------------------------------------------

    //MIN DIMENSIONS
    window()->setMinimumHeight(_globals->getHeight()/2);
    window()->setMinimumWidth(_globals->getWidth()/2);
    //MAX DIMENSIONS
    window()->setMaximumHeight(_globals->getHeight());
    window()->setMaximumWidth(_globals->getWidth());
    //---------------------------------------------------------

    //Create new Texture_Viewer Object (Left Side of GUI Display)
    Texture_Viewer *texture_Image = new Texture_Viewer(globals);
    //Create new Segmentations_Viewer Object (Right Side of GUI Display)
    Segmentations_Viewer *segmentations = new Segmentations_Viewer(globals, texture_Image);
    _segmentations_Viewer = segmentations;

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(texture_Image->getLayout()); // THIS LAYOUT HOLDS THE WIDGETS FOR THE OBJECT "Texture_Viewer" which Enables the user to view images, zoom in, zoom out, and reset the image.
    mainLayout->addLayout(segmentations->getLayout()); // THIS LAYOUT HOLDS THE WIDGETS FOR THE OBJECT "Segmentations_Viewer" which Enables the user to load segmentations, and generate new texture images.

    QWidget *w = new QWidget();// Creates the Primary Widget to display GUI Functionality
    w->setLayout(mainLayout);// w(the main window) gets assigned the mainLayout

    // Display Window
    //------------------------------
    setCentralWidget(w); // w is a wrapper widget for all of the widgets in the main window.

    create_Actions(); // Creates the Actions for the Menu Bar & Sub-Menus
    create_Menus(); // Creates the Menus and adds them to the Menu Bar
}

void MainWindow::getFilePath()// Gets the Folder Path of the Volume Package location, and initiates a Volume Package.
{
    QFileDialog *dialogBox= new QFileDialog();
    QString filename = dialogBox->getExistingDirectory();
    std::string file_Name = filename.toStdString();

    if(filename!=NULL)// If the user selected a Folder Path
    {
        if ((file_Name.substr(file_Name.length()-7, file_Name.length())).compare(".volpkg") == 0)// Checks the Folder Path for .volpkg extension
        {
            try {
                    _globals->setPath(filename);// Sets Folder Path in Globals
                    _globals->createVolumePackage();// Creates a Volume Package Object
                    _globals->getMySegmentations();// Gets Segmentations and assigns them to "segmentations" in Globals
                    _segmentations_Viewer->setSegmentations();// Sets the Segmentations for the Segmentation Viewer and assigns the
                    _segmentations_Viewer->setVol_Package_Name(filename);// Sets the name of the Volume Package to Display on the GUI

                }catch(...)
                        {
                            QMessageBox::warning(this, tr("Error Message"), "Error Opening File.");
                        };


        } else {
                    QMessageBox::warning(this, tr("Error Message"), "Invalid File.");
               }
    }
}

void MainWindow::saveTexture()// Overrides the Current Texture Image in the Segmentation's Folder with the newly Generated Texture Image.
{
    if(_globals->isVPKG_Intantiated() && _globals->getVolPkg()->getSegmentations().size()!=0)//If A Volume Package is Loaded and there are Segmentations (continue)
    {
        if(_globals->getTexture().hasImages())// Checks to see if there are images
        {
            try{
                _globals->getVolPkg()->saveTextureData(_globals->getTexture());// Saves Texture Image
                QMessageBox::information(this, tr("Error Message"), "Saved Successfully.");

                }catch(...)
                    {
                        QMessageBox::warning(_globals->getWindow(), "Error", "Failed to Save Texture Image Properly!");
                    }

        }else QMessageBox::information(this, tr("Error Message"), "Please Generate a New Texture Image.");

    }else QMessageBox::warning(this, tr("Error Message"), "There is no Texture Image to Save!");
}

void MainWindow::exportTexture()// Exports the Image as .tif, .tiff, .png, .jpg, and .jpeg
{
    if(_globals->isVPKG_Intantiated() && _globals->getVolPkg()->getSegmentations().size()!=0) //If A Volume Package is Loaded and there are Segmentations (continue)
    {
        if(_globals->getTexture().hasImages())// Checks to see if there are images
        {
            bool path = false;// Used to determine whether to output error message

            try
            {
                QString imagePath = "";
                imagePath = QFileDialog::getSaveFileName(this, tr("Export Texture Image"), "",tr("Images (*.png *jpg *jpeg *tif *tiff)"));

                if(imagePath!="")
                {
                    path = true;
                    cv::Mat texture = _globals->getTexture().getImage(0).clone();
                    texture.convertTo(texture, CV_8U, 255.0 / 65535.0);
                    cv::imwrite( imagePath.toStdString(), texture );
                    QMessageBox::information(this,"Successful","Successfully Exported Image");
                }

            }catch(...)
                {
                    if(path)
                    {
                        QMessageBox::warning(this, "Error", "Error Exporting Texture Image Properly! Acceptable: ( .jpg, .jpeg, .png, .tif, .tiff)");
                    }
                }

        }else
            {
                bool path = false;
                cv::Mat texture = _globals->getVolPkg()->getTextureData();

                try
                {
                    if (texture.data == nullptr)
                        {
                        QMessageBox::warning(this, "Error", "There is no Texture Image to Export!");

                    } else
                          {
                              QString imagePath = "";
                              imagePath = QFileDialog::getSaveFileName(this, tr("Export Texture Image"), "", tr("Images (*.png *jpg *jpeg *tif *tiff)"));

                              if(imagePath!="")
                              {
                                  path = true;
                                  texture.convertTo(texture, CV_8U, 255.0 / 65535.0);
                                  cv::imwrite( imagePath.toStdString(), texture);
                                  QMessageBox::information(this,"Successful","Successfully Exported Image");
                              }
                          }

                }catch(...)
                    {
                        if(path)
                        {
                            QMessageBox::warning(this, "Error", "Error Exporting Texture Image Properly! Acceptable: ( .jpg, .jpeg, .png, .tif, .tiff)");
                        }
                    }
            }

    }else QMessageBox::warning(this, "Error", "There is no Texture Image to Export!");
}

void MainWindow::create_Actions()
{
    actionGetFilePath = new QAction( "Open Volume Package...", this );
    connect( actionGetFilePath, SIGNAL( triggered() ), this, SLOT(getFilePath()));

    actionSave = new QAction( "Save Texture", this );
    connect( actionSave, SIGNAL( triggered() ), this, SLOT(saveTexture()));

    actionExport = new QAction( "Export Texture", this);
    connect( actionExport, SIGNAL(triggered()), this, SLOT(exportTexture()));
}

void MainWindow::create_Menus()
{
    fileMenu = new QMenu( tr( "&File" ), this );
    _globals->setFileMenu(fileMenu);

    fileMenu->addAction(actionGetFilePath);
    fileMenu->addAction(actionSave);
    fileMenu->addAction(actionExport);

    menuBar()->addMenu(fileMenu);
}
