#include "pullplantabwidget.h"

PullPlanTabWidget::PullPlanTabWidget(QWidget* parent) : QWidget(parent)
{
    pullPlanTabs = new QTabWidget(parent);

    pullPlanEditor = new PullPlanEditorWidget();
    topPlan = pullPlanEditor->getPlan();
    pullPlanCustomize = new PullPlanCustomizeWidget(topPlan);
    this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
    niceViewWidget->setGeometry(&geometry);

    pullPlanTabs->addTab(pullPlanEditor, QString("Standard"));
    pullPlanTabs->addTab(pullPlanCustomize, QString("Customize"));
    pullPlanTabs->setCurrentWidget(pullPlanEditor);

    QHBoxLayout* pageLayout = new QHBoxLayout(this);

    QVBoxLayout* editorLayout = new QVBoxLayout(this);
    pageLayout->addLayout(editorLayout, 1);
    editorLayout->addWidget(pullPlanTabs, 1);

    QLabel* descriptionLabel = new QLabel("Cane editor - drag color or other canes in.", this);
    descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    editorLayout->addWidget(descriptionLabel, 0);

    QVBoxLayout* niceViewLayout = new QVBoxLayout(this);
    pageLayout->addLayout(niceViewLayout, 1);
    niceViewLayout->addWidget(niceViewWidget, 10);

    // Little description for the editor
    QLabel* niceViewDescriptionLabel = new QLabel("3D view of cane.", this);
    niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    niceViewLayout->addWidget(niceViewDescriptionLabel, 0);

    this->setLayout(pageLayout);

    connect(pullPlanEditor, SIGNAL(geometryChanged(Geometry)), this, SLOT(updateGeometry(Geometry)));
    connect(pullPlanEditor, SIGNAL(pullPlanChanged(PullPlan*)), this, SLOT(updateCustomizePlan(PullPlan*)));
    connect(pullPlanCustomize, SIGNAL(newPullPlan(PullPlan*)), this, SLOT(updateEditorPlan(PullPlan*)));
}

void PullPlanTabWidget :: updateEditorPlan(PullPlan* p)
{
    topPlan = p;
    pullPlanEditor->setPlan(p);
}

void PullPlanTabWidget :: updateCustomizePlan(PullPlan* p)
{
    topPlan = p;
    pullPlanCustomize->setPullPlan(p);
}

void PullPlanTabWidget :: updateGeometry(Geometry g)
{
    geometry = g;
    niceViewWidget->repaint();
}

void PullPlanTabWidget :: updateEverything()
{
    pullPlanEditor->updateEverything();
//    pullPlanCustomize->updateEverything();
}

/*void PullPlanTabWidget :: mousePressEvent(QMouseEvent* event)
{
    if(pullPlanTabs->currentWidget() == pullPlanEditor)
    {
        std::cout << "editor" << std::endl;
        pullPlanEditor->mousePressEvent(event);
    }
    else
    {
        std::cout << "customize" << std::endl;
        pullPlanCustomize->mousePressEvent(event);
    }
}*/

void PullPlanTabWidget :: seedTemplates()
{
    pullPlanEditor->seedTemplates();
}

void PullPlanTabWidget :: updateLibraryWidgetPixmaps(AsyncPullPlanLibraryWidget* w)
{
    pullPlanEditor->updateLibraryWidgetPixmaps(w);
}

void PullPlanTabWidget :: setPlan(PullPlan* p)
{
    topPlan = p;
    pullPlanEditor->setPlan(p);
    emit someDataChanged();
}

PullPlan* PullPlanTabWidget :: getPlan()
{
    return topPlan;
}
