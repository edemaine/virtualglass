
#ifndef PULLPLANGEOMETRYTHREAD_H
#define PULLPLANGEOMETRYTHREAD_H

#include <QThread>

class PullPlanEditorWidget;

class PullPlanGeometryThread : public QThread
{
	Q_OBJECT

	public:
		PullPlanGeometryThread(PullPlanEditorWidget* ppew);
		void run();
		PullPlanEditorWidget* ppew;

	signals:
		void finishedMesh(bool completed, unsigned int quality);
};

#endif




