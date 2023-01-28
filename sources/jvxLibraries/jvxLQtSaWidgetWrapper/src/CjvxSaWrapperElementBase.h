#ifndef __CjvwWrapperElementBase_H__
#define __CjvwWrapperElementBase_H__

#include "jvx.h"
#include "CjvxQtSaWidgetWrapper_types.h"
#include "CjvxQtSaWidgetWrapper_keymap.h"

#include <QtWidgets/QWidget>
class CjvxQtSaWidgetWrapper;

/*
 * Processing
 * property value -> transform_to -> min/max -> show
 * user input -> min/max -> transform_from -> property value
 */
#define CHECK_NONBLOCKING_RESPONSE(propFeatures, callContext) \
	((jvx_bitTest(propFeatures, JVX_PROPERTY_ACCESS_DELAYED_RESPONSE_SHIFT)) && (callContext == JVX_WIDGET_RWAPPER_UPDATE_NORMAL))

#define JVX_IF_NONBLOCKING_RESPONSE(propFeatures, callContext) \
	if CHECK_NONBLOCKING_RESPONSE(propFeatures, callContext)


typedef enum
{
	JVX_WIDGET_RWAPPER_UPDATE_NORMAL,
	JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE
} jvxPropertyCallContext;

typedef enum
{
	JVX_WIDGET_RWAPPER_OPERATION_GET_DESCRIPTOR_SHIFT = 0x0,
	JVX_WIDGET_RWAPPER_OPERATION_GET_EXTENDED_INFO_SHIFT = 0x1,
	JVX_WIDGET_RWAPPER_OPERATION_SET_PROPERTY_SHIFT = 0x2,
	JVX_WIDGET_RWAPPER_OPERATION_UPDATE_WINDOW_SHIFT = 0x3,
	JVX_WIDGET_RWAPPER_OPERATION_UPDATE_STATUS_SHIFT = 0x4

} jvxPropertyCallOperation;

typedef enum
{
	JVX_WIDGET_RWAPPER_FAILED,
	JVX_WIDGET_RWAPPER_INITIALIZE,
	JVX_WIDGET_RWAPPER_UPDATE_WINDOW,
	JVX_WIDGET_RWAPPER_UPDATE_WINDOW_PERIODIC,
	JVX_WIDGET_RWAPPER_GET_CONTENT,
	JVX_WIDGET_RWAPPER_SET_CONTENT,
	JVX_WIDGET_RWAPPER_UPDATE_STATUS,
	JVX_WIDGET_RWAPPER_UPDATE_PROPERTY
} jvxPropertyDelayedCallPurpose;

struct jvxPropertyCallPrivate
{
	// We need to store the parameters when triggering the 
	// corresponding update process to, e.g., run the secondary update
	// function only on the exact same amount of elements as during the first 
	// run.
	// Typically, this feature is only used when running groups of properties 
	// from the widget wrapper class CjvxQtSaWidgetWrapper
	jvxPropertyDelayedCallPurpose delayedCallPurpose;
	std::string selector_lst;
	std::string wildcard;
	jvxSize groupid_emit;
	jvxSize prop_id;
	jvxPropertyCategoryType prop_cat;
	jvxBool prop_conly;
	jvxBool passExternalUId;
	jvxSize* externalUId;

	/*
	jvxPropertyCallPrivate(const char* origin)
	{
		std::cout << "Allocate:" << this << " in this context: " << origin << std::endl;
		*/
	jvxPropertyCallPrivate()
	{
		delayedCallPurpose = JVX_WIDGET_RWAPPER_UPDATE_PROPERTY;
		//selector_lst = selector_lst;
		//prv->wildcard = "";
		groupid_emit = 0;
		prop_id = 0;
		prop_cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		prop_conly = false;
		externalUId = NULL;
		passExternalUId = false;
	};
	~jvxPropertyCallPrivate()
	{
		// std::cout << "Deallocate:" << this << std::endl;
	}
};

class CjvxSaWrapperElementBase: public QObject, public IjvxAccessProperties_delayed_report
{
	Q_OBJECT
public:

protected:

