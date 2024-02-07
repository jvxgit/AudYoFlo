/**
 * Class to share external call handler instance between component and core mexcall handler
 *///===============================================================================

class CjvxMexCallsMin
{
public:
	// Reference to matlab call handler to be used throughout the components life time
	// Shared between inernal and external use
	IjvxExternalCall* _theExtCallHandler = nullptr;
	std::string _theExtCallObjectName;

};