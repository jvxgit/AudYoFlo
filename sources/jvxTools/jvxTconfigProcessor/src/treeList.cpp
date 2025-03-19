#include <iostream>
#include <cassert>
#include "treeList.h"
#include "jvx.h"
#include "CjvxSectionOriginList.h"
// Print out doubles in high precision floating point
#define HIGH_PRECISION_OUT


/*
std::string filterStringsPrint_endbackslash(std::string in)
{
	std::string out = in;
	int numChars = (int)in.size();
	if(numChars > 0)
	{
		// Remove backslash if expression stops with backslash
		if(in[numChars-1] == '\\')
		{
			//out = in.substr(0, numChars-1);
			out = in + " "; // <- maybe the better solution: add a space if backslash is last character
		}
	}
	return(out);
}
*/

treeList::treeList()
{
	treeListElement* singleElementStart = new treeListElement();
	treeListElement* singleElementStop = new treeListElement();
	singleElementStart->belongsto = this;
	singleElementStop->belongsto = this;

	singleElementStart->type = treeListElement::STARTELM;
	singleElementStart->previous = NULL;
	singleElementStart->next = singleElementStop;

	singleElementStop->type = treeListElement::STOPELM;
	singleElementStop->next = NULL;
	singleElementStop->previous = singleElementStart;
	
	this->content = singleElementStart;

	this->refSection = NULL;
}

void
treeList::push_back(treeListElement* newElm)
{
	treeListElement* obj = content;
	while(obj->next)
	{
		obj = obj->next;
	}

	newElm->next = obj;
	obj->previous->next = newElm;
	newElm->previous = obj->previous;
	obj->previous = newElm;
	newElm->belongsto = this;
}

void
treeList::push_front(treeListElement* newElm)
{
	treeListElement* obj = content;
	newElm->next = obj->next;
	obj->next->previous = newElm;
	obj->next = newElm;
	newElm->previous = obj;
	newElm->belongsto = this;
}


void
treeList::setReference(treeListElement* elm)
{
	refSection = elm;
}

treeList::~treeList()
{
	treeListElement* obj = content;
	while(obj)
	{
		treeListElement* nextPtr = obj->next;
		delete(obj);
		obj = nextPtr;
	}
}

treeListElement::~treeListElement()
{
	// Delete all subsections if there are any
	if(this->subsection)
		delete(subsection);
}

treeListElement::treeListElement(jvxAccessRightFlags_rwcd* acc_flags, jvxConfigModeFlags* cfg_flags)//IrtpConfigProcessor::rtpConfigSectionTypes tp)
{
//	isReference = false;
	this->type = UNDEFINED;
/*
	switch(tp)
	{
	case IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_SECTION:
		this->type = treeListElement::SECTION;
		break;
	case IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
		this->type = treeListElement::ASSIGNMENTSTRING;
		break;
	case IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
		this->type = treeListElement::ASSIGNMENTSTRING;
		break;
	case IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_VALUELIST:
		this->type = treeListElement::VALUELIST;
		break;
	case IrtpConfigProcessor::JVX_CONFIG_SECTION_TYPE_STRINGLIST:
		this->type = treeListElement::STRINGLIST;
		break;
	}
*/
	this->isGroup = false;
	this->subsection = NULL;
	this->belongsto = NULL;
	this->next = NULL;
	this->previous = NULL;
	this->nameElement = "UNKNOWN";
	//this->value = oneElm;
	this->assignedString = "UNKNOWN";
	this->filename = "UNKNOWN";
	this->lineno = -1;
	this->isEmpty = false;
	this->referenceAbsolute = false;

	this->acc_flags = 0;
	this->acc_flags_set = false;
	if (acc_flags)
	{
		this->acc_flags = *acc_flags;
		this->acc_flags_set = true;
	}

	this->cfg_flags = 0;
	this->cfg_flags_set = false;
	if (cfg_flags)
	{
		this->cfg_flags = *cfg_flags;
		this->cfg_flags_set = true;
	}
}


void
treeListElement::addSubSection(treeList* addList, std::string name)
{
	this->nameElement = name;
	this->type = SECTION;
	this->subsection = addList;
	addList->setReference(this);
}

void
treeListElement::setGroupStatus(bool isGroup)
{
	this->isGroup = isGroup;
}

void
treeListElement::setToAssignmentValue(jvxValue val, std::string name)
{
	this->type = ASSIGNMENTVALUE;
	this->value = val;
	this->nameElement = name;
}

void
treeListElement::setToAssignmentString(std::string val, std::string name)
{
	this->type = ASSIGNMENTSTRING;
	this->assignedString = val;
	this->nameElement = name;
}

void
treeListElement::setToAssignmentHexValue(jvxBitField val, std::string name)
{
	this->type = ASSIGNMENTHEXSTRING;
	this->assignedString = jvx_bitField2String(val);;
	this->nameElement = name;
}

void
treeListElement::setToAssignmentHexString(std::string val, std::string name)
{
	this->type = ASSIGNMENTHEXSTRING;
	this->assignedString = val;
	this->nameElement = name;
}
void
treeListElement::setToAssignmentValueList(std::string name)
{
	this->type = VALUELIST;
	// this->dataArrayList;
	this->nameElement = name;
	this->isEmpty = true;
}

