#include "mexJvxHost.h"
#include "realtimeViewer_helpers.h"
#include "HjvxMex2CConverter.h"
#include <sstream>

jvxErrorType
mexJvxHost::list_connection_rules(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string nmRule;
			jvxSize idRule = JVX_SIZE_UNSELECTED;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2String(nmRule, prhs, paramId, nrhs);
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:				
				parametersOk = true;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				res = mexArgument2Index<jvxSize>(idRule, prhs, paramId, nrhs); 
				if (res != JVX_NO_ERROR)
				{
					MEX_PARAMETER_ERROR("connection rule id", paramId, "numeric");
				}
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if (parametersOk)
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], true);
				}
				if (nlhs > 1)
				{
					this->mexReturnStructConnectionRules(plhs[1], nmRule, idRule);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::list_connections(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string nmConnection;
			jvxSize idConnection = JVX_SIZE_UNSELECTED;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2String(nmConnection, prhs, paramId, nrhs);
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				parametersOk = true;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				res = mexArgument2Index<jvxSize>(idConnection, prhs, paramId, nrhs);
				if (res != JVX_NO_ERROR)
				{
					MEX_PARAMETER_ERROR("connection id", paramId, "numeric");
				}
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if (parametersOk)
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], true);
				}
				if (nlhs > 1)
				{
					this->mexReturnStructConnectionProcesses(plhs[1], nmConnection, idConnection);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::add_connection_rule(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string nmRule;
			jvxComponentIdentification cpTp0, cpTp1;
			std::string par1, par2;

			bool parametersOk = true;
			jvxInt32 paramId = 0;

			jvxConnectionRuleDropzone dz;

			jvxSize numBridges = nrhs - 5;
			numBridges = numBridges / 7;
			if (numBridges * 7 + 5 != nrhs)
			{
				MEX_PARAMETER_ERROR("connection rule", paramId, "string");
				parametersOk = false;
			}
			else
			{

				// ==============================
				// Read in name of new rule
				// ==============================
				paramId = 1;
				res = mexArgument2String(nmRule, prhs, paramId, nrhs);
				switch (res)
				{
				case JVX_NO_ERROR:
					break;
				case JVX_ERROR_ID_OUT_OF_BOUNDS:
					MEX_PARAMETER_ERROR("connection rule name", paramId, "string");
					parametersOk = false;
					break;
				case JVX_ERROR_INVALID_ARGUMENT:
					MEX_PARAMETER_ERROR("connection rule name", paramId, "string");
					parametersOk = false;
					break;
				default:
					this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
					break;
				}

				// Read in master component id
				paramId++;
				res = mexArgument2ComponentIdentification(cpTp0, prhs, paramId, nrhs);
				switch (res)
				{
				case JVX_NO_ERROR:
					break;
				case JVX_ERROR_ID_OUT_OF_BOUNDS:
					MEX_PARAMETER_ERROR("master component identification", paramId, "string");
					parametersOk = false;
					break;
				case JVX_ERROR_INVALID_ARGUMENT:
					MEX_PARAMETER_ERROR("master component identification", paramId, "string");
					parametersOk = false;
					break;
				default:
					this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
					break;
				}

				// Read in master factory wildcard
				paramId++;
				res = mexArgument2String(par1, prhs, paramId, nrhs);
				switch (res)
				{
				case JVX_NO_ERROR:
					break;
				case JVX_ERROR_ID_OUT_OF_BOUNDS:
					MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
					parametersOk = false;
					break;
				case JVX_ERROR_INVALID_ARGUMENT:
					MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
					parametersOk = false;
					break;
				default:
					this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
					break;
				}

				// Read in master wildcard
				paramId++;
				res = mexArgument2String(par2, prhs, paramId, nrhs);
				switch (res)
				{
				case JVX_NO_ERROR:
					break;
				case JVX_ERROR_ID_OUT_OF_BOUNDS:
					MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
					parametersOk = false;
					break;
				case JVX_ERROR_INVALID_ARGUMENT:
					MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
					parametersOk = false;
					break;
				default:
					this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
					break;
				}

				if (parametersOk)
				{
					dz.rule_name = nmRule;
					dz.cpMaster = cpTp0;
					dz.interceptors = false;
					dz.masterFactoryWildcard = par1;
					dz.masterWildcard = par2;

					for (i = 0; i < numBridges; i++)
					{
						jvxConnectionRuleBridge br;

						// Read in bridge name
						paramId++;
						res = mexArgument2String(par1, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						if (parametersOk)
						{
							br.bridge_name = par1;
						}
						else
						{
							break;
						}

						// =====================================================================

						// Read in from component id
						paramId++;
						res = mexArgument2ComponentIdentification(cpTp0, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master component identification", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master component identification", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						// Read in from factory wildcard
						paramId++;
						res = mexArgument2String(par1, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						// Read in from wildcard
						paramId++;
						res = mexArgument2String(par2, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						if (parametersOk)
						{
							br.fromCpTp = cpTp0;
							br.fromFactoryWildcard = par1;
							br.fromConnectorWildcard = par2;
						}
						else
						{
							break;
						}

						// ==========================================================================

						// Read in to component id
						paramId++;
						res = mexArgument2ComponentIdentification(cpTp0, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master component identification", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master component identification", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						// Read in to factory wildcard
						paramId++;
						res = mexArgument2String(par1, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master fac wildcard", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						// Read in to wildcard
						paramId++;
						res = mexArgument2String(par2, prhs, paramId, nrhs);
						switch (res)
						{
						case JVX_NO_ERROR:
							break;
						case JVX_ERROR_ID_OUT_OF_BOUNDS:
							MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
							parametersOk = false;
							break;
						case JVX_ERROR_INVALID_ARGUMENT:
							MEX_PARAMETER_ERROR("master wildcard", paramId, "string");
							parametersOk = false;
							break;
						default:
							this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
							break;
						}

						if (parametersOk)
						{
							br.toCpTp = cpTp0;
							br.toFactoryWildcard = par1;
							br.toConnectorWildcard = par2;
						}
						else
						{
							break;
						}

						if (parametersOk)
						{
							dz.bridges.push_back(br);
						}
					}
				}
			}

			if (parametersOk)
			{
				IjvxDataConnections* theDataConnections = NULL;
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
				assert(theDataConnections);

				res = jvx_rule_from_dropzone(theDataConnections, &dz);

				if (res == JVX_NO_ERROR)
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to add data connection rule, reason: " + jvxErrorType_descr(res), res);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::rem_connection_rule(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string nmRule;
			jvxSize idRule = JVX_SIZE_UNSELECTED;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2String(nmRule, prhs, paramId, nrhs);
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				parametersOk = true;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				res = mexArgument2Index<jvxSize>(idRule, prhs, paramId, nrhs);
				if (res != JVX_NO_ERROR)
				{
					MEX_PARAMETER_ERROR("connection rule id", paramId, "numeric");
				}
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if (parametersOk)
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], true);
				}
				if (nlhs > 1)
				{
					this->mexReturnStructSequencer(plhs[1]);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::check_ready_for_start(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{

			IjvxDataConnections* theDataConnections = NULL;
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

			jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
			if ((res == JVX_NO_ERROR) && theDataConnections)
			{
				jvxApiString reasonIfNot;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], true);
				}

				jvxErrorType resL = theDataConnections->ready_for_start(&reasonIfNot);
				if (resL == JVX_NO_ERROR)
				{
					mexReturnStructReadyForStartResult(plhs[1], true, "");
				}
				else
				{
					mexReturnStructReadyForStartResult(plhs[1], false, reasonIfNot.std_str());
				}
				mexFillEmpty(plhs, nlhs, 2);
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
				theDataConnections = NULL;
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}
	
jvxErrorType
mexJvxHost::return_result_chain_master(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;
	jvxSize paramId;
	std::string txt;
	bool parametersOk = true;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			jvxSize uidProcess = JVX_SIZE_UNSELECTED;
			jvxApiString errMess;
			paramId = 1;
			res = mexArgument2Index<jvxSize>(uidProcess, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("process id", paramId, "numeric");
				parametersOk = false;
			}

			if (parametersOk)
			{
				IjvxDataConnections* theDataConnections = NULL;
				IjvxDataConnectionProcess* theRef = NULL;
				IjvxConnectionMaster* theMas = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
				jvxBool testOk = false;

				jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
				if ((res == JVX_NO_ERROR) && theDataConnections)
				{
					jvxApiString nmChain;
					JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

					theDataConnections->reference_connection_process_uid(uidProcess, &theRef);
					if (theRef)
					{
						// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
						jvxErrorType resL = theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_REF_CALL_A(fdb));
						if (testOk)
						{
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}

							std::ostringstream outs;
							fdb__.printResult(outs, 0);

							if (nlhs > 1)
							{
								mexReturnString(plhs[1], outs.str());
							}
							mexFillEmpty(plhs, nlhs, 2);
						}
						else
						{
							theRef->descriptor_connection(&nmChain);
							fdb->getErrorShort(NULL, &errMess, NULL);
							txt = "Testing signal processing chain failed, reason: ";
							txt += errMess.std_str();

							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if (nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + txt.c_str(), JVX_ERROR_NOT_READY);
							}
							mexFillEmpty(plhs, nlhs, 2);
						}
						theDataConnections->return_reference_connection_process(theRef);
					}
					else
					{
						txt = "Could not address process with unique Id <" + jvx_size2String(uidProcess) + ">, reason: ";

						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + txt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
					theDataConnections = NULL;
				}
				else
				{
					assert(0);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::test_chain_master(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;
	jvxSize paramId;
	std::string txt;
	bool parametersOk = true;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			jvxSize uidProcess = JVX_SIZE_UNSELECTED;
			jvxApiString errMess;
			paramId = 1;
			res = mexArgument2Index<jvxSize>(uidProcess, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("process id", paramId, "numeric");
				parametersOk = false;
			}
				
			if (parametersOk)
			{
				IjvxDataConnections* theDataConnections = NULL;
				IjvxDataConnectionProcess* theRef = NULL;
				IjvxConnectionMaster* theMas = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
				jvxBool testOk = false;

				jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
				if ((res == JVX_NO_ERROR) && theDataConnections)
				{
					jvxApiString nmChain;
					JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

					theDataConnections->reference_connection_process_uid(uidProcess, &theRef);
					if (theRef)
					{
						// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
						jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

						res = theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_REF_CALL_A(fdb));
						if (res == JVX_NO_ERROR)
						{
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}

							if (nlhs > 1)
							{
								this->mexReturnBool(plhs[1], testOk);
							}
							if (nlhs > 2)
							{
								std::stringstream ss;
								fdb->printResult(ss, 0);
								this->mexReturnString(plhs[2], ss.str());
							}

							mexFillEmpty(plhs, nlhs, 3);
						}
						else
						{
							theRef->descriptor_connection(&nmChain);
							fdb->getErrorShort(NULL, &errMess, NULL);
							txt = "Testing signal processing chain failed, reason: ";
							txt += errMess.std_str();

							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if (nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + txt.c_str(), JVX_ERROR_NOT_READY);
							}
							mexFillEmpty(plhs, nlhs, 2);
						}
						theDataConnections->return_reference_connection_process(theRef);
					}
					else
					{
						txt = "Could not address process with unique Id <" + jvx_size2String(uidProcess) +">, reason: ";
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + txt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
					theDataConnections = NULL;
				}
				else
				{
					assert(0);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}