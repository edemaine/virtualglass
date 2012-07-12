#ifndef PULLPLANTABWIDGET_H
#define PULLPLANTABWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "pullplaneditorwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "asyncpullplanlibrarywidget.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "pullplancustomizewidget.h"

class PullPlanTabWidget : public QWidget
{
    Q_OBJECT

public:
    PullPlanTabWidget(QWidget* parent=0);
    PullPlan* getPlan();
    void setPlan(PullPlan* p);
    void updateLibraryWidgetPixmaps(AsyncPullPlanLibraryWidget* w);
    void seedTemplates();
    //void mousePressEvent(QMouseEvent*);

signals:
    void someDataChanged();
    void newPullPlan(PullPlan* p);
    void geometryChanged(Geometry g);
//    void pullPlanChanged(PullPlan* p);

public slots:
    void updateEverything();
    void updateGeometry(Geometry g);
    void updateEditorPlan(PullPlan* p);
    void updateCustomizePlan(PullPlan* p);

private:
    Geometry geometry;
    QTabWidget *pullPlanTabs;
    PullPlanCustomizeWidget *pullPlanCustomize;
    PullPlanEditorWidget *pullPlanEditor;
    PullPlan* topPlan;
    NiceViewWidget* niceViewWidget;
};

#endif // PULLPLANTABWIDGET_H
