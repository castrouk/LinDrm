/***************************************************************************
 *   Copyright (C) 2000-2008 by Johan Maes                                 *
 *   on4qz@telenet.be                                                      *
 *   http://users.telenet.be/on4qz                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "editor.h"

// #include <QtGui>
// #include <QtWidgets>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets>
#else
	#include <QtGui>
#endif



// #include "qsstvglobal.h"
#include "utils/supportfunctions.h"
#include "editorview.h"
#include "configparams.h"
// #include "dispatcher.h"

/*! 
	constructor
*/
editor::editor(QWidget *parent,Qt::WindowFlags flags): QMainWindow(parent,flags)
{
	ev=new editorView(this);
	setCentralWidget (ev);
	initActions();
	initMenubar();
	statusBar()->showMessage("Select a tool");
	setMinimumSize(640,480);
	resize(640,480);
//  addToLog (QString(" editor create: %1")
//    .arg(QString::number((ulong)this,16)),LOGEDIT);
  setWindowTitle("QSSTV Editor");
}

/*! 
	destructor (saves settings on deletion)
*/

editor::~editor()
{
//  addToLog (QString(" editor delete: %1")
//    .arg(QString::number((ulong)this,16)),LOGEDIT);
	writeSettings();
}

/*! 
	reads the settings (saved images for tx,rx,templates)
*/

void editor::readSettings()
{
	QSettings qSettings;
	qSettings.beginGroup ("Editor" );
	int windowWidth = qSettings.value( "windowWidth", 640 ).toInt();
	int windowHeight = qSettings.value( "windowHeight", 480 ).toInt();
	int windowX = qSettings.value( "windowX", -1 ).toInt();
	int windowY = qSettings.value( "windowY", -1 ).toInt();
	if ( windowX != -1 || windowY != -1 ) 	move ( windowX, windowY );
	resize ( windowWidth, windowHeight );
	qSettings.endGroup();
}

/*! 
	writes the settings (saved images for tx,rx,templates)
*/
void editor::writeSettings()
{
	QSettings qSettings;
	qSettings.beginGroup ("Editor" );
	qSettings.setValue ( "windowWidth", width() );
	qSettings.setValue ( "windowHeight", height() );
	qSettings.setValue ( "windowX", x() );
	qSettings.setValue ( "windowY", y() );
	qSettings.endGroup();
}


void editor::initActions()
{
	fileNew = new QAction(QIcon(":/icons/filenew.png"),tr("&New"),this);
	fileNew->setShortcut(tr("Ctrl+N"));
	fileNew->setStatusTip(tr("Create a new image"));
  connect(fileNew, SIGNAL(triggered()), this, SLOT(slotFileNew()));
	  	
	fileOpen = new QAction(QIcon(":/icons/fileopen.png"),tr("&Open"),this);
	fileOpen->setShortcut(tr("Ctrl+O"));
	fileOpen->setStatusTip(tr("Open an image file"));
  connect(fileOpen, SIGNAL(triggered()), this, SLOT(slotFileOpen()));
	
	fileSave = new QAction(QIcon(":/icons/filesave.png"),tr("&Save file .."),this);
  fileSave->setStatusTip(tr("Save the file under the same name and format"));
  connect(fileSave, SIGNAL(triggered()), this, SLOT(slotFileSave()));
	
	fileSaveImage = new QAction(tr("Save &Image file .."),this);
  fileSaveImage->setStatusTip(tr("Save the file in PNG format"));
  connect(fileSaveImage, SIGNAL(triggered()), this, SLOT(slotFileSaveImage()));
	
	fileSaveTemplate = new QAction(("Save &Template .."),this);
  fileSaveTemplate->setStatusTip(tr("Save template file "));
  connect(fileSaveTemplate, SIGNAL(triggered()), this, SLOT(slotFileSaveTemplate()));
	
	fileQuit = new QAction(tr("Quit"),this);
	fileQuit->setShortcut(tr("Ctrl+Q"));
  fileQuit->setStatusTip(tr("Quits the editor"));
  connect(fileQuit, SIGNAL(triggered()), this, SLOT(slotFileQuit()));
		
	clearAll= new QAction(QIcon(":/icons/eraser.png"),tr("Clear &All"),this);
  clearAll->setShortcut(tr("Ctrl+A"));
  clearAll->setStatusTip(tr("Delete all objects and fill the background with the background color"));
  connect(clearAll, SIGNAL(triggered()), ev, SLOT(slotClearAll()));

	copy= new QAction(tr("Copy"),this);
  copy->setShortcut(tr("Ctrl+C"));
  connect(copy, SIGNAL(triggered()), ev->getScene(), SLOT(slotCopy()));

	paste= new QAction(tr("Paste"),this);
	paste->setShortcut(tr("Ctrl+V"));
    connect(paste, SIGNAL(triggered()), ev->getScene(), SLOT(slotPaste()));

	deleteAction=new QAction(tr("&Delete"),this);
	deleteAction->setShortcut(tr("Del"));
    connect(deleteAction, SIGNAL(triggered()), ev->getScene(), SLOT(slotDeleteItem()));

	dump= new QAction(tr("dump"),this);
  connect(dump, SIGNAL(triggered()), ev, SLOT(slotDump()));
}



