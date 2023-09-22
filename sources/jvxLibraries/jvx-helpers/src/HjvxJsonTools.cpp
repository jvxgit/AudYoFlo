#include "jvx-helpers.h"
#include "CjvxJson.h"
namespace jvx {
	namespace helper {
		namespace json {
			jvxErrorType commandRequest2Json(const CjvxReportCommandRequest& request, std::string& jsonTxt)
			{
				jvxErrorType res = JVX_NO_ERROR;
				CjvxJsonElement jelm;
				CjvxJsonElementList jelml;
				CjvxJsonElementList jelmlspec;
				jvxApiString astr;
				jvxReportCommandRequest req = request.request();
				jvxReportCommandDataType dtype = request.datatype();
				jvxComponentIdentification cpId = request.origin();

				const CjvxReportCommandRequest_id* idPtr = nullptr;
				const CjvxReportCommandRequest_ss* ssPtr = nullptr;
				const CjvxReportCommandRequest_uid* uidPtr = nullptr;
				const CjvxReportCommandRequest_seq* seqPtr = nullptr;
				

				jelm.makeAssignmentString("req", jvxReportCommandRequest_txt(req));
				jelml.addConsumeElement(jelm);

				jelm.makeAssignmentString("type", jvxReportCommandDataType_txt(dtype));
				jelml.addConsumeElement(jelm);

				CjvxJsonElementList jelmlOrig;
				jelm.makeAssignmentString("component_identification", jvxComponentIdentification_txt(cpId));
				jelmlOrig.addConsumeElement(jelm);

				jelm.makeAssignmentInt("uid", cpId.uId);
				jelmlOrig.addConsumeElement(jelm);

				jelm.makeSection("origin", jelmlOrig);
				jelml.addConsumeElement(jelm);

				switch (dtype)
				{
				case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:

					idPtr = castCommandRequest< CjvxReportCommandRequest_id>(request);
					if (idPtr)
					{

						idPtr->ident(&astr);
						jelm.makeAssignmentString("ident", astr.std_str());
						jelmlspec.addConsumeElement(jelm);
					}
					else
					{
						res = JVX_ERROR_INVALID_FORMAT;
					}
					break;

				case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS:

					ssPtr = castCommandRequest< CjvxReportCommandRequest_ss>(request);
					if (ssPtr)
					{
						jvxStateSwitch ss = JVX_STATE_SWITCH_NONE;
						ssPtr->sswitch(&ss);
						jelm.makeAssignmentString("sswitch", jvxStateSwitch_txt(ss));
						jelmlspec.addConsumeElement(jelm);
					}
					else
					{
						res = JVX_ERROR_INVALID_FORMAT;
					}
					break;

				case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:

					uidPtr = castCommandRequest< CjvxReportCommandRequest_uid>(request);
					if (uidPtr)
					{
						jvxSize uid = JVX_SIZE_UNSELECTED;
						uidPtr->uid(&uid);
						jelm.makeAssignmentInt("uid", JVX_SIZE_INT(uid));
						jelmlspec.addConsumeElement(jelm);
					}
					else
					{
						res = JVX_ERROR_INVALID_FORMAT;
					}
					break;

				case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ:

					seqPtr = castCommandRequest< CjvxReportCommandRequest_seq>(request);
					if (seqPtr)
					{
						TjvxSequencerEvent ev;
						seqPtr->seq_event(&ev);
						jelm.makeAssignmentString("seq-state", jvxSequencerStatus_txt(ev.seqStat));
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentString("seqev-descr", ev.description.std_str());
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentString("seqev-qtp", jvxSequencerQueueType_txt(ev.tp));
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentString("seqev-etp", jvxSequencerElementType_txt(ev.stp));
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentInt("seqev-stpid", ev.stepId);
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentInt("seqev-seqid", ev.sequenceId);
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentInt("seqev-stpfid", ev.fId);
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentString("seqev-ferror", ((ev.indicateFirstError == true) ? "yes" : "no"));
						jelmlspec.addConsumeElement(jelm);
						jelm.makeAssignmentInt("seqev-state-id", ev.current_state);
						jelmlspec.addConsumeElement(jelm);						
						jelm.makeAssignmentString("seqev-mask", jvx_size2String(ev.event_mask));
						jelmlspec.addConsumeElement(jelm);
					}
					else
					{
						res = JVX_ERROR_INVALID_FORMAT;
					}
					break;
				}
				if (res == JVX_NO_ERROR)
				{
					jelm.makeSection("specific", jelmlspec);
					jelml.addConsumeElement(jelm);
					jelml.printToJsonView(jsonTxt);
				}
				return res;
			};
		}
	}
}