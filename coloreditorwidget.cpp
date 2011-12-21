
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(QWidget* parent) : QWidget(parent)
{
	Color* color = new Color;
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;
        this->colorBar = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
        this->viewWidget = new ColorEditorViewWidget(colorBar, this);
	this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	mesher.generateMesh(colorBar, &geometry);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void ColorEditorWidget :: seedColors()
{
	viewWidget->seedBrandColors();	
}

void ColorEditorWidget :: updateEverything()
{
        geometry.clear();
        mesher.generateMesh(colorBar, &geometry);
        niceViewWidget->repaint();
}

#ifdef UNDEF
void PullPlanEditorWidget :: unhighlightLibraryWidget(PullTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
}

void PullPlanEditorWidget :: highlightLibraryWidget(PullTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
        ((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
        w->graphicsEffect()->setEnabled(true);
}
#endif

void ColorEditorWidget :: setupLayout()
{
        QHBoxLayout* pageLayout = new QHBoxLayout(this);
        this->setLayout(pageLayout);
        QVBoxLayout* editorLayout = new QVBoxLayout(this);
        pageLayout->addLayout(editorLayout);
        editorLayout->addWidget(viewWidget, 0);
        pageLayout->addWidget(niceViewWidget, 10);

        // Little description for the editor
        QLabel* descriptionLabel = new QLabel("Color editor", this);
        descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        editorLayout->addWidget(descriptionLabel, 0);
}

void ColorEditorWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(viewWidget, SIGNAL(someDataChanged()), this, SLOT(viewWidgetDataChanged()));
}

void ColorEditorWidget :: viewWidgetDataChanged()
{
	emit someDataChanged();
}

void ColorEditorWidget :: updateLibraryWidgetPixmaps(ColorBarLibraryWidget* w)
{
	QPixmap editorPixmap(100, 100);
	editorPixmap.fill(QColor(255*colorBar->color->r,
		255*colorBar->color->g,
		255*colorBar->color->b,
		MAX(255*colorBar->color->a, 255*0.05)));
	w->updatePixmaps(
		QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100),
		editorPixmap);
}

void ColorEditorWidget :: setColor(float r, float g, float b, float a)
{
	colorBar->color->r = r;	
	colorBar->color->g = g;	
	colorBar->color->b = b;	
	colorBar->color->a = a;	
	emit someDataChanged();	
}

void ColorEditorWidget :: setColorBar(PullPlan* b)
{
	colorBar = b;
	viewWidget->setPullPlan(b);
	emit someDataChanged();	
}

Color* ColorEditorWidget :: getColor()
{
	return colorBar->color;
}

PullPlan* ColorEditorWidget :: getColorBar()
{
	return colorBar;
}