void
treeListElement::setToReference(std::string expression_key, std::string expression_ref, std::string name, bool absolutePath)
{
	std::string substr;
	this->type = REFERENCE;
	this->nameElement = name;
	size_t pos = 0;
	expression_key = expression_key.substr(1, std::string::npos); // Remove leading @
	referenceKey = expression_key;
	while(1)
	{

		pos = expression_ref.find(':');
		if(pos != std::string::npos)
		{
			substr = expression_ref.substr(0, pos);
			expression_ref = expression_ref.substr(pos+1);
			referenceList.push_back(substr);
		}
		else
		{
			referenceList.push_back(expression_ref);
			break;
		}
	}
	this->referenceAbsolute = absolutePath;
	this->isEmpty = false;
}

void
treeListElement::setToAssignmentValueList(std::vector<std::vector<jvxValue> >& newList, std::string name)
{
	this->type = VALUELIST;
	this->dataArrayList = newList;
	this->nameElement = name;
	if(newList.empty())
		this->isEmpty = true;
}

void
treeListElement::setToAssignmentStringList(std::string name)
{
	this->type = STRINGLIST;
	// this->assignedStringList;
	this->nameElement = name;
	this->isEmpty = true;
}

void
treeListElement::setToAssignmentStringList(std::vector<std::string>& newList, std::string name)
{
	this->type = STRINGLIST;
	this->assignedStringList = newList;
	this->nameElement = name;
	if(newList.empty())
		this->isEmpty = true;
}

void
treeListElement::setToEmptySection(std::string name)
{
	this->type = SECTION;
	this->subsection = new treeList();
	this->nameElement = name;
}

void
treeListElement::setToComment(std::string comment)
{
	// Redefine type
	this->type = COMMENT;
	this->nameElement = comment;
}

void
treeListElement::setToEmptyElement(elementType tp, std::string name)
{
	this->type = tp;
	this->isEmpty = true;
	this->nameElement = name;
}

bool
treeListElement::isSectionEmpty(jvxBool* isEmptyR)
{
	jvxBool iamempty = false;
	if (this->isEmpty)
	{
		iamempty = true;
	}
	else
	{ 
		treeList* ss = this->subsection;
		if (ss == NULL)
		{
			iamempty = true;
		}
		else
		{
			if (ss->content->next == NULL)
			{
				iamempty = true;
			}
			else
			{
				if ((ss->content->type == STARTELM) && (ss->content->next->type == STOPELM))
				{
					iamempty = true;
				}
			}
		}		
	}
	

	if (isEmptyR)
	{
		*isEmptyR = iamempty;
	}
	return(true);
}

/*
 void 
treeListElement::setToValueList(std::string name, int numArraies)
{
	this->type = VALUELIST;
	this->nameElement = name;
	int i = 0; 
	for(i = 0; i < numArraies; i++)
	{
		std::vector<jvxData> lst;
		this->dataArrayList.push_back(lst);
	}
}
*/

bool 
treeListElement::addAssignmentValueList(std::vector<jvxValue> newList )
{
	if(this->type == VALUELIST)
	{
		this->isEmpty = false;
		this->dataArrayList.push_back(newList);
		return(true);
	}	
	return(false);
}

bool
treeListElement::addElementAssignmentValueList(jvxValue value, int idxArray)
{
	int i;
	if(this->type == VALUELIST)
	{
		this->isEmpty = false;
		if(idxArray < 0)
			idxArray = 0;
		
		if(idxArray < (int)this->dataArrayList.size())
		{
			this->dataArrayList[idxArray].push_back(value);
			return(true);
		}
		else
		{
			for(i = (int)dataArrayList.size(); i < idxArray; i++)
			{
				std::vector<jvxValue> newEmptyList;
				dataArrayList.push_back(newEmptyList);
			}
			std::vector<jvxValue> newListOneElement;
			newListOneElement.push_back(value);
			dataArrayList.push_back(newListOneElement);			
		}
	}
	return(false);
}

bool
treeListElement::addElementAssignmentStringList(std::string value)
{
	if(this->type == STRINGLIST)
	{
		this->isEmpty = false;
		this->assignedStringList.push_back(value);
		return(true);
	}
	return(false);
}

void
treeListElement::setOrigin(std::string fName, int lineno)
{
	this->filename = fName;
	this->lineno = lineno;
};

void
treeListElement::setPriSecFlags(const std::string& bf)
{
	std::vector<std::string> lst;
	jvx::helper::parseStringListIntoTokens(bf, lst);
	jvxBool err = false;
	this->acc_flags = 0;
	this->acc_flags_set = false;
	this->acc_flags = 0;
	this->acc_flags_set = false;
	if (lst.size() >= 1)
	{
		if (lst[0].size())
		{
			this->acc_flags = jvx_string2bitfield64(lst[0], err);
			this->acc_flags_set = true;
		}
	}
	if (lst.size() >= 2)
	{
		if (lst[1].size())
		{
			this->cfg_flags = jvx_string2bitfield16(lst[1], err);
			this->cfg_flags_set = true;
		}
	}

};

