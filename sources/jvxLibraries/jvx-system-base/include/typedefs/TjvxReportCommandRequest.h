#ifndef __HJVXREPORTCOMMANDREQUEST_H__
#define __HJVXREPORTCOMMANDREQUEST_H__

//! Datatype for command requests. All specific requests may be derived from this type
class CjvxReportCommandRequest // : public CjvxReportCommandRequest
{
protected:
	jvxReportCommandRequest req = jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC;
	jvxReportCommandDataType type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE;
	jvxReportCommandBroadcastType broad = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER;
	jvxComponentIdentification orig = JVX_COMPONENT_UNKNOWN;
	jvxHandle* userData = nullptr;
	jvxBool immediate_call = false;

public:
	CjvxReportCommandRequest(jvxReportCommandRequest reqArg, 		
		jvxComponentIdentification originArg = JVX_COMPONENT_UNKNOWN, 
		jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER, 
		jvxHandle* userDataArg = NULL, jvxBool immediateArg = false)
	{
		req = reqArg;
		orig = originArg;
		userData = userDataArg;
		broad = broadArg;
		immediate_call = immediateArg;
	};

	CjvxReportCommandRequest(const CjvxReportCommandRequest& inst)
	{
		req = inst.request();
		type = inst.datatype();
		orig = inst.origin();
		broad = inst.broadcast();
		userData = inst.user_data();
	};

	~CjvxReportCommandRequest()
	{
	};

	virtual jvxReportCommandRequest request() const 
	{
		return req;
	};

	virtual jvxBool immediate() const
	{
		return immediate_call;
	};

	virtual jvxComponentIdentification origin() const
	{
		return orig;
	}

	virtual jvxReportCommandBroadcastType broadcast() const
	{
		return broad;
	}

	virtual void modify_broadcast(jvxReportCommandBroadcastType broadArg) 
	{
		broad = broadArg;
	}

	virtual jvxReportCommandDataType datatype() const
	{
		return type;
	}

	virtual jvxHandle* user_data() const
	{
		return userData;
	}

	// ======================================================================

	virtual jvxErrorType specialization(const jvxHandle**, jvxReportCommandDataType tp) const
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual void set_broadcast(jvxReportCommandBroadcastType broadArg)
	{
		broad = broadArg;
	}

	virtual void set_immediate(jvxBool immArg)
	{
		immediate_call = immArg;
	};

	virtual void set_request(jvxReportCommandRequest reqArg)
	{
		req = reqArg;
	};

};

//! Datatype to reschedule a request to the main thread. The event is described by the "id"
class CjvxReportCommandRequest_rm: public CjvxReportCommandRequest// , public IjvxReportCommandRequest_rm
{
	jvxSize sched_id = JVX_SIZE_UNSELECTED;

public:

	CjvxReportCommandRequest_rm(jvxComponentIdentification cpArg, jvxSize sched_idArg, 
			jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER, 
		jvxHandle* userDataArg = NULL):
		CjvxReportCommandRequest(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN, 
			cpArg, broadArg, userDataArg)
	{
		type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE;
		sched_id = sched_idArg;		
	};

	~CjvxReportCommandRequest_rm()
	{
	};
	
