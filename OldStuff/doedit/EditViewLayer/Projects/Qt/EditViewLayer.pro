include(BuildSystem/BuildSystem.pro)

VPATH += $$SOURCES_ROOT/EditViewLayer/Sources/CPP \
         $$SOURCES_ROOT/EditViewLayer/Sources/H

INCLUDEPATH += $$VPATH

HEADERS	+=	EvMainWindow.h \
			EvDocumentMgr.h \
			EvDocumentBrowser.h  \
			EvTabBar.h  \
			EvTabbedDocumentBrowser.h  \
			

SOURCES	+=	EvMainWindow.cpp \
			EvDocumentMgr.cpp \
			EvTabbedDocumentBrowser.cpp  \
			EvTabBar.cpp  \
			
