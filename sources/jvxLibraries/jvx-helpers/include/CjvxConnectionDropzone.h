#ifndef __CJVXCONNECTIONDROPZONE_H__
#define __CJVXCONNECTIONDROPZONE_H__

#include "jvx-helpers.h"

class CjvxConnectionDropzone
{
protected:
	typedef struct
	{
		struct
		{
			jvxSize mas_fac_uid;
			jvxSize mas_id;
		} identify;
	} lst_elm_master;

	typedef struct
	{
		struct
		{
			jvxSize fac_uid;
			jvxSize icon_id;
		} identify;
	} lst_elm_icons;

	typedef struct
	{
		struct
		{
			jvxSize fac_uid;
			jvxSize ocon_id;
		} identify;
	} lst_elm_ocons;

	typedef struct
	{
		struct
		{
			jvxSize fac_uid;
			jvxSize icon_id;
		} identify_in;
		struct
		{
			jvxSize fac_uid;
			jvxSize ocon_id;
		} identify_out;
		std::string bridge_name;
		jvxBool dedicatedThread;
		jvxBool boostThread;
		jvxSize uIdB;
	} lst_elm_bidges;

	std::string process_name;
	std::string process_description;
	std::list<lst_elm_master> lst_masters;
	std::list<lst_elm_icons> lst_inputs;
	std::list<lst_elm_ocons> lst_outputs;
	std::list<lst_elm_bidges> lst_bridges;

	jvxSize id_select_icon;
	jvxSize id_select_ocon;
	jvxSize id_select_bridge;
	jvxSize id_select_master;

public:

	CjvxConnectionDropzone();

	jvxErrorType connect_process_from_dropzone(
		IjvxDataConnections* allConnections, IjvxDataConnectionProcess* theProc
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	jvxErrorType connect_group_from_dropzone(
		IjvxDataConnections* allConnections, IjvxDataConnectionGroup* theGroup
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	void jvx_cleanup_candidates_dropzone(IjvxDataConnections* allConnections);

	void jvx_reset_dropzone();

	void jvx_set_process_name(const char* nm);

	void jvx_set_process_description(const char* descr);

	void jvx_add_master(jvxSize mas_fac_uid, jvxSize mas_id, jvxBool reset_id);

	void jvx_add_bridge(const char* bridge_name, jvxSize out_fac_uid, jvxSize out_ocon_id, jvxSize in_fac_uid, jvxSize in_icon_id, jvxBool ded_thread, jvxBool boost_thread);

};

#endif