	CjvxReportCommandRequest_rm(const CjvxReportCommandRequest& inst):
		CjvxReportCommandRequest(inst)
	{
		const CjvxReportCommandRequest_rm* spec = NULL;
		inst.specialization(reinterpret_cast<const jvxHandle**>(&spec),
			jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE);
		if (spec)
		{			
			sched_id = spec->schedule_id();
		}	
	}
	virtual jvxErrorType specialization(const jvxHandle** dat, jvxReportCommandDataType tp) const override
	{
		if (tp == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE)
		{
			if (dat)
			{
				*dat = static_cast<const CjvxReportCommandRequest_rm*>(this);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxSize schedule_id() const // override
	{
		return sched_id;
	}
};

//! Datatype to request a specific operation with a string to indicate WHAT needs to be done
class CjvxReportCommandRequest_id : public CjvxReportCommandRequest // , public IjvxReportCommandRequest_id
{
	std::string ident_str;

public:

	CjvxReportCommandRequest_id(jvxReportCommandRequest reqArg, jvxComponentIdentification cpArg, const char* identArg, 
		jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
		jvxHandle* userDataArg = NULL) :
		CjvxReportCommandRequest(reqArg, cpArg, broadArg, userDataArg)
	{
		type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT;
		ident_str = identArg;
	};

	~CjvxReportCommandRequest_id()
	{
	};


	CjvxReportCommandRequest_id(const CjvxReportCommandRequest& inst) :
		CjvxReportCommandRequest(inst)
	{
		const CjvxReportCommandRequest_id* spec = NULL;
		inst.specialization(reinterpret_cast<const jvxHandle**>(&spec),
			jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT);
		if (spec)
		{
			jvxApiString astr;
			spec->ident(&astr);
			ident_str = astr.std_str();
		}
	}
	virtual jvxErrorType specialization(const jvxHandle** dat, jvxReportCommandDataType tp) const override
	{
		if (tp == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT)
		{
			if (dat)
			{
				*dat = static_cast<const CjvxReportCommandRequest_id*>(this);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual void ident(jvxApiString* astr) const
	{
		if (astr)
		{
			astr->assign(ident_str);
		}
	};

};

class CjvxReportCommandRequest_uid : public CjvxReportCommandRequest // , public IjvxReportCommandRequest_uid
{
	jvxSize uid_value;

public:

	CjvxReportCommandRequest_uid(jvxReportCommandRequest reqArg, jvxComponentIdentification cpArg, jvxSize uid,
		jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
		jvxHandle* userDataArg = NULL) :
		CjvxReportCommandRequest(reqArg, cpArg, broadArg, userDataArg)
	{
		type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID;
		uid_value = uid;
	};

	~CjvxReportCommandRequest_uid()
	{
	};


	CjvxReportCommandRequest_uid(const CjvxReportCommandRequest& inst) :
		CjvxReportCommandRequest(inst)
	{
		const CjvxReportCommandRequest_uid* spec = NULL;
		inst.specialization(reinterpret_cast<const jvxHandle**>(&spec),
			jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID);
		if (spec)
		{
			spec->uid(&uid_value);
		}
	}
	virtual jvxErrorType specialization(const jvxHandle** dat, jvxReportCommandDataType tp) const override
	{
		if (tp == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID)
		{
			if (dat)
			{
				*dat = static_cast<const CjvxReportCommandRequest_uid*>(this);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual void uid(jvxSize* uid) const 
	{
		if (uid)
		{
			*uid = uid_value;
		}
	};


	
};

class CjvxReportCommandRequest_ss : public CjvxReportCommandRequest
{
	jvxStateSwitch sswitch_value = JVX_STATE_SWITCH_NONE;

public:

	CjvxReportCommandRequest_ss(jvxReportCommandRequest reqArg, jvxComponentIdentification cpArg, jvxStateSwitch ssArg,
		jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
		jvxHandle* userDataArg = NULL) :
		CjvxReportCommandRequest(reqArg, cpArg, broadArg, userDataArg)
	{
		type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS;
		sswitch_value = ssArg;
	};

	~CjvxReportCommandRequest_ss()
	{
	};


	CjvxReportCommandRequest_ss(const CjvxReportCommandRequest& inst) :
		CjvxReportCommandRequest(inst)
	{
		const CjvxReportCommandRequest_ss* spec = NULL;
		inst.specialization(reinterpret_cast<const jvxHandle**>(&spec),
			jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS);
		if (spec)
		{
			spec->sswitch(&sswitch_value);
		}
	}
	virtual jvxErrorType specialization(const jvxHandle** dat, jvxReportCommandDataType tp) const override
	{
		if (tp == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS)
		{
			if (dat)
			{
				*dat = static_cast<const CjvxReportCommandRequest_ss*>(this);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual void sswitch(jvxStateSwitch* ssRet) const
	{
		if (ssRet)
		{
			*ssRet = sswitch_value;
		}
	};
};

class CjvxReportCommandRequest_seq : public CjvxReportCommandRequest// , public IjvxReportCommandRequest_seq
{
	TjvxSequencerEvent theEvent;

public:

	CjvxReportCommandRequest_seq(jvxReportCommandRequest reqArg, jvxComponentIdentification cpArg, TjvxSequencerEvent* ev,
		jvxReportCommandBroadcastType broadArg = jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
		jvxHandle* userDataArg = NULL) :
		CjvxReportCommandRequest(reqArg, cpArg, broadArg, userDataArg)
	{
		type = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ;
		if (ev)
		{
			theEvent = *ev;
		}
	};

	~CjvxReportCommandRequest_seq()
	{
	};


	CjvxReportCommandRequest_seq(const CjvxReportCommandRequest& inst) :
		CjvxReportCommandRequest(inst)
	{
		const CjvxReportCommandRequest_seq* spec = NULL;
		inst.specialization(reinterpret_cast<const jvxHandle**>(&spec),
			jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ);
		if (spec)
		{
			spec->seq_event(&theEvent);
		}
	}
	virtual jvxErrorType specialization(const jvxHandle** dat, jvxReportCommandDataType tp) const override
	{
		if (tp == jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ)
		{
			if (dat)
			{
				*dat = static_cast<const CjvxReportCommandRequest_seq*>(this);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual void seq_event(TjvxSequencerEvent* ev) const
	{
		if (ev)
		{
			ev->event_mask = theEvent.event_mask;
			ev->description.assign(theEvent.description.std_str());
			ev->sequenceId = theEvent.sequenceId;
			ev->stepId = theEvent.stepId;
			ev->tp = theEvent.tp;
			ev->stp = theEvent.stp;
			ev->fId = theEvent.fId;
			ev->current_state = theEvent.current_state;
			ev->indicateFirstError = theEvent.indicateFirstError;
		}
	}
};

#endif