void
treeList::print(unsigned int numTabs)
{
	unsigned int i,j;
	jvxValue oneElm;
	std::string tabs = "";
	for(i = 0; i < numTabs; i++)
		tabs += "\t";

	treeListElement* obj = this->content;
	while(obj)
	{
		switch (obj->type)
		{
		case treeListElement::STARTELM:
			//std::cout << "(start element)" << std::endl;
			break;
		case treeListElement::STOPELM:
			//std::cout << "(stop element)" << std::endl;
			break;
		case treeListElement::SECTION:
			std::cout << tabs << "SECTION " << obj->nameElement << std::endl;
			std::cout << tabs << "{" << std::endl;
			obj->subsection->print(numTabs + 1);
			std::cout << tabs << "}" << std::endl;
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		case treeListElement::VALUELIST:
			std::cout << tabs << obj->nameElement << " = " << std::flush;
			for (i = 0; i < obj->dataArrayList.size(); i++)
			{
				std::cout << "{" << std::flush;
				for (j = 0; j < obj->dataArrayList[j].size(); j++)
				{
					if (j != 0)
						std::cout << ", " << std::flush;

					oneElm = obj->dataArrayList[j][i];
					switch(oneElm.tp)
					{
					case JVX_DATAFORMAT_DATA:
					
#ifdef HIGH_PRECISION_OUT
						std::cout << jvx_data2String_highPrecision(oneElm.ct.valD, 30) << std::flush;
#else
						std::cout << jvx_data2String(obj->dataArrayList[j][i]) << std::flush;
#endif
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						std::cout << jvx_int322String(oneElm.ct.valI16) + "<i16>";
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						std::cout << jvx_int322String(oneElm.ct.valI32) + "<i32>";
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						std::cout << jvx_int642String(oneElm.ct.valI64) + "<i64>";
						break;
					case JVX_DATAFORMAT_8BIT:
						std::cout << jvx_int322String(oneElm.ct.valI8) + "<i8>";
						break;
					case JVX_DATAFORMAT_U16BIT_LE:
						std::cout << jvx_uint322String(oneElm.ct.valI16) + "<ui16>";
						break;
					case JVX_DATAFORMAT_U32BIT_LE:
						std::cout << jvx_uint322String(oneElm.ct.valI32) + "<ui32>";
						break;
					case JVX_DATAFORMAT_U64BIT_LE:
						std::cout << jvx_uint642String(oneElm.ct.valI64) + "<ui64>";
						break;
					case JVX_DATAFORMAT_U8BIT:
						std::cout << jvx_uint322String(oneElm.ct.valI8) + "<ui8>";
						break;
					case JVX_DATAFORMAT_SIZE:
						std::cout << jvx_size2String(oneElm.ct.valS) << "<s>" << std::flush;
						/*
						if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
						{
							std::cout << "-1<s>" << std::flush;
						}
						else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
						{
							std::cout << "-2<s>" << std::flush;
						}
						else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
						{
							std::cout << "-3<s>" << std::flush;
						}
						else
						{
							std::cout << jvx_size2String(oneElm.ct.valS) + "<s>" << std::flush;
						}
						*/
						break;
					}
				}
				std::cout << "}" << std::flush;
				if (i != (obj->dataArrayList.size() - 1))
				{
					std::cout << ", " << std::flush;
				}
			}
			std::cout << ";" << std::endl;
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		case treeListElement::STRINGLIST:
			std::cout << tabs << obj->nameElement << " = " << "{ " << std::flush;
			for (i = 0; i < obj->assignedStringList.size(); i++)
			{
				if (i != 0)
					std::cout << ", " << std::flush;
				std::cout << obj->assignedStringList[i] << std::flush;
			}
			std::cout << "};" << std::endl;
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		case treeListElement::ASSIGNMENTVALUE:
			oneElm = obj->value;

			

#ifdef HIGH_PRECISION_OUT
				std::cout << jvx_data2String_highPrecision(oneElm.ct.valD, 30) << std::flush;
#else
				std::cout << jvx_data2String(obj->dataArrayList[j][i]) << std::flush;
#endif
				break;
			
			switch (oneElm.tp)
			{
			case JVX_DATAFORMAT_DATA:
#ifdef HIGH_PRECISION_OUT
				std::cout << tabs << obj->nameElement << " = " << jvx_data2String_highPrecision(oneElm.ct.valD, 30) << ";" << std::endl;
#else
				std::cout << tabs << obj->nameElement << " = " << jvx_data2String(obj->value) << ";" << std::endl;
#endif
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_int322String(oneElm.ct.valI16) + "<i16>" << std::endl;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_int322String(oneElm.ct.valI32) + "<i32>" << std::endl;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_int642String(oneElm.ct.valI64) + "<i64>" << std::endl;
				break;
			case JVX_DATAFORMAT_8BIT:
				std::cout << tabs << obj->nameElement << " = " << jvx_int322String(oneElm.ct.valI8) + "<i8>" << std::endl;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_uint322String(oneElm.ct.valUI16) + "<ui16>" << std::endl;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_uint322String(oneElm.ct.valUI32) + "<ui32>" << std::endl;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				std::cout << tabs << obj->nameElement << " = " << jvx_uint642String(oneElm.ct.valUI64) + "<ui64>" << std::endl;
				break;
			case JVX_DATAFORMAT_U8BIT:
				std::cout << tabs << obj->nameElement << " = " << jvx_uint322String(oneElm.ct.valUI8) + "<ui8>" << std::endl;
				break;
			case JVX_DATAFORMAT_SIZE:
				
				std::cout << tabs << obj->nameElement << " = " << jvx_size2String(oneElm.ct.valS) << "<s>" << std::endl;
				/*
				if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
				{
					std::cout << tabs << obj->nameElement << " = " << "-1<s>" << std::endl;
				}
				else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
				{
					std::cout << tabs << obj->nameElement << " = " << "-2<s>" << std::endl;
				}
				else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
				{
					std::cout << tabs << obj->nameElement << " = " << "-3<s>" << std::endl;
				}
				else
				{
					std::cout << tabs << obj->nameElement << " = " << jvx_size2String(oneElm.ct.valS) + "<s>" << std::endl;
				}*/
				break;
			}
			
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		case treeListElement::ASSIGNMENTSTRING:
			std::cout << tabs << obj->nameElement << " = " << obj->assignedString << ";" << std::endl;
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		case treeListElement::COMMENT:
			std::cout << tabs << "/*" << obj->nameElement << " */" << std::endl;
			std::cout << "Origin: " << obj->filename << "::" << obj->lineno << std::endl;
			break;
		}
		obj = obj->next;
	}
}

