
#include "jvx.h"

// #define TRACK_MEMORY

#ifdef TRACK_MEMORY
static jvxSize numAlloc = 0;
#endif

CjvxCallProt::CjvxCallProt(IjvxCallProt* par, const char * sel)
{
	component.tpId = JVX_COMPONENT_UNKNOWN;

	success.res = JVX_NO_ERROR;
	parent = par;
	call_selector = sel;
};

CjvxCallProt::CjvxCallProt()
{
	component.tpId = JVX_COMPONENT_UNKNOWN;

	success.res = JVX_NO_ERROR;
	parent = NULL;
};

CjvxCallProt::CjvxCallProt(const CjvxCallProt& copyme)
{
	assert(0);
}

CjvxCallProt::~CjvxCallProt()
{
	this->clear();
};

CjvxCallProt&
CjvxCallProt::operator=(const CjvxCallProt& copyme)
{
	this->duplicateFrom(&copyme);
	return *this;
}

jvxErrorType
CjvxCallProt::setErrorCode(jvxErrorType resP, const char* message, const char* origin)
{
	success.res = resP;
	success.error_message = message;
	success.origin_error = origin;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCallProt::setIdentification(jvxComponentIdentification tp, 
	const char* descriptor, const char* conn, 
	const char* origin, const char* ctxt,
	const char* comment)
{
	component.tpId = tp;
	if (descriptor)
	{
		component.name = descriptor;
	}
	else
	{
		component.name = "<unspecified>";
	}
	if (conn)
	{
		component.conn = conn;
	}
	if (origin)
	{
		component.origin = origin;
	}
	if (ctxt)
	{
		context = ctxt;
	}
	if (comment)
	{
		component.comment = comment;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxCallProt::addEntry(const char* selector, const char* message, jvxErrorType res, jvxCallProtClassification theClassification)
{
	std::string txtSel = selector;
	auto elm = jvx_findItemSelectorInList<CjvxCallProt_entry>(entries, txtSel);
	if (elm == entries.end())
	{
		CjvxCallProt_entry entr;
		entr.selector = txtSel;
		entr.message.assign(message);
		entr.result = res;
		entr.classification = theClassification;

		entries.push_back(entr);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
};

jvxErrorType 
CjvxCallProt::addBranch(const char* selector, IjvxCallProt* next)
{
	std::string txtSel =selector;
	auto elm = jvx_findItemSelectorInList<CjvxCallProt_next>(nexts, txtSel);
	if (elm == nexts.end())
	{
		CjvxCallProt_next newElm;
		newElm.selector = selector;
		newElm.ptr = next;
		nexts.push_back(newElm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
};
	
IjvxCallProt* 
CjvxCallProt::newEntry(const char* selector)
{
	CjvxCallProt* newElm = NULL;
	IjvxCallProt* retVal = NULL;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txtSel = selector;
	while (1)
	{
		txtSel = selector;
		if (cnt != 0)
		{
			txtSel += "_";
			txtSel += jvx_size2String(cnt);
		}
		cnt++;
		auto elm = jvx_findItemSelectorInList<CjvxCallProt_next>(nexts, txtSel);
		if (elm == nexts.end())
		{
			newElm = new CjvxCallProt(this, txtSel.c_str());
			IjvxCallProt* newElmI = static_cast<IjvxCallProt*>(newElm);

#ifdef TRACK_MEMORY
			std::cout << __FUNCTION__ << ": Allocated " << newElmI << std::endl;
			numAlloc++;
			std::cout << __FUNCTION__ << ": Number allocated on allocate = " << numAlloc << std::endl;
#endif

			newElm->context = this->context;
			newElm->component.conn = this->component.conn;
			res = addBranch(txtSel.c_str(), newElmI);
			assert(res == JVX_NO_ERROR);
			break;
		}
	}
	
	retVal = static_cast<IjvxCallProt*>(newElm);
	return retVal;
}

jvxErrorType
CjvxCallProt::duplicateFrom(const IjvxCallProt* copyme)
{
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldStr;
	jvxApiString str1;
	jvxApiString str2;
	jvxApiString str3;
	jvxApiString str4;
	jvxApiString str5;

	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType resStored = JVX_NO_ERROR;
	IjvxCallProt::jvxCallProtClassification classStored = JVX_CALL_PROT_INFO;

	this->clear();
	if (copyme)
	{
		resL = copyme->getIdentification(&component.tpId, &str1, &str2, &str3, &str4, &str5);
		assert(resL == JVX_NO_ERROR);

		component.name = str1.std_str();
		component.conn = str2.std_str();
		component.origin = str3.std_str();
		context = str4.std_str();
		component.comment = str5.std_str();

		resL = copyme->getSuccess(&success.res, &str1, &str2);
		assert(resL == JVX_NO_ERROR);

		success.error_message = str1.std_str();
		success.origin_error = str2.std_str();
	
		resL = copyme->getNumberNext(&num);
		assert(resL == JVX_NO_ERROR);

		for(i = 0; i < num; i++)
		{ 
			IjvxCallProt* next = NULL;
			CjvxCallProt* nextc = new CjvxCallProt(this, call_selector.c_str());
			CjvxCallProt_next newElm;

			resL = copyme->getNext(&fldStr, &next, i);
			assert(resL == JVX_NO_ERROR);
			assert(next);
			nextc->duplicateFrom(next);
			newElm.ptr = static_cast<IjvxCallProt*>(nextc);
			newElm.selector = fldStr.std_str();
			nexts.push_back(newElm);
		}

		num = 0;
		resL = copyme->getNumberEntries(&num);
		assert(resL == JVX_NO_ERROR);

		for (i = 0; i < num; i++)
		{
			CjvxCallProt_entry newElm;

			resL = copyme->getEntry(&fldStr, &str1, &resStored, &classStored, i);
			newElm.classification = classStored;
			newElm.message = str1.std_str();
			newElm.result = resStored;
			newElm.selector = fldStr.std_str();
			entries.push_back(newElm);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCallProt::getErrorShort(jvxBool* errorHere, jvxApiString* onRetMess, jvxApiString* onRetLoc)
{
	if (success.res != JVX_NO_ERROR)
	{
		if (onRetMess)
		{
			onRetMess->assign(success.error_message);
		}
		if (errorHere)
		{
			*errorHere = true;
		}
		if(onRetLoc)
		{
			onRetLoc->assign(component.name);
		}
	}
	else
	{
		if (errorHere)
		{
			*errorHere = false;
		}
		for (auto elm = nexts.begin(); elm != nexts.end(); elm++)
		{
			jvxBool err = false;
			success.res = elm->ptr->getErrorShort(&err, onRetMess, onRetLoc);
			assert(success.res == JVX_NO_ERROR);
			if (err)
			{
				if (errorHere)
				{
					*errorHere = true;
				}
				break;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCallProt::getIdentification(jvxComponentIdentification* cpId, jvxApiString* descriptor,
	jvxApiString* conn, jvxApiString* origin, jvxApiString* ctxt, jvxApiString* comm) const
{
	if (cpId)
	{
		*cpId = component.tpId;
	}
	if (descriptor)
	{
		descriptor->assign(component.name);
	}
	if (conn)
	{
		conn->assign(component.conn);
	}
	if(origin)
	{
		origin->assign(component.origin);
	}
	if (ctxt)
	{
		ctxt->assign(context);
	}
	if (comm)
	{
		comm->assign(component.comment);
	}
	return JVX_NO_ERROR;
}
jvxErrorType
CjvxCallProt::getSuccess(jvxErrorType* errTp, jvxApiString* message,
	jvxApiString* origin) const
{
	if (errTp)
	{
		*errTp = success.res;
	}
	if (message)
	{
		message->assign(success.error_message);
	}
	if (origin)
	{
		origin->assign(success.origin_error);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxCallProt::getNumberNext(jvxSize* num) const
{
	if (num)
	{
		*num = nexts.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxCallProt::getNext(jvxApiString* str, IjvxCallProt** next, jvxSize idx) const
{
	jvxSize cnt = 0;
	auto elm = nexts.begin();
	for (; elm != nexts.end(); elm++)
	{
		if (cnt == idx)
		{
			if (next)
			{
				*next = elm->ptr;
			}
			if (str)
				str->assign(elm->selector);

			return JVX_NO_ERROR;
		}
		cnt++;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxCallProt::getNumberEntries(jvxSize* num) const
{
	if (num)
		*num = entries.size();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxCallProt::getEntry(jvxApiString* str, jvxApiString* message,
	jvxErrorType* result, IjvxCallProt::jvxCallProtClassification* classification, jvxSize idx) const
{
	jvxSize cnt = 0;
	auto elm = entries.begin();
	for (; elm != entries.end(); elm++)
	{
		if (cnt == idx)
		{
			if (str)
			{
				str->assign(elm->selector);
			}
			if(message)
			{
				message->assign(elm->message);
			}
			if (result)
			{
				*result = elm->result;
			}
			if (classification)
			{
				*classification = elm->classification;
			}
		
			return JVX_NO_ERROR;
		}
		cnt++;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}


jvxErrorType
CjvxCallProt::getSelector(jvxApiString* str) const
{
	if (str)
		str->assign(call_selector);
	return JVX_NO_ERROR;
}

// ===================================================================================
jvxErrorType 
CjvxCallProt::clear()
{
	std::for_each(nexts.begin(), nexts.end(), [](CjvxCallProt_next& entry)
	{
		entry.ptr->clear();

#ifdef TRACK_MEMORY
		std::cout << __FUNCTION__ << ": Delete " << entry.ptr << std::endl;
#endif

		JVX_DSP_SAFE_DELETE_OBJECT(entry.ptr);

#ifdef TRACK_MEMORY
		numAlloc--;
		std::cout << __FUNCTION__ << ": Number allocated on delete = " << numAlloc << std::endl;
#endif
	});
	nexts.clear();

	entries.clear();

	success.error_message.clear();
	success.origin_error.clear();
	success.res = JVX_NO_ERROR;

	component.name.clear();
	component.conn.clear();
	component.origin.clear();
	component.tpId = JVX_COMPONENT_UNKNOWN;

	call_selector.clear();

	return JVX_NO_ERROR;
}

void
CjvxCallProt::printResult(std::ostream& out, jvxSize level)
{
	jvxSize i;
	jvxSize cnt = 0;
	for (i = 0; i < level; i++)
	{
		out << "--" << std::flush;
	}
	out << "Entering ## component: " << component.name << std::flush;
	if (!component.conn.empty())
		out << "-" << component.conn << std::flush;
	out << "-" << jvxComponentIdentification_txt(component.tpId) << "-" << std::flush;
	if (!component.origin.empty())
	{
		out << component.origin << "-" << std::flush;
	}
	out << "Context=<" << context << ">" << std::endl;
	
	// ============================================================
	for (i = 0; i < level; i++)
	{
		out << "--" << std::flush;
	}
	out << "-- " << entries.size() << " entries --" << std::endl;

	for (auto elm = entries.begin(); elm != entries.end(); elm++)
	{
		for (i = 0; i < level; i++)
		{
			out << "--" << std::flush;
		}
		out << "+[" << cnt << "]+" << std::flush;
		out << "[" << elm->selector << "]:" << std::flush;
		out << jvxErrorType_txt(elm->result) << "--" << elm->message << std::endl;
		cnt++;
	}
	cnt = 0;

	// =========================================================
	for (i = 0; i < level; i++)
	{
		out << "--" << std::flush;
	}
	out << "-- " << nexts.size() << " subcalls --" << std::endl;

	for (auto elm = nexts.begin(); elm != nexts.end(); elm++)
	{
		for (i = 0; i < level; i++)
		{
			out << "--" << std::flush;
		}
		out << "+<" << cnt << ">+" << std::flush;
		out << "<" << elm->selector << ">:" << std::endl;
		elm->ptr->printResult(out, level + 1);
		cnt++;
	}
	for (i = 0; i < level; i++)
	{
		out << "--" << std::flush;
	}
	out << "Leaving function with error code: " << jvxErrorType_txt(success.res) << std::flush;
	if (success.res != JVX_NO_ERROR)
	{
		out << "--" << success.error_message << "--" << success.origin_error << std::endl;
	}
	else
	{
		out << std::endl;
	}
}