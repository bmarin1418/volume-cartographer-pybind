// CWindow.h
// Chao Du 2014 Dec
#ifndef _CWINDOW_H_
#define _CWINDOW_H_

#include "VCNewGuiHeader.h"

#ifdef _QT5_
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#else // _QT4_
#include <QtGui>
#endif

#include "mathUtils.h"
#include "CBSpline.h"
#include "CXCurve.h"

#include "ui_VCMain.h"

#include <opencv2/opencv.hpp>
#include <pcl/common/common.h>
#include <pcl/point_types.h>

class VolumePkg;

namespace ChaoVis {

class CVolumeViewerWithCurve;

class CWindow : public QMainWindow {

    Q_OBJECT

public:
    // REVISIT - states not used, remove me
    enum EWindowState { WindowStateSegment,      // under segmentation state
                        WindowStateRefine };     // under mesh refinemen state

    typedef struct SSegParams_tag {
        int fGravityScale;
        int fThreshold;
        int fEndOffset;
    } SSegParams;

public:
    CWindow( void );
    ~CWindow( void );

protected:
    void mousePressEvent( QMouseEvent *nEvent );
	void keyPressEvent( QKeyEvent *event );

private:
    void CreateWidgets( void );
    void CreateMenus( void );
    void CreateActions( void );

    bool InitializeVolumePkg( const std::string &nVpkgPath );

    void UpdateView( void );

    void DoSegmentation( void );
    bool SetUpSegParams( void );

private slots:
    void Open( void );
    void Close( void );
    void About( void );

    void OnNewPathClicked( void );
    void OnPathItemClicked( QListWidgetItem* nItem );

    void TogglePenTool( void );
    void ToggleEditTool( void );

    void OnEdtGravityValChange( QString nText );
    void OnEdtSampleDistValChange( QString nText );
    void OnEdtStartingSliceValChange( QString nText );
    void OnEdtEndingSliceValChange( QString nText );

    void OnBtnStartSegClicked( void );

private:
	// data model
    EWindowState fWindowState;

    VolumePkg   *fVpkg;
    QString     fVpkgPath;
    std::string fVpkgName;

    std::string fSegmentationId;

    int         fPathOnSliceIndex; // effectively equivalent to the starting slice index

    CBSpline    fCurve; // the curve at current slice
    std::vector< CXCurve > fIntersections; // curves of all the slices
    std::vector< CXCurve > fCurvesLower; // neighboring curves, { -1, -2, ... }
    std::vector< CXCurve > fCurvesUpper; // neighboring curves, { +1, +2, ... }

    // REVSIIT - the state of the application should be unique and consistent
    //           that means these mode should be mutually exclusive; consider using one variable, see EWindowState
    bool        fIsInDrawingMode;
    bool        fIsInEditingMode;

    SSegParams  fSegParams;

    // REVISIT - how is the point cloud manipulated?
    //           we have this global point cloud, initialized by the starting path, then expanded by segmentation
    //           once we choose editing at a certain slice, we modify the vertices (however, keep the # of vertices unchanged)
    //           and then let the user start segmentation again from this slice. We allow the user to select how many sliced
    //           he want to iterate through, however the newly generated point cloud will overwrite these and the rest data
    //           in the previous point cloud.
    //           Terminologies:
    //           For our current particle simulation method, we can represent the point cloud with width (# of particles in one slice) and
    //           height (# of iterations). So we can start segmentation from any slice given these two parameters. We call
    //           the point cloud before current slice (3 ~ fPathOnSliceIndex-1) the "upper part", and the newly generated
    //           point cloud from segmentation routine the "lower part". The upper part is from the original point cloud (if there was one)
    //           and the final point cloud is the concatenation of the two parts.
    //           We call the point cloud loaded from disk the "immutable cloud". We only save to disk after the concatenation.
    //           Previously we use a txt file to store vertices of the path where the particle simulation starts. Now they are
    //           stored in "path cloud".
    // REVISIT - maybe redundant
    pcl::PointCloud< pcl::PointXYZRGB > fICloud;    // immutable cloud, load from disk
    pcl::PointCloud< pcl::PointXYZRGB > fPathCloud; // path cloud, particle simulation seed
    pcl::PointCloud< pcl::PointXYZRGB > fUpperPart; // upper part of immutable cloud, not changed
    pcl::PointCloud< pcl::PointXYZRGB > fLowerPart; // newly generated point cloud, from segmentation
    pcl::PointCloud< pcl::PointXYZRGB > fMasterCloud;  // concatenation of upper part and lower part

    // window components
    QMenu		*fFileMenu;
	QMenu		*fEditMenu;
    QMenu       *fHelpMenu;

    QAction		*fOpenVolAct;
    QAction		*fExitAct;
    QAction     *fAboutAct;

    CVolumeViewerWithCurve
                *fVolumeViewerWidget;
    QListWidget *fPathListWidget;
    QPushButton *fPenTool; // REVISIT - change me to QToolButton
    QPushButton *fEditTool;

    QLineEdit   *fEdtGravity;
    QLineEdit   *fEdtSampleDist;
    QLineEdit   *fEdtStartIndex;
    QLineEdit   *fEdtEndIndex;

    Ui::VCMainWindow    ui;

}; // class CWindow

} // namespace ChaoVis

#endif // _CWINDOW_H_
