#include <QtWidgets/QFileDialog>
#include "jvx_connect_results.h"
#include <iostream>
#include <fstream>

#define JVX_NUM_COLORS_SHOW 4
QColor colorlist[JVX_NUM_COLORS_SHOW] =
{
	/* QColor(127, 127, 127)*/
	Qt::green,
	Qt::yellow,
	Qt::magenta, 
	Qt::lightGray
};

// ===========================================================================

jvx_connect_results::jvx_connect_results(QWidget* parent) : QDialog(parent)
{
	fName = "./connection_result.txt";
	filterEntrAct = false;
	tokenFilter = "/*";

}

void 
jvx_connect_results::create(IjvxCallProt* callProt)
{
	setupUi(this);

	callProtStore = callProt;
	updateWindow(true);
}

jvx_connect_results::~jvx_connect_results()
{

}

void 
jvx_connect_results::updateWindow(jvxBool updateTree)
{
	if (updateTree)
	{
		jvxSize colId = 0;
		std::list< oneColorEntry> colors;
		

		treeWidget_result->clear();

		QTreeWidgetItem* newItem = new QTreeWidgetItem(treeWidget_result);
		treeWidget_result->addTopLevelItem(newItem);

		addLayerTree(newItem, callProtStore, "Function Entry", colId, colors);

		newItem->setExpanded(true);
	}
	lineEdit_fname->setText(fName.c_str());
}

jvxSize 	
jvx_connect_results::find_color_id(jvxSize& colId, const jvxComponentIdentification& cpTp,
	const std::string& description, const std::string& ctxt, const std::string& connector, std::list<oneColorEntry>&colors)
{
	for (auto elm = colors.begin(); elm != colors.end(); elm++)
	{
		if (
			(elm->context == ctxt) && 
			(elm->cpTp == cpTp) &&
			(elm->connector == connector) &&
			(elm->description == description))
		{
			return elm->colId;
		}
	}
	oneColorEntry newEntry;
	colId = (colId + 1) % JVX_NUM_COLORS_SHOW;
	newEntry.colId = colId;
	newEntry.context = ctxt;
	newEntry.cpTp = cpTp;
	newEntry.connector = connector;
	newEntry.description = description;
	colors.push_back(newEntry);
	return colId;

}