void editor::initMenubar()
{
	fileMenu=menuBar()->addMenu(tr("&File"));
	editMenu=menuBar()->addMenu(tr("&Edit"));
	fileMenu->addAction(fileNew);
	fileMenu->addAction(fileOpen);
	fileMenu->addAction(fileSave);
	fileMenu->addAction(fileSaveImage);
	fileMenu->addAction(fileSaveTemplate);
	fileMenu->addAction(fileQuit);
	editMenu->addAction(deleteAction);
	editMenu->addAction(copy);
	editMenu->addAction(paste);
	editMenu->addAction(clearAll);
	editMenu->addAction(dump);
}


void editor::slotFileNew()
{
	if(ev->isModified())
		{
			switch( QMessageBox::information( this, "Editor",
        "The document has not been saved as a template\n",
        "&Continue Anyway","Cancel",NULL, 
        -1,      // Enter == button 0
        1 ) )
				 { // Escape == button 2
 	   				case 0: // Continu clicked
  	      	break;
						case 1: // Cancel clicked
  	      		return;
    				break;
	    	}
		}
	ev->slotClearAll();
	localFile.close();
	localFile.setFileName("");
}

void editor::slotFileOpen()
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
/*	QFileDialog *fd = new QFileDialog(this,0,TRUE);
//	fd->show();*/
//	dirDialog d(this,0,FALSE);
//    d.show();
//	QString s=d.openFileName(txImagesPath,"*.png *.gif *.jpg *.templ");
//	if (s==QString::null) return ;
//	if (s.isEmpty()) return ;
//	localFile.setFileName(s);
//	ev->open(localFile);
////  addToLog("localfile after open = " + localFile.fileName(),LOGEDIT);
}

/*!
		\fn editor::slotFileSave()
		\brief save file under same name and same type
*/

void editor::slotFileSave()
{
	if(localFile.fileName().isEmpty())
		{
			slotFileSaveTemplate();
			return;
		}
	if(ev->getScene()->getImageType()==editorScene::FLATIMAGE)
		{
//      addToLog("localfile to save = " + localFile.fileName(),LOGEDIT);
			ev->save(localFile,FALSE);
		}
	else
		{
			ev->save(localFile,TRUE);
		}
}

void editor::slotFileSaveImage()
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
//    dirDialog d((QWidget *)this,"Editor",TRUE);
//    d.show();
//	QString s(localFile.fileName());
//	if(s.isEmpty())
//		{
//			s=txImagesPath;
//		}
//	s=d.saveFileName(s,"*.png","png");
//	if (s==QString::null) return ;
//	if (s.isEmpty()) return ;
//	localFile.setFileName(s);
//	ev->save(localFile,FALSE);
}

void editor::slotFileSaveTemplate()
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
//    dirDialog d((QWidget *)this,"Browse",TRUE);
//    d.show();
//	QString s(localFile.fileName());
//	if(s.isEmpty())
//		{
//			s=templatesPath;
//		}
//	s=d.saveFileName(s,"*.templ","templ");
//	if (s==QString::null) return ;
//	if (s.isEmpty()) return ;
//	localFile.setFileName(s);
//	ev->save(localFile,TRUE);
}


void editor::slotFileQuit()
{
	close();	
}



void editor::closeEvent(QCloseEvent *e)
{

//	if(ev->isModified())
//		{
//     QMessageBox msgBox;
//     msgBox.setText("The document has been modified.");
//     msgBox.setInformativeText("Do you want to save your changes?");
//     msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//     msgBox.setDefaultButton(QMessageBox::Save);
//     int ret = msgBox.exec();
//     switch (ret)
//      {
//        case QMessageBox::Save:
//            slotFileSave();
//            break;
//        case QMessageBox::Discard:
//            // Don't Save was clicked
//            break;
//        case QMessageBox::Cancel:
//            return;
//            break;
//        default:
//            // should never be reached
//            break;
//      }
//   }






//			switch( QMessageBox::information( this, "Editor",
//        "The document has not been saved as a template\n",
//        "&Continue Anyway","Cancel",NULL,-1,      // Enter == button 0
//        1 ) )
//				 { // Escape == button 2
// 	   				case 0: // Continu clicked
//  	      	break;
//						case 1: // Cancel clicked
//							editorFinishedEvent *ce = new editorFinishedEvent(TRUE,localFile.fileName());
//							QApplication::postEvent(dispatcherPtr, ce );
//  	      		return;
//    				break;
//	    	}
//		}
//  else
//    {
//      editorFinishedEvent *ce = new editorFinishedEvent(TRUE,localFile.fileName());
//      QApplication::postEvent(dispatcherPtr, ce );  // Qt will delete it when done	emit imageAvailable(ev->getImage());
////    }
//	writeSettings();
//	e->accept();
}


//bool editor::render(QImage **im,QString fn)
//{
//	if(!openFile(fn)) return FALSE;
//  *im=ev->getScene()->renderImage();
//  addToLog(QString("editor: render size: %1 x %2").arg((*im)->size().width()).arg((*im)->size().height()),LOGEDIT);
//  return TRUE;
//}

bool editor::setImage(QImage *im)
{
  ev->setImage(im);
  return TRUE;
}

bool editor::openFile(QString fn)
{
	QFile f(fn);
	localFile.setFileName(fn);
	return ev->open(f);
}

