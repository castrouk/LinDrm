
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtSql>
#include <QSqlIndex>
#include <QDebug>
#include <QTime>

#include "tableeditor.h"

TableEditor::TableEditor(const QString &tableName, QWidget *parent)
	: QWidget(parent)
{
	model = new QSqlTableModel(this);
	model->setTable(tableName);

	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();

	model->setHeaderData(0, Qt::Horizontal, tr(" Call "));
	model->setHeaderData(1, Qt::Horizontal, tr("   Name   "));
	model->setHeaderData(2, Qt::Horizontal, tr("        Comment        "));
	model->setHeaderData(3, Qt::Horizontal, tr("    Time    "));

	QTableView *view = new QTableView;
	view->setModel(model);
	view->resizeColumnsToContents();

	newEntryButton = new QPushButton(tr("Insert"));
//	MoveButton = new QPushButton(tr("Move"));
	submitButton = new QPushButton(tr("Submit"));
	submitButton->setDefault(true);
	revertButton = new QPushButton(tr("&Revert"));
	DeleteButton = new QPushButton(tr("Delete"));
	ClearButton = new QPushButton(tr("Delete All"));
	quitButton = new QPushButton(tr("Quit"));

	buttonBox = new QDialogButtonBox(Qt::Vertical);
	buttonBox->addButton(newEntryButton, QDialogButtonBox::ActionRole);
//	buttonBox->addButton(MoveButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(submitButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(revertButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(DeleteButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(ClearButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	connect(newEntryButton, SIGNAL(clicked()), this, SLOT(Insert()));
	connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
	connect(revertButton, SIGNAL(clicked()), model, SLOT(revertAll()));
	connect(DeleteButton, SIGNAL(clicked()), this, SLOT(Delete()));
	connect(ClearButton, SIGNAL(clicked()), this, SLOT(Clear()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(view);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Session Log"));
//	qDebug() << __FILE__ << __LINE__;
}

//TableEditor::~TableEditor()
//{
//	qDebug() << __FILE__ << __LINE__;
//}

//void TableEditor::close()
//{
//	qDebug() << __FILE__ << __LINE__;
//}

//void TableEditor::insert(QString call)
//{
////	QString call("G0KRK");
//	QString cmd;
//	QString time = QTime::currentTime().toString();

//	cmd.sprintf("insert into session values('%s', '-', '-', '%s')",
//					call.toLatin1().data(),
//					time.toLatin1().data());
//	QSqlQuery query;
//	query.exec(cmd.toLatin1().data());
//	model->submitAll();
////	move();

//}

void TableEditor::saveLogs()
{
	qDebug() << __FILE__ << __LINE__ << "TableEditor::saveLogs()";
	this->close();
}

void TableEditor::Insert()
{
	qDebug() << __FILE__ << __LINE__ ;
	insert("G7ORT");
}

void TableEditor::insert(QString Call)
{
//	qDebug() << __FILE__ << __LINE__ << Call;
	QString call;
	QString name;
	QString comment;

	// id, firstname, comment
	QSqlQuery query("SELECT * FROM session");
	bool found = false;
//	int row = 0;
	while(query.next())
	{
		call = query.value(0).toString();
		name = query.value(1).toString();
		comment = query.value(2).toString();
		if(call.contains(Call))
		{
			found = true;
			qDebug() << __FILE__ << __LINE__ << query.at() << call << found;
			break;
		}
//		row++;
	}
	if(found)
	{
		model->removeRow(query.at());
		model->submitAll();
	}
	else
	{
		name = "-";
		comment = "-";
	}


	QString time = QTime::currentTime().toString();
	QString cmd;
	cmd.sprintf("insert into session values('%s', '%s', '%s', '%s')",
					Call.toLatin1().data(),
					name.toLatin1().data(),
					comment.toLatin1().data(),
					time.toLatin1().data());
	query.exec(cmd.toLatin1().data());
	model->submitAll();
}

void TableEditor::submit()
{
	model->database().transaction();
	if (model->submitAll())
	{
		model->database().commit();
	}
	else
	{
		model->database().rollback();
		QMessageBox::warning(this, tr("Cached Table"),
									tr("The database reported an error: %1")
									.arg(model->lastError().text()));
	}
}

void TableEditor::Delete()
{
	qDebug() << __FILE__ << __LINE__ << model->select();
//	model->
}

void TableEditor::Clear()
{
	qDebug() << __FILE__ << __LINE__;

}

