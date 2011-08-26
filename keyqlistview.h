

#ifndef KEYQLISTVIEW_H
#define KEYQLISTVIEW_H

#include <QtGui>

class KeyQListView : public QListView
{
public:
                KeyQListView(QWidget * parent) : QListView(parent) {}
                KeyQListView() : QListView() {}
protected:
                void keyPressEvent(QKeyEvent *event)
                {
                                QModelIndex oldIdx = currentIndex();
                                QListView::keyPressEvent(event);
                                QModelIndex newIdx = currentIndex();
                                if(oldIdx.row() != newIdx.row())
                                {
                                                emit clicked(newIdx);
                                }
                }
};

#endif