void
treeList::outputToString(const std::string& path, const std::string& origin, 
	std::string& bufOut, unsigned int numTabs, bool compactForm, 
	CjvxSectionOriginList* decomposeIntoFiles,
	std::list<std::string>* includeReferencesThisSectionIn)
{

	treeListElement* obj = this->content;
	while(obj)
	{
		obj->outputToString(path, origin, bufOut, numTabs + 1,compactForm, decomposeIntoFiles, includeReferencesThisSectionIn);
		obj = obj->next;
	}
}

bool 
treeListElement::outputToString(const std::string& path, const std::string& origin, 
	std::string& bufOutIn, int numTabsIn, bool compactForm, CjvxSectionOriginList* decomposeIntoFiles,
	std::list<std::string>* includeReferencesThisSectionIn)
{
	unsigned int i,j;
	jvxValue oneElm;
	std::string tabs = "";
	std::string bufOutSubSection;
	std::string* bufOutFwd = &bufOutIn;
	
	std::string pathFwd;

	std::string originFwd = origin;
	jvxSize numTabsFwd = numTabsIn;

	jvxBool startedSubSectionToFileHere = false;	
	std::list<std::string> includeReferencesThisSectionLocal;

	std::string flagtagstr = "";

	for (i = 0; i < (unsigned)numTabsFwd; i++)
		tabs += "\t";

	if(this->acc_flags_set || this->cfg_flags_set)
	{
		flagtagstr = " <";
		if (this->acc_flags_set)
		{
			flagtagstr += jvx_bitfield642String(this->acc_flags);
		}
		flagtagstr += ",";
		if (this->cfg_flags_set)
		{
			flagtagstr += jvx_bitfield162String(this->cfg_flags);
		}
		flagtagstr += ">";
	}

	numTabsFwd = numTabsIn;
	bufOutFwd = &bufOutIn;
	startedSubSectionToFileHere = false;
	pathFwd = jvx_makePathExpr(path, this->nameElement);
	// std::cout << "Path = " << pathFwd << " -- " << origin << " ## " << decomposeIntoFiles << "##" << std::endl;
	originFwd = origin;
	if (decomposeIntoFiles)
	{
		auto elm = decomposeIntoFiles->sectionList.find(pathFwd);
		if (elm != decomposeIntoFiles->sectionList.end())
		{
			//std::cout << "Check in list with <" << pathFwd << "> -- " << origin << std::endl;
			originFwd = elm->second.origin;
		}
		else
		{
			//std::cout << "Entry <" << pathFwd << " not found -- " << origin << std::endl;
		}
	}

	if (originFwd != origin)
	{
		bufOutFwd = &bufOutSubSection;
		numTabsFwd = 0;
		startedSubSectionToFileHere = true;

		tabs.clear();
		for (i = 0; i < (unsigned)numTabsFwd; i++)
			tabs += "\t";
	}

	// Special tabs rule here!

	switch(this->type)
	{
	case treeListElement::STARTELM:
		//std::cout << "(start element)" << std::endl;
		break;
	case treeListElement::STOPELM:
		//std::cout << "(stop element)" << std::endl;
		break;
	case treeListElement::SECTION:

		if(!compactForm)
		{
			if(this->isGroup)
			{
				*bufOutFwd += tabs + "GROUP " + this->nameElement + flagtagstr + "\n";
			}
			else
			{
				*bufOutFwd += tabs + "SECTION " + this->nameElement + flagtagstr + "\n";
			}
			*bufOutFwd += tabs + "{" + "\n";
			this->subsection->outputToString(pathFwd, originFwd, *bufOutFwd, numTabsFwd +1, compactForm, decomposeIntoFiles, &includeReferencesThisSectionLocal);
			*bufOutFwd += tabs + "};";
			if(this->lineno >= 0)
			{
				*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
			}
			else
			{
				*bufOutFwd += "\n";
			}
		}
		else
		{
			if(this->isGroup)
			{
				*bufOutFwd += "GROUP " + this->nameElement + flagtagstr + "{";
			}
			else
			{
				*bufOutFwd += "SECTION " + this->nameElement + flagtagstr + "{";
			}
			this->subsection->outputToString(path, origin , *bufOutFwd, numTabsFwd+1, compactForm, decomposeIntoFiles, &includeReferencesThisSectionLocal);
			*bufOutFwd += "};";
		}
		
		break;
	case treeListElement::VALUELIST:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				*bufOutFwd += tabs + "// Empty VALUELIST element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					*bufOutFwd += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
			else
			{
				jvxBool multiList = false;
				if (this->dataArrayList.size() > 1)
				{
					multiList = true;
				}

				*bufOutFwd += tabs + this->nameElement + flagtagstr + " = [";
				if (multiList)
				{
					*bufOutFwd += "\n";
				}

				for (i = 0; i < this->dataArrayList.size(); i++)
				{
					if (multiList)
					{
						*bufOutFwd += tabs + "\t[";
					}
					else
					{
						*bufOutFwd += "[";
					}
					for(j = 0; j < this->dataArrayList[i].size(); j++)
					{
						if(j != 0)
							*bufOutFwd += ", ";
						oneElm = this->dataArrayList[i][j];



						switch (oneElm.tp)
						{
						case JVX_DATAFORMAT_DATA:

#ifdef HIGH_PRECISION_OUT
							* bufOutFwd += jvx_data2String_highPrecision(oneElm.ct.valD, 30);
#else
							bufOut += jvx_data2String(oneElm.ct.valD, 30);
#endif
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI16) + "<i16>";
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI32) + "<i32>";
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							*bufOutFwd += jvx_int642String(oneElm.ct.valI64) + "<i64>";
							break;
						case JVX_DATAFORMAT_8BIT:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI8) + "<i8>";
							break;
						case JVX_DATAFORMAT_U16BIT_LE:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI16) + "<ui16>";
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI32) + "<ui32>";
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							*bufOutFwd += jvx_uint642String(oneElm.ct.valUI64) + "<ui64>";
							break;
						case JVX_DATAFORMAT_U8BIT:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI8) + "<ui8>";
							break;
						case JVX_DATAFORMAT_SIZE:
							
							*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
							/*
							if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
							{
								bufOut += "-1<s>" ;
							}
							else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
							{
								bufOut += "-2<s>" ;
							}
							else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
							{
								bufOut += "-3<s>";
							}
							else
							{
								bufOut += jvx_size2String(oneElm.ct.valS) + "<s>";
							}*/
							break;
						}
					}
					*bufOutFwd += "]";
					if(i != (this->dataArrayList.size()-1))
					{
						*bufOutFwd += "; ";
						*bufOutFwd += "\n";
					}
				}
				if (multiList)
				{
					*bufOutFwd += "\n" + tabs + "];";
				}
				else
				{
					*bufOutFwd += "];";
				}
				if(this->lineno >= 0)
				{
					*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				*bufOutFwd += this->nameElement + flagtagstr + "=[";
				for(i = 0; i < this->dataArrayList.size(); i++)
				{
					*bufOutFwd += "[";
					for(j = 0; j < this->dataArrayList[i].size(); j++)
					{
						if(j != 0)
							*bufOutFwd += ",";
						oneElm = this->dataArrayList[i][j];


						switch (oneElm.tp)
						{
						case JVX_DATAFORMAT_DATA:

#ifdef HIGH_PRECISION_OUT
							* bufOutFwd += jvx_data2String_highPrecision(oneElm.ct.valD, 30);
#else
							* bufOutFwd += jvx_data2String(oneElm.ct.valD, 30);
#endif
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI16) + "<i16>";
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI32) + "<i32>";
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							*bufOutFwd += jvx_int642String(oneElm.ct.valI64) + "<i64>";
							break;
						case JVX_DATAFORMAT_8BIT:
							*bufOutFwd += jvx_int322String(oneElm.ct.valI8) + "<i8>";
							break;
						case JVX_DATAFORMAT_U16BIT_LE:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI16) + "<ui16>";
							break;
						case JVX_DATAFORMAT_U32BIT_LE:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI32) + "<ui32>";
							break;
						case JVX_DATAFORMAT_U64BIT_LE:
							*bufOutFwd += jvx_uint642String(oneElm.ct.valUI64) + "<ui64>";
							break;
						case JVX_DATAFORMAT_U8BIT:
							*bufOutFwd += jvx_uint322String(oneElm.ct.valUI8) + "<ui8>";
							break;
						case JVX_DATAFORMAT_SIZE:
							*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
							/*
							if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
							{
								bufOut += "-1<s>";
							}
							else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
							{
								bufOut += "-2<s>";
							}
							else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
							{
								bufOut += "-3<s>";
							}
							else
							{
								bufOut += jvx_size2String(oneElm.ct.valS) + "<s>";
							}*/
							break;
						}
					}
					*bufOutFwd += "]";
					if(i != (this->dataArrayList.size()-1))
					{
						*bufOutFwd += ";";
					}
				}
				*bufOutFwd += "];";
			}
		}
		break;

	case treeListElement::STRINGLIST:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				*bufOutFwd += tabs + "// Empty STRINGLIST element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					*bufOutFwd += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
			else
			{
				*bufOutFwd += tabs + this->nameElement + flagtagstr + " = " + "{ ";
				for(i = 0; i < this->assignedStringList.size(); i++)
				{
					if( i != 0)
						*bufOutFwd += ", ";
					*bufOutFwd += "\"" + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedStringList[i]) + "\"";
				}
				*bufOutFwd += "};";
				if(this->lineno >= 0)
				{
					*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				*bufOutFwd += this->nameElement + flagtagstr + "=" + "{";
				for(i = 0; i < this->assignedStringList.size(); i++)
				{
					if( i != 0)
						*bufOutFwd += ",";
					*bufOutFwd += "\"" + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedStringList[i]) + "\"";
				}
				*bufOutFwd += "};";
			}
		}
		break;
	case treeListElement::ASSIGNMENTVALUE:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				*bufOutFwd += tabs + "// Empty ASSIGNMENTVALUE element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					*bufOutFwd += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
			else
			{
				*bufOutFwd += tabs + this->nameElement + flagtagstr + " = ";
				oneElm = this->value;
				switch (oneElm.tp)
				{
				case JVX_DATAFORMAT_DATA:

#ifdef HIGH_PRECISION_OUT
					*bufOutFwd += jvx_data2String_highPrecision(oneElm.ct.valD, 30);
#else
					*bufOutFwd += jvx_data2String(oneElm.ct.valD, 30);
#endif
					break;
				case JVX_DATAFORMAT_16BIT_LE:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI16) + "<i16>";
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI32) + "<i32>";
					break;
				case JVX_DATAFORMAT_64BIT_LE:
					*bufOutFwd += jvx_int642String(oneElm.ct.valI64) + "<i64>";
					break;
				case JVX_DATAFORMAT_8BIT:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI8) + "<i8>";
					break;
				case JVX_DATAFORMAT_U16BIT_LE:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI16) + "<ui16>";
					break;
				case JVX_DATAFORMAT_U32BIT_LE:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI32) + "<ui32>";
					break;
				case JVX_DATAFORMAT_U64BIT_LE:
					*bufOutFwd += jvx_uint642String(oneElm.ct.valUI64) + "<ui64>";
					break;
				case JVX_DATAFORMAT_U8BIT:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI8) + "<ui8>";
					break;
				case JVX_DATAFORMAT_SIZE:
					*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
					/*
					if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
					{
						*bufOutFwd += "-1<s>";
					}
					else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
					{
						*bufOutFwd += "-2<s>";
					}
					else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
					{
						*bufOutFwd += "-3<s>";
					}
					else
					{
						*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
					}*/
					break;
				}
				*bufOutFwd += ";";
				if (this->lineno >= 0)
				{
					*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				*bufOutFwd += this->nameElement + flagtagstr + "=";
				oneElm = this->value;

				switch (oneElm.tp)
				{
				case JVX_DATAFORMAT_DATA:

#ifdef HIGH_PRECISION_OUT
					*bufOutFwd += jvx_data2String_highPrecision(oneElm.ct.valD, 30);
#else
					*bufOutFwd += jvx_data2String(oneElm.ct.valD, 30);
#endif
					break;
				case JVX_DATAFORMAT_16BIT_LE:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI16) + "<i16>";
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI32) + "<i32>";
					break;
				case JVX_DATAFORMAT_64BIT_LE:
					*bufOutFwd += jvx_int642String(oneElm.ct.valI64) + "<i64>";
					break;
				case JVX_DATAFORMAT_8BIT:
					*bufOutFwd += jvx_int322String(oneElm.ct.valI8) + "<i8>";
					break;
				case JVX_DATAFORMAT_U16BIT_LE:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI16) + "<ui16>";
					break;
				case JVX_DATAFORMAT_U32BIT_LE:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI32) + "<ui32>";
					break;
				case JVX_DATAFORMAT_U64BIT_LE:
					*bufOutFwd += jvx_uint642String(oneElm.ct.valUI64) + "<ui64>";
					break;
				case JVX_DATAFORMAT_U8BIT:
					*bufOutFwd += jvx_uint322String(oneElm.ct.valUI8) + "<ui8>";
					break;
				case JVX_DATAFORMAT_SIZE:
					*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
					/*
					if (JVX_CHECK_SIZE_UNSELECTED(oneElm.ct.valS))
					{
						*bufOutFwd += "-1<s>";
					}
					else if (JVX_CHECK_SIZE_DONTCARE(oneElm.ct.valS))
					{
						*bufOutFwd += "-2<s>";
					}
					else if (JVX_CHECK_SIZE_RETAIN(oneElm.ct.valS))
					{
						*bufOutFwd += "-3<s>";
					}
					else
					{
						*bufOutFwd += jvx_size2String(oneElm.ct.valS) + "<s>";
					}*/
					break;
				}
				*bufOutFwd += ";";
			}
		}
		break;

	case treeListElement::ASSIGNMENTSTRING:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				*bufOutFwd += tabs + "// Empty ASSIGNMENTSTRING element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					*bufOutFwd += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
			else
			{
				*bufOutFwd += tabs + this->nameElement + flagtagstr + " = \"" + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedString) + "\";" ;
				if(this->lineno >= 0)
				{
					*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				*bufOutFwd += this->nameElement + flagtagstr + "=\"" + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedString) + "\";" ;
			}
		}
		break;
	case treeListElement::ASSIGNMENTHEXSTRING:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				*bufOutFwd += tabs + "// Empty ASSIGNMENTHEXSTRING element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					*bufOutFwd += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
			else
			{
				*bufOutFwd += tabs + this->nameElement + flagtagstr + " = " + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedString) + ";" ;
				if(this->lineno >= 0)
				{
					*bufOutFwd += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					*bufOutFwd += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				*bufOutFwd += this->nameElement + "= " + jvx_replaceSpecialCharactersBeforeWriteSystem(this->assignedString) + ";" ;
			}
		}
		break;
	case treeListElement::COMMENT:
		if(!compactForm)
		{			
			// Only for manually built tree
			*bufOutFwd += tabs + "//" + this->nameElement + "\n" ;
		}
		break;
	}

	if (startedSubSectionToFileHere)
	{
		char str[JVX_MAXSTRING] = { 0 };
		JVX_GETCURRENTDIRECTORY(str, JVX_MAXSTRING);
		originFwd = jvx_absoluteToRelativePath(originFwd, true, str);

		auto elm = decomposeIntoFiles->collections.find(originFwd);
		if (elm != decomposeIntoFiles->collections.end())
		{
			elm->second.content_sections.push_back(bufOutSubSection);
		}
		else
		{
			CjvxSectionOriginList::CjvxOneOutputContent newElm;
			newElm.fName = originFwd;
			newElm.content_sections.push_back(bufOutSubSection);
			decomposeIntoFiles->collections[originFwd] = newElm;

		}

		if (includeReferencesThisSectionIn)
		{
			auto elmSecs = includeReferencesThisSectionIn->begin();
			for (; elmSecs != includeReferencesThisSectionIn->end(); elmSecs++)
			{
				if (*elmSecs == originFwd)
				{
					// An include was already added!!
					break;
				}
			}
			if (elmSecs == includeReferencesThisSectionIn->end())
			{
				tabs.clear();
				for (i = 0; i < (unsigned)numTabsIn; i++)
					tabs += "\t";

				// Generate include only for first occurence
				bufOutIn += tabs + "include \"" + originFwd + "\";\n";
			}
			includeReferencesThisSectionIn->push_back(originFwd);
		}
	}

	return(true);
}