void
jvx_connect_results::addLayerTree(QTreeWidgetItem* addToMe, IjvxCallProt* stepCall, const std::string& entry, 
	jvxSize& colId, std::list< oneColorEntry>& colors)
{
	jvxComponentIdentification cpTp = JVX_COMPONENT_UNKNOWN;
	jvxApiString descr;
	jvxApiString conn;
	jvxApiString origin;
	jvxApiString ctxt;
	jvxApiString comm;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString mess;
	jvxApiString origin_err;
	jvxApiString loc;
	jvxSize num = 0;
	IjvxCallProt* theNext = NULL;
	jvxSize i;
	jvxSize locColId;
	QTreeWidgetItem* newItemSubCall = addToMe;
	QColor locCol = Qt::black;
	jvxBool showMe = true;
	if (filterEntrAct)
	{
		showMe = false;
		stepCall->getNumberEntries(&num);
		if (num > 0)
		{
			for (i = 0; i < num; i++)
			{
				stepCall->getEntry(&descr, NULL, NULL, NULL, i);
				if (jvx_compareStringsWildcard(tokenFilter, descr.std_str()))
				{
					showMe = true;
					break;
				}
			}
		}
	}
	stepCall->getIdentification(&cpTp, &descr, &conn, &origin, &ctxt, &comm);
	stepCall->getSuccess(&res, &mess, &origin_err);

	locColId = find_color_id(colId, cpTp, descr.std_str(), ctxt.std_str(), conn.std_str(), colors);
	locCol = colorlist[locColId];

	if (showMe)
	{
		addToMe->setText(0, entry.c_str());
		if (cpTp.tp != JVX_COMPONENT_UNKNOWN)
		{
			addToMe->setText(1, (jvxComponentIdentification_txt(cpTp)).c_str());
		}
		else
		{
			addToMe->setText(1, ((std::string)"No Type, context <" + ctxt.std_str() + ">").c_str());
		}
		addToMe->setText(2, descr.c_str());
		addToMe->setText(3, conn.c_str());
	}
	addToMe->setText(4, ctxt.c_str());

	addToMe->setBackground(1, locCol);
	addToMe->setBackground(2, locCol);
	addToMe->setBackground(3, locCol);
	addToMe->setBackground(4, locCol);

	if (showMe)
	{
		addToMe->setText(5, comm.c_str());
		addToMe->setText(6, origin.c_str());
	}

	addToMe->setText(7, (jvxErrorType_txt(res)));
	addToMe->setText(8, mess.c_str());
	addToMe->setText(9, origin_err.c_str());

	if (res != JVX_NO_ERROR)
	{
		addToMe->setBackground(7, QBrush(Qt::red));
		addToMe->setBackground(8, QBrush(Qt::red));
		addToMe->setBackground(9, QBrush(Qt::red));
	}
	
	if (showMe)
	{
		stepCall->getNumberEntries(&num);
		if (num > 0)
		{
			QTreeWidgetItem* newItemSubCall = new QTreeWidgetItem(addToMe);
			newItemSubCall->setBackground(0, QBrush(Qt::lightGray));
			newItemSubCall->setText(0, ("-- " + jvx_size2String(num) + " entries --").c_str());
			newItemSubCall->setText(1, "------------");
			newItemSubCall->setText(2, "------------");
			newItemSubCall->setText(3, "------------");
			newItemSubCall->setText(4, "------------");
			newItemSubCall->setText(5, "------------");
			newItemSubCall->setText(6, "------------");
			newItemSubCall->setText(7, "------------");
			if (filterEntrAct)
			{
				newItemSubCall->setExpanded(true);
			}
			for (i = 0; i < num; i++)
			{
				stepCall->getEntry(&descr, &mess, NULL, NULL, i);

				QTreeWidgetItem* newItem = new QTreeWidgetItem(newItemSubCall);
				newItem->setText(0, (descr.std_str()).c_str());
				newItem->setText(1, mess.c_str());
				if (filterEntrAct)
				{
					newItem->setExpanded(true);
				}
			}
		}
	}

	stepCall->getNumberNext(&num);
	if (num > 0)
	{
		QTreeWidgetItem* newItemSubCall = new QTreeWidgetItem(addToMe);
		newItemSubCall->setText(0, ("-- " + jvx_size2String(num) + " subcalls --").c_str());
		newItemSubCall->setBackground(0, QBrush(Qt::lightGray));
		newItemSubCall->setText(1, "------------");
		newItemSubCall->setText(2, "------------");
		newItemSubCall->setText(3, "------------");
		newItemSubCall->setText(4, "------------");
		newItemSubCall->setText(5, "------------");
		newItemSubCall->setText(6, "------------");
		newItemSubCall->setText(7, "------------");
		newItemSubCall->setExpanded(true);
		for (i = 0; i < num; i++)
		{
			QTreeWidgetItem* newItem = new QTreeWidgetItem(newItemSubCall);
			stepCall->getNext(&loc, &theNext, i);
			addLayerTree(newItem, theNext, loc.std_str(), colId, colors);
			newItem->setExpanded(true);
		}
	}
}

void
jvx_connect_results::print_file()
{
	std::ofstream localOut;
	localOut.open(fName);
	if (localOut.is_open())
	{
		callProtStore->printResult(localOut, 0);
		localOut.close();
	}
}

void 
jvx_connect_results::toggle_full()
{
	if (!isFullScreen())
	{
		showFullScreen();
	}
	else
	{
		showNormal();
	}
}

void 
jvx_connect_results::set_filename()
{
#ifdef JVX_OS_WINDOWS
	QString fN = QFileDialog::getSaveFileName(this, tr("Open Output File for Connection Printout"), tr(".\\"), tr("(*.txt)"));
#else
	QString fN = QFileDialog::getSaveFileName(this, tr("Open Output File for Connection Printout"), tr("./"), tr("(*.txt)"));
#endif
	if (!fN.isEmpty())
	{
		fName = fN.toLatin1().data();
		updateWindow(false);
	}
}

void
jvx_connect_results::toggle_activeFilterEntries()
{
	filterEntrAct = !filterEntrAct;
	updateWindow(true);
}

void
jvx_connect_results::set_filterEntries()
{
	tokenFilter = lineEdit_filterEntries->text().toLatin1().data();
	if (filterEntrAct)
	{
		updateWindow(true);
	}
}
