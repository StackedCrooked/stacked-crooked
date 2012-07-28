#include "ListView.h"


enum {
    cIconSize = 200,
    cGridSize = 400
};


ListView::ListView(QWidget * )
{
    setDragEnabled(false);
    setViewMode(QListView::ListMode);
    setIconSize(QSize(cIconSize, cIconSize));
    setGridSize(QSize(cGridSize, cIconSize));
    setSpacing(10);
    setAcceptDrops(false);
}
