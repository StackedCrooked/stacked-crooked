/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef EvMainWindow_H
#define EvMainWindow_H

#include <QMainWindow>
#include "EvTabbedDocumentBrowser.h"


class QAction;
class QMenu;
class QTextEdit;
class QTabWidget;

class EvMainWindow : public QMainWindow
{
    Q_OBJECT

public:
	static EvMainWindow&
	GetInstance();

	static void
	Finalize();

public slots:
	void
	receiveCurrentChanged( int inIndex );

	void
	NewFile();


protected:
    void closeEvent(QCloseEvent *event);

private slots:
	
    void receiveOpen();
    bool receiveSave();
    bool receiveSaveAs();
    bool receiveRevertFile();
    void receiveAbout();

private:
	
	static EvMainWindow* sInstance;

	EvMainWindow();

	EvTabbedDocumentBrowser* mTabbedDocumentBrowser;
	QTextEdit* mTextEdit;

	void LoadFile(const QString &fileName);
    bool SaveFile(const QString &fileName);
    bool RevertFile(const QString &fileName);

    void SetCurrentFile(const QString &fileName);
	
    void createActions();
    void createMenus();
    void createToolBars();
    void readSettings();
    void writeSettings();
    
	


    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *newTabAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *revertFileAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
