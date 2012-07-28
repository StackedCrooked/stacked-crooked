#include "ListView.h"


enum {
    cIconSize = 20,
    cGridSize = 40
};


ListView::ListView(QWidget * )
{
    setDragEnabled(false);
    setViewMode(QListView::IconMode);
    setIconSize(QSize(cIconSize, cIconSize));
    setGridSize(QSize(cGridSize, cIconSize));
    setSpacing(10);
    setAcceptDrops(false);
}