/*
bool 
treeListElement::outputToString4File(std::string& bufOut, int numTabs, bool compactForm)
{
	unsigned int i,j;
	std::string tabs = "";
	for(i = 0; i < (unsigned)numTabs; i++)
		tabs += "\t";	
	
	switch(this->type)
	{
	case treeListElement::STARTELM:
		//std::cout << "(start element)" << std::endl;
		break;
	case treeListElement::STOPELM:
		//std::cout << "(stop element)" << std::endl;
		break;
	case treeListElement::SECTION:
		if(!compactForm)
		{
			bufOut += tabs + "SECTION " + this->nameElement + "\n";
			bufOut += tabs + "{" + "\n";
			this->subsection->outputToString4File(bufOut, numTabs+1,compactForm);
			bufOut += tabs + "};";
			if(this->lineno >= 0)
			{
				bufOut += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
			}
			else
			{
				bufOut += "\n";
			}
		}
		else
		{
			bufOut += "SECTION " + this->nameElement + "{";
			this->subsection->outputToString4File(bufOut, numTabs+1, compactForm);
			bufOut += "};";
		}
		break;
	case treeListElement::VALUELIST:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				bufOut += tabs + "// Empty VALUELIST element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					bufOut += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
			else
			{
				bufOut += tabs + this->nameElement + " = [ ";
				for(i = 0; i < this->dataArrayList.size(); i++)
				{
					bufOut += "[";
					for(j = 0; j < this->dataArrayList[i].size(); j++)
					{
						if(j != 0)
							bufOut += ", ";
#ifdef HIGH_PRECISION_OUT
						bufOut += jvx_data2String_highPrecision(this->dataArrayList[i][j], 30);
#else
						bufOut += jvx_data2String(this->dataArrayList[i][j]);
#endif
					}
					bufOut += "]";
					if(i != (this->dataArrayList.size()-1))
					{
						bufOut += ", ";
					}
				}
				bufOut += "];";
				if(this->lineno >= 0)
				{
					bufOut += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				bufOut += this->nameElement + "=[";
				for(i = 0; i < this->dataArrayList.size(); i++)
				{
					bufOut += "[";
					for(j = 0; j < this->dataArrayList[i].size(); j++)
					{
						if(j != 0)
							bufOut += ",";
#ifdef HIGH_PRECISION_OUT
						bufOut += jvx_data2String_highPrecision(this->dataArrayList[i][j], 30);
#else
						bufOut += jvx_data2String(this->dataArrayList[i][j]);
#endif
					}
					bufOut += "]";
					if(i != (this->dataArrayList.size()-1))
					{
						bufOut += ",";
					}
				}
				bufOut += "];";
			}
		}
		break;
	case treeListElement::STRINGLIST:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				bufOut += tabs + "// Empty STRINGLIST element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					bufOut += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
			else
			{
				bufOut += tabs + this->nameElement +  " = " + "{ ";
				for(i = 0; i < this->assignedStringList.size(); i++)
				{
					if( i != 0)
						bufOut += ", ";
					bufOut += "\"" + filterStrings_specialCharacters(this->assignedStringList[i]) + "\"";
				}
				bufOut += "};";
				if(this->lineno >= 0)
				{
					bufOut += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				bufOut += this->nameElement +  "=" + "{";
				for(i = 0; i < this->assignedStringList.size(); i++)
				{
					if( i != 0)
						bufOut += ",";
					bufOut += "\"" + filterStrings_specialCharacters(this->assignedStringList[i]) + "\"";
				}
				bufOut += "};";
			}
		}
		break;
	case treeListElement::ASSIGNMENTVALUE:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				bufOut += tabs + "// Empty ASSIGNMENTVALUE element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					bufOut += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
			else
			{
#ifdef HIGH_PRECISION_OUT
				bufOut += tabs + this->nameElement + " = " + jvx_data2String_highPrecision(this->value, 30) + ";";
#else
				bufOut += tabs + this->nameElement + " = " + jvx_data2String(this->value) + ";";
#endif
				if(this->lineno >= 0)
				{
					bufOut += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
#ifdef HIGH_PRECISION_OUT
				bufOut += this->nameElement + "=" + jvx_data2String_highPrecision(this->value, 30) + ";";
#else
				bufOut += this->nameElement + "=" + jvx_data2String(this->value) + ";";
#endif
			}
		}
		break;
	case treeListElement::ASSIGNMENTSTRING:
		if(!compactForm)
		{
			if(this->isEmpty)
			{
				bufOut += tabs + "// Empty ASSIGNMENTSTRING element " + this->nameElement + ", removed by configProcessor.";
				if(this->lineno >= 0)
				{
					bufOut += ":: Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
			else
			{
				bufOut += tabs + this->nameElement + " = \"" + filterStrings_specialCharacters(this->assignedString) + "\";" ;
				if(this->lineno >= 0)
				{
					bufOut += "// Origin: " + this->filename + "::" + jvx_int2String(this->lineno) + "\n";
				}
				else
				{
					bufOut += "\n";
				}
			}
		}
		else
		{
			if(!this->isEmpty)
			{
				bufOut += this->nameElement + "=\"" + filterStrings_specialCharacters(this->assignedString) + "\";" ;
			}
		}
		break;
	case treeListElement::COMMENT:
		if(!compactForm)
		{			
			// Only for manually built tree
			bufOut += tabs + "//" + this->nameElement + "\n" ;
		}
		break;
	}
	return(true);
}
*/