	CjvxQtSaWidgetWrapper* backwardRef;
	IjvxAccessProperties* propRef;
	jvxBitField thePropAFeatures;
	QWidget* uiRef;
	std::string propertyGetSetTag;
	std::vector<std::string> paramLst;
	jvxPropertyDescriptor thePropDescriptor;
	std::string propDescription;
	std::string propDescriptor;
	std::string propName;
	std::string propPrefix;
	jvxBool isUnchecked;
	jvxBool errorState;
	std::string tag;
	std::string selector_lst;
	std::string uiObjectName;
	jvxData transformScale;
	basePropInfos myBaseProps;
	jvxState widgetStatus;
	jvxSize procId;
	jvxSize idPostpone;
	jvxSize procIdP;
	jvxSize assoc_id;
	std::vector<std::string> dependencies;
	std::string ttip_core;
	std::string ttip_show;
	jvxBool contentOnly = true;
	IjvxHiddenInterface* theHostRef;
	std::string delayedActionName;
	jvxCBitField delayedActionOperations;
	jvxCBitField delayedActionPending;
	jvxSize loopCheckCnt;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		JVX_DEFINE_RT_ST_INSTANCES
#endif

public:
	CjvxSaWrapperElementBase(QWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, IjvxAccessProperties* propRefIn,
		std::string propertyNameLoc, std::vector<std::string> paramLstLoc, const std::string& tag, const std::string& selector_lst, const std::string& uiObjectName,
		const std::string& propPrefix, jvxBool verboseLoc, jvxSize uId, IjvxHiddenInterface* hostRef);

	virtual ~CjvxSaWrapperElementBase();

	virtual jvxErrorType initializeProperty_core(jvxBool checkProperty, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
	virtual void setProperties_core_ui(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
	virtual jvxErrorType setProperties_core_direct(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, 
		jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
	virtual void updateWindow_core(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL);
	virtual void updateStatus_core(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL);

	jvxErrorType initParameters_getMin(keyValueList* theLst, std::vector<std::string> & paramLst);

	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr) override;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr) override;



	virtual void initializeUiElement(jvxPropertyCallContext ccontext) = 0;
	virtual void setPropertiesUiElement(jvxCBitField whattoset, jvxPropertyCallContext ccontext) = 0;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) = 0;

	virtual jvxBool updateWindowUiElement_ifavail(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL);
	virtual jvxBool updatePropertyStatus_ifavail(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL);
	virtual jvxBool trigger_updateWindow_periodic_ifavail(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL);

	virtual jvxBool local_command(std::string& local_command);

	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) = 0;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL) = 0;
	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL);

	jvxState get_widget_status()
	{
		return(widgetStatus);
	};

	virtual void clearUiElements() {};
	virtual bool matchesWildcard(const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType);
	virtual bool matchesIdCat(jvxSize propId, jvxPropertyCategoryType cat);

	static void getDisplayUnitToken(jvxWwTransformValueType tp, std::string& unit);
	static jvxData transformTo(jvxData valD, jvxWwTransformValueType tp, jvxData scaleFac);
	static jvxData transformFrom(jvxData valD, jvxWwTransformValueType tp, jvxData scaleFac);

	void jvx_set_transform_scale(jvxData val);
	jvxData jvx_get_transform_scale();

	virtual jvxBool updateRealtime() { return(myBaseProps.updateRt); };
	jvxBool verboseOut() { return(myBaseProps.verbose_out); };

	static void resetBasePropInfos(basePropInfos& mbp);

	static void init_propertyPrivate(jvxPropertyCallPrivate* prv, jvxPropertyDelayedCallPurpose purp, const std::string& tag,
		const std::string& wildcard, jvxSize groupid_emit); // 0 means: no specific group!
	static void init_propertyPrivate(jvxPropertyCallPrivate* prv, jvxPropertyDelayedCallPurpose purp,
		const std::string& tag, jvxSize pId,
		jvxPropertyCategoryType pCat, jvxBool content_only);


	virtual jvxErrorType triggerDirect(const std::string& propertyDescr, jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, const std::string& uiFilterWildcard = "");

	virtual jvxErrorType set_config(jvxWwConfigType tp, jvxHandle* load);

	virtual void updateUiDescriptor(jvxAccessProtocol accProt);

	jvxErrorType handleAccessDelayed_start(const char* callEnter, jvxCBitField oper);
	void handleAccessDelayed_stop(const char* callEnter, jvxCBitField oper);

	std::string getDelayedStatusExpression(jvxCBitField oper);

	void check_start_new_trigger_if_desired(jvxPropertyCallContext ccontext);
	void mainWidgetEnable();
	void mainWidgetDisable();

signals:
	void emit_reenter_delayed_initialize();
	void emit_trigger_updateWindow();
	void emit_trigger_updateStatus();
	void emit_send_content();

public slots:
	void slot_reenter_delayed_initialize();
	void slot_trigger_updateWindow();
	void slot_trigger_updateStatus();
	void slot_send_content();
};

#endif
