#ifndef __JVX_CONNECT_RESULTS_H__
#define __JVX_CONNECT_RESULTS_H__

#include "ui_jvx_connect_results.h"
#include "jvx.h"

class jvx_connect_results : public QDialog, public Ui::Dialog
{
private:
	typedef struct
	{
		jvxSize colId;
		std::string context;
		std::string connector;
		std::string description;
		jvxComponentIdentification cpTp;
	} oneColorEntry;

	Q_OBJECT
	IjvxCallProt* callProtStore;
	std::string fName;
	jvxBool filterEntrAct;
	std::string tokenFilter;
	
protected:
	void addLayerTree(QTreeWidgetItem* addToMe, IjvxCallProt* stepCall, const std::string& entry,
		jvxSize& colId, std::list< oneColorEntry>& colors);
public:

	jvx_connect_results(QWidget* parent);
	~jvx_connect_results();
	void create(IjvxCallProt* callProt);
	void updateWindow(jvxBool updateTree);
	jvxSize find_color_id(jvxSize& colId, const jvxComponentIdentification& cpTp,
		const std::string& description, const std::string& ctxt, const std::string& connector, std::list<oneColorEntry>&colors);

public slots:
	void print_file();
	void toggle_full();
	void set_filename();
	void toggle_activeFilterEntries();
	void set_filterEntries();
};

#endif