int 
treeListElement::getNumberSubsections()
{
	int cnt = 0;
	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if(first->type == treeListElement::SECTION)
			{
				cnt ++;
			}
			first = first->next;
		}
	}
	return(cnt);
}

int 
treeListElement::getNumberEntries()
{
	int cnt = 0;
	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if((first->type != treeListElement::STARTELM) &&(first->type != treeListElement::STOPELM))
			{
				cnt ++;
			}
			first = first->next;
		}
	}
	return(cnt);
}



bool
treeListElement::getCopySubsectionSection_id(treeListElement** dataOut, jvxSize idx)
{
	jvxSize cnt = 0;
	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if((first->type != treeListElement::STARTELM) &&(first->type != treeListElement::STOPELM))
			{
				if(cnt == idx)
				{
					if(dataOut)
					{
						*dataOut = first->copy();
						break;
					}
				}
				cnt ++;
			}
			first = first->next;
		}
	}
	return(true);
}

bool
treeListElement::getCopySubsectionSection_name(treeListElement** dataOut, const char* nameSection)
{
	if(dataOut)
		*dataOut = NULL;

	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if((first->type != treeListElement::STARTELM) &&(first->type != treeListElement::STOPELM))
			{
				if(first->nameElement == (std::string)nameSection)
				{
					if(dataOut)
					{
						*dataOut = first->copy();
						return(true);
					}
				}
			}
			first = first->next;
		}
	}
	return(false);
}

bool
treeListElement::getReferenceSubsectionSection_id(treeListElement** dataOut, jvxSize idx)
{
	int cnt = 0;
	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if(first->type == treeListElement::SECTION)
			{
				if(cnt == idx)
				{
					if(dataOut)
					{
						*dataOut = first;
						break;
					}
				}
				cnt ++;
			}
			first = first->next;
		}
	}
	return(true);
}

bool
treeListElement::getReferenceEntrySection_id(treeListElement** dataOut, jvxSize idx)
{
	int cnt = 0;
	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if((first->type != treeListElement::STARTELM) &&(first->type != treeListElement::STOPELM))
			{
				if(cnt == idx)
				{
					if(dataOut)
					{
						*dataOut = first;
						break;
					}
				}
				cnt ++;
			}
			first = first->next;
		}
	}
	return(true);
}

bool
treeListElement::getReferenceEntrySection_name(treeListElement** dataOut, const char* nameSection)
{
	if(dataOut)
		*dataOut = NULL;

	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if((first->type != treeListElement::STARTELM) &&(first->type != treeListElement::STOPELM))
			{
				if(first->nameElement == (std::string)nameSection)
				{
					if(dataOut)
					{
						*dataOut = first;
						return(true);
					}
				}
			}
			first = first->next;
		}
	}
	return(false);
}

bool
treeListElement::getReferenceSubsectionSection_name(treeListElement** dataOut, const char* nameSection)
{
	if(dataOut)
		*dataOut = NULL;

	if(subsection)
	{
		treeListElement* first =subsection->content;
		while(first)
		{
			if(first->type == treeListElement::SECTION)
			{
				if(first->nameElement == (std::string)nameSection)
				{
					if(dataOut)
					{
						*dataOut = first;
						return(true);
					}
				}
			}
			first = first->next;
		}
	}
	return(false);
}

treeListElement*
treeListElement::copy()
{
	treeListElement* newElement = new treeListElement();

	newElement->assignedString = this->assignedString;
	newElement->assignedStringList = this->assignedStringList;
	newElement->belongsto = NULL;
	newElement->dataArrayList = this->dataArrayList;
	newElement->filename = this->filename;
//	newElement->isReference = false;
	newElement->lineno = this->lineno;
	newElement->nameElement = this->nameElement;
	newElement->next = NULL;
	newElement->previous = NULL;
	newElement->subsection = NULL;
	if(this->subsection)
	{
		newElement->subsection = new treeList;
		treeListElement* ptr = this->subsection->content;
		while(ptr)
		{
			if((ptr->type != treeListElement::STARTELM)&&(ptr->type != treeListElement::STOPELM))
			{
				newElement->subsection->push_back(ptr->copy());
			}
			ptr = ptr->next;
		}
	}
	newElement->type = this->type;
	newElement->value = this->value;
	return(newElement);
}

bool
treeListElement::getOriginSection(std::string& str, int& line)
{
	str = filename;
	line = lineno;
	return(true);
}
