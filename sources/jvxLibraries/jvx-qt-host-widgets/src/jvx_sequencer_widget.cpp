#include <QtWidgets/QMessageBox>
#include "jvx_sequencer_widget.h"
#include "jvx-qt-helpers.h"

//Q_DECLARE_METATYPE(jvxUInt64)

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtSequencerWidget, jvxQtSequencerWidget, jvx_sequencer_widget)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtSequencerWidget, jvxQtSequencerWidget)

void 
jvx_sequencer_widget::init(IjvxHost* theHost, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	jvxSize i;
	QStringList lst;
	theHostRef = theHost;
	theBwdRef = bwd;
	theSequencer = NULL;
	theReport = NULL;
	theControl = NULL;
	theSeqRep = NULL;
	this->setupUi(this);

	connect(this, SIGNAL(emit_updateStateSequencer(jvxCBitField, jvxSize, jvxSize, jvxSequencerQueueType)), this, SLOT(updateStateSequencer_inThread(jvxCBitField, jvxSize, jvxSize, jvxSequencerQueueType)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_timerExpired_local()), this, SLOT(timerExpired_local()), Qt::QueuedConnection);

	lst.push_back("Sequence Name");
	lst.push_back("Description");
	lst.push_back("Label");
	treeWidget->setHeaderLabels(lst);
	treeWidget->setSortingEnabled(false);
	this->treeWidget->setColumnCount(3);

	comboBox_assoc_mode->addItem("Error");
	comboBox_run_mode->addItem("Error");
	for (i = 1; i < JVX_SEQUENCER_NUMBER_MODES; i++)
	{
		comboBox_assoc_mode->addItem(("Mode " + jvx_size2String(i)).c_str());
		comboBox_run_mode->addItem(("Mode " + jvx_size2String(i)).c_str());
	}
	this->setWindowTitle("Sequencer Configuration");
}

void 
jvx_sequencer_widget::getMyQWidget(QWidget** retWidget, jvxSize id)
{
	switch (id)
	{
	case 0:
		*retWidget = static_cast<QWidget*>(this);
		break;
	default:
		*retWidget = NULL;
	}
}

void 
jvx_sequencer_widget::terminate()
{
	theHostRef = NULL;
	theBwdRef = NULL;
	theSequencer = NULL;
	theReport = NULL;
	theControl = NULL;
	theSeqRep = NULL;
}

void 
jvx_sequencer_widget::activate()
{
	jvxErrorType res1 = JVX_NO_ERROR;
	jvxErrorType res2 = JVX_NO_ERROR;
	res1 = theHostRef->request_hidden_interface(JVX_INTERFACE_SEQUENCER, (jvxHandle**)&theSequencer);
	res2 = theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, (jvxHandle**)&theReport);

	if (
		(res1 != JVX_NO_ERROR) ||
		(res2 != JVX_NO_ERROR))
	{
		QMessageBox msgBox(QMessageBox::Critical, "Fatal Error", "No access to JVX data connections interface!");
		msgBox.exec();
		exit(-1);
	}

	start_timer_wait(1000);
}

void 
jvx_sequencer_widget::deactivate()
{
	if (theSequencer)
	{
		//theSequencer->remove_all_connection();
		theHostRef->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, (jvxHandle*)theSequencer);
		theSequencer = NULL;
	}
	if (theReport)
	{
		//theSequencer->remove_all_connection();
		theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, (jvxHandle*)theReport);
		theReport = NULL;
	}
	stop_timer_wait();
}

void 
jvx_sequencer_widget::processing_started()
{
}

void 
jvx_sequencer_widget::processing_stopped()
{
}

void 
jvx_sequencer_widget::update_window(jvxCBitField prio)
{
	//verifySetting();
	jvxBitField btf;
	jvx_bitFull(btf);
	updateWindow(btf);
}

void 
jvx_sequencer_widget::update_window_periodic()
{
}

//=========================================================================
void
jvx_sequencer_widget::set_control_ref(IjvxMainWindowControl* cntrl, IjvxQtSequencerWidget_report* theSeq)
{
	theControl = cntrl;
	theSeqRep = theSeq;
}

void
jvx_sequencer_widget::unset_control_ref()
{
	theControl = NULL;
	theSeqRep = NULL;
}

void
jvx_sequencer_widget::update_window_rebuild()
{
	updateWindow_rebuild();
}

//=========================================================================

/**
 * Initialize the jvx_sequencer_widget class. Cross refernces not yet passed.
 *///====================================================================
jvx_sequencer_widget::jvx_sequencer_widget(QWidget* parent): QWidget(parent)
{
	theHostRef = NULL;
	theBwdRef = NULL;
	theSequencer = NULL;
	theReport = NULL;

	//qRegisterMetaType<jvxUInt64>("jvxUInt64");
	runtime.cntLabels = 0;

	others.extTrigger = true;
	others.idQtThread = JVX_GET_CURRENT_THREAD_ID();

	config.min_width = 800;
	config.min_height = 600;

	this->resetCurrentSetup();

	activateApply_step = true;
}

QSize
jvx_sequencer_widget::sizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

QSize
jvx_sequencer_widget::minimumSizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

void
jvx_sequencer_widget::local_close()
{
	parentWidget()->close();
}

/**
 * Function gateway to report an error.
 *///====================================================================
void
jvx_sequencer_widget::reportError(std::string err)
{
	if (theReport)
	{
		theReport->report_simple_text_message(err.c_str(), JVX_REPORT_PRIORITY_ERROR);
	}
}

/**
 * Check that editing is currently allowed. Once the sequencer is in run mode,
 * other operations are not allowed as it assumed that sequencer runs in a different thread.
 *///====================================================================
bool
jvx_sequencer_widget::checkRightStateForEdit()
{
	bool res = true;
	if(theHostRef == NULL)
	{
		res = false;
	}
	return(res);
}

/**
 * Reser the current selection of the user.
 *///====================================================================
void
jvx_sequencer_widget::resetCurrentSetup(bool resetSeq, bool resetStep, bool resetSelect)
{
	if(resetSeq)
	{
		this->currentSetupShown.newSequence.description = "No description";
	}

	if(resetStep)
	{
		this->currentSetupShown.newStep.description = "No description";
		this->currentSetupShown.newStep.elementTp = JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE;
		this->currentSetupShown.newStep.fId = 0;
		this->currentSetupShown.newStep.targetTp = jvxComponentIdentification( JVX_COMPONENT_UNKNOWN);
		this->currentSetupShown.newStep.timeout_ms = -1;
		this->currentSetupShown.newStep.labelTrue = "";
		this->currentSetupShown.newStep.labelFalse = "";
		this->currentSetupShown.newStep.labelFalse = "";

		this->currentSetupShown.newStep.break_active = false;
		this->currentSetupShown.newStep.assoc_mode = JVX_SEQUENCER_MODE_ALL;
	}

	if(resetSelect)
	{
		// No selection made yet
		this->currentSetupShown.selection.sequenceId = JVX_SIZE_UNSELECTED;
		this->currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
		this->currentSetupShown.selection.queueSelection = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	}
}

/**
 * New item has been selected by the user
 *///====================================================================
void
jvx_sequencer_widget::newSelection_currentItem(QTreeWidgetItem* it,int column)
{
	QVariant idVSeq = -1;
	QVariant idVStep = -1;
	QVariant idVRun = JVX_SEQUENCER_QUEUE_TYPE_NONE;

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num, numStepsRun, numStepsLeave;
	jvxApiString fldStr;
	jvxApiString fldLabel;
	jvxApiString fldStr_lab;
	jvxApiString fldStr_lab_true;
	jvxApiString fldStr_lab_false;
	jvxSize idSeq = JVX_SIZE_UNSELECTED;
	jvxSize idStep = JVX_SIZE_UNSELECTED;
	jvxSequencerQueueType tp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxBool b_action = false;
	jvxCBitField assoc_mode = 0;
	idVSeq = it->data(0, Qt::UserRole);
	idVStep = it->data(0, Qt::UserRole+1);
	idVRun = it->data(0, Qt::UserRole+2);

	if(idVSeq.isValid() && idVStep.isValid() && idVRun.isValid())
	{
		idSeq = idVSeq.toInt();
		idStep = idVStep.toInt();
		tp = (jvxSequencerQueueType)idVRun.toInt();
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idStep))
	{
		idStep = 0;
	}

	if (tp == JVX_SEQUENCER_QUEUE_TYPE_NONE)
	{
		tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	}
	this->currentSetupShown.selection.sequenceId = idSeq;
	this->currentSetupShown.selection.stepId = idStep;
	this->currentSetupShown.selection.queueSelection = tp;

	if(JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId))
	{
		theSequencer->number_sequences(&num);
		if (this->currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(this->currentSetupShown.selection.sequenceId, &fldStr, &fldLabel,
				&numStepsRun, &numStepsLeave, NULL);

			this->currentSetupShown.newSequence.description = fldStr.std_str();
			this->currentSetupShown.newSequence.label = fldLabel.std_str();

			/*
			this->lineEdit_sequencename->setText(fldStr.c_str());
			this->lineEdit_sequencelabel->setText(fldLabel.c_str());
			*/
			if (JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.stepId))
			{
				if (this->currentSetupShown.selection.queueSelection == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				{
					if (this->currentSetupShown.selection.stepId < numStepsRun)
					{
						theSequencer->description_step_sequence(this->currentSetupShown.selection.sequenceId,
							this->currentSetupShown.selection.stepId, JVX_SEQUENCER_QUEUE_TYPE_RUN,
							&fldStr, &currentSetupShown.newStep.elementTp, &currentSetupShown.newStep.targetTp,
							&currentSetupShown.newStep.fId, &currentSetupShown.newStep.timeout_ms,
							&fldStr_lab, &fldStr_lab_true, &fldStr_lab_false, &b_action, &assoc_mode);

						currentSetupShown.newStep.description = fldStr.std_str();
						currentSetupShown.newStep.label = fldStr_lab.std_str();
						currentSetupShown.newStep.labelTrue = fldStr_lab_true.std_str();
						currentSetupShown.newStep.labelFalse = fldStr_lab_false.std_str();
						currentSetupShown.newStep.break_active = b_action;
						currentSetupShown.newStep.assoc_mode = assoc_mode;
					}
					else
					{
						this->currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
					}
				}
				else
				{
					if (this->currentSetupShown.selection.queueSelection == JVX_SEQUENCER_QUEUE_TYPE_LEAVE)
					{
						if (this->currentSetupShown.selection.stepId < numStepsLeave)
						{
							theSequencer->description_step_sequence(this->currentSetupShown.selection.sequenceId,
								this->currentSetupShown.selection.stepId, JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
								&fldStr, &currentSetupShown.newStep.elementTp, &currentSetupShown.newStep.targetTp,
								&currentSetupShown.newStep.fId, &currentSetupShown.newStep.timeout_ms,
								&fldStr_lab, &fldStr_lab_true, &fldStr_lab_false, &b_action, &assoc_mode);

							currentSetupShown.newStep.description = fldStr.std_str();
							currentSetupShown.newStep.label = fldStr_lab.std_str();
							currentSetupShown.newStep.labelTrue = fldStr_lab_true.std_str();
							currentSetupShown.newStep.labelFalse = fldStr_lab_false.std_str();
							currentSetupShown.newStep.break_active = b_action;
							currentSetupShown.newStep.assoc_mode = assoc_mode;
						}
						else
						{
							this->currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
						}
					}
				}// else: if(this->currentSetupShown.selection.queueSelection == JVX_SEQUENCER_QUEUE_TYPE_RUN)
			} // if(this->currentSetupShown.selection.stepId >= 0)
		} // if(this->currentSetupShown.selection.sequenceId < num)
		else
		{
			this->resetCurrentSetup(true, true);
		}
		activateApply_step = false;
		jvxBitField btf;
		jvx_bitFull(btf);
		this->updateWindow(btf);
	}// if(this->currentSetupShown.selection.sequenceId >= 0)
}

/**
 * User has pushed button to edit the currently selected step.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_edit()
{
	jvxSize num = 0;
	jvxSize numR = 0;
	jvxSize numL = 0;
	jvxErrorType res = JVX_NO_ERROR;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	if(JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId) && JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.stepId))
	{
		theSequencer->number_sequences(&num);
		if (this->currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &numR, &numL, NULL);
			if (currentSetupShown.selection.queueSelection == JVX_SEQUENCER_QUEUE_TYPE_RUN)
			{
				if (currentSetupShown.selection.stepId < numR)
				{
					theSequencer->edit_step_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.newStep.elementTp, currentSetupShown.newStep.targetTp, currentSetupShown.newStep.description.c_str(),
						currentSetupShown.newStep.fId, currentSetupShown.newStep.timeout_ms, currentSetupShown.newStep.label.c_str(),
						currentSetupShown.newStep.labelTrue.c_str(), currentSetupShown.newStep.labelFalse.c_str(), 
						currentSetupShown.newStep.break_active,
						currentSetupShown.newStep.assoc_mode);
				}
			}
			else
			{
				if (currentSetupShown.selection.stepId < numL)
				{
					theSequencer->edit_step_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.newStep.elementTp, currentSetupShown.newStep.targetTp, currentSetupShown.newStep.description.c_str(),
						currentSetupShown.newStep.fId, currentSetupShown.newStep.timeout_ms, currentSetupShown.newStep.label.c_str(),
						currentSetupShown.newStep.labelTrue.c_str(), currentSetupShown.newStep.labelFalse.c_str(), 
						currentSetupShown.newStep.break_active,
						currentSetupShown.newStep.assoc_mode);
				}
			}
			activateApply_step = false;
			updateWindow_rebuild();
		}
	}
}

/**
 * User has pushed button to insert a new sequence step.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_insert()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	if(JVX_CHECK_SIZE_UNSELECTED(currentSetupShown.selection.sequenceId))
	{
		newButtonPush_seqadd();
	}

	if(JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		res = theSequencer->number_sequences(&num);
		if (currentSetupShown.selection.sequenceId < num)
		{
			std::string lab = currentSetupShown.newStep.label;
			std::string desc = currentSetupShown.newStep.description;

			theSequencer->insert_step_sequence_at(currentSetupShown.selection.sequenceId, currentSetupShown.selection.queueSelection,
				currentSetupShown.newStep.elementTp, currentSetupShown.newStep.targetTp, desc.c_str(),
				currentSetupShown.newStep.fId, currentSetupShown.newStep.timeout_ms, lab.c_str(),
				currentSetupShown.newStep.labelTrue.c_str(), currentSetupShown.newStep.labelFalse.c_str(),
				currentSetupShown.newStep.break_active,
				currentSetupShown.newStep.assoc_mode,
				currentSetupShown.selection.stepId + 1);
			currentSetupShown.selection.stepId += 1;
		}
		else
		{
			assert(0);
		}
	}
	this->updateWindow_rebuild();
}

/**
 * User has pushed button to remove currently selected step.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_remove()
{
	jvxSize num = 0;
	jvxSize numR = 0;
	jvxSize numL = 0;
	jvxErrorType res = JVX_NO_ERROR;
	
	if(!this->checkRightStateForEdit())
	{
		return;
	}

	if(JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId) && JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.stepId))
	{
		theSequencer->number_sequences(&num);
		if (this->currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &numR, &numL, NULL);
			switch (currentSetupShown.selection.queueSelection)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				if (currentSetupShown.selection.stepId < numR)
				{
					theSequencer->remove_step_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId);

					if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.stepId) && currentSetupShown.selection.stepId >= 1)
					{
						currentSetupShown.selection.stepId--;
					}
					else
					{
						if (numR >= 1)
						{
							currentSetupShown.selection.stepId = 0;
						}
						else
						{
							currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
						}
					}
				}
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				if (currentSetupShown.selection.stepId < numL)
				{
					theSequencer->remove_step_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId);
					if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.stepId) && currentSetupShown.selection.stepId >= 1)
					{
						currentSetupShown.selection.stepId--;
					}
					else
					{
						if (numL >= 1)
						{
							currentSetupShown.selection.stepId = 0;
						}
						else
						{
							currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
						}
					}
				}
				break;
			default:
				assert(0);
			}

			this->updateWindow_rebuild();
		}
	}
}

/**
 * User has pushed button to move curent step up one position.
 *///====================================================================
void
jvx_sequencer_widget:: newButtonPush_up()
{
	jvxSize num = 0;
	jvxSize numR = 0;
	jvxSize numL = 0;
	jvxErrorType res = JVX_NO_ERROR;
	
	if(!this->checkRightStateForEdit())
	{
		return;
	}

	if(JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId) && 
		JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.stepId))
	{
		theSequencer->number_sequences(&num);
		if (JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId) && this->currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &numR, &numL, NULL);
			switch (currentSetupShown.selection.queueSelection)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.stepId) && (currentSetupShown.selection.stepId < numR) && (currentSetupShown.selection.stepId >= 1))
				{
					theSequencer->switch_steps_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.selection.stepId - 1);
					currentSetupShown.selection.stepId--;

				}
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				if (currentSetupShown.selection.stepId < numL)
				{
					theSequencer->switch_steps_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.selection.stepId - 1);
					currentSetupShown.selection.stepId--;
				}
				break;
			default:
				assert(0);
			}

			this->updateWindow_rebuild();
		}
	}
}

/**
 * User has pushed button to move current step one position down.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_down()
{
	jvxSize num = 0;
	jvxSize numR = 0;
	jvxSize numL = 0;
	jvxErrorType res = JVX_NO_ERROR;
	
	if(!this->checkRightStateForEdit())
	{
		return;
	}

	if(JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.sequenceId) && JVX_CHECK_SIZE_SELECTED(this->currentSetupShown.selection.stepId))
	{
		theSequencer->number_sequences(&num);
		if (this->currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &numR, &numL, NULL);
			switch (currentSetupShown.selection.queueSelection)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				if (currentSetupShown.selection.stepId < numR - 1)
				{
					theSequencer->switch_steps_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.selection.stepId + 1);
					currentSetupShown.selection.stepId++;

				}
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				if (currentSetupShown.selection.stepId < numL - 1)
				{
					theSequencer->switch_steps_sequence(
						currentSetupShown.selection.sequenceId,
						currentSetupShown.selection.queueSelection,
						currentSetupShown.selection.stepId,
						currentSetupShown.selection.stepId + 1);
					currentSetupShown.selection.stepId++;

				}
				break;
			default:
				assert(0);
			}

			this->updateWindow_rebuild();

		}
	}
}

/**
 * User has specified a new timeout selection in the step editor area.
 *///====================================================================
void
jvx_sequencer_widget::newText_timeout_step()
{
	QString txt = this->lineEdit_timeout_step->text();
	bool ok = false;
	jvxData val = txt.toData(&ok);
	if(ok)
	{
		if(val > 0)
		{
			this->currentSetupShown.newStep.timeout_ms = val;

		}
		else
		{
			this->currentSetupShown.newStep.timeout_ms = -1;
		}
		// Set the timeout value
		this->setTimeout();
	}
	else
	{
		this->currentSetupShown.newStep.timeout_ms = -1;
	}
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has specified new label text for this sequencer step.
 *///====================================================================
void
jvx_sequencer_widget::newText_label_step()
{
	this->currentSetupShown.newStep.label = lineEdit_name_label->text().toLatin1().constData();
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected new label for condition true jump target
 *///====================================================================
void
jvx_sequencer_widget::newSelection_labeltrue_step(int val)
{
	currentSetupShown.newStep.labelTrue = this->comboBox_labeltrue_p1_step->currentText().toLatin1().constData();
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected new label for condition false jump target
 *///====================================================================
void
jvx_sequencer_widget::newSelection_labelfalse_step(int val)
{
	currentSetupShown.newStep.labelFalse = this->comboBox_labelfalse_p2_step->currentText().toLatin1().constData();
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected new command id for current step.
 *///====================================================================
void
jvx_sequencer_widget::newSelection_commandid_step(int)
{
	QVariant qvar = this->comboBox_commandid_step->itemData(comboBox_commandid_step->currentIndex(), Qt::UserRole + 1);
	int idSelection = qvar.toInt();
	this->currentSetupShown.newStep.fId = idSelection;
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected new component for step editor.
 *///====================================================================

void
jvx_sequencer_widget::newSelection_component_step(int val)
{
	if (val < comboBox_component_step->count())
	{
		QVariant var1 = comboBox_component_step->itemData(val, JVX_USER_ROLE_COMPONENT_ID);
		QVariant var2 = comboBox_component_step->itemData(val, JVX_USER_ROLE_COMPONENT_SLOT_ID);
		QVariant var3 = comboBox_component_step->itemData(val, JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);
		if (
			(var1.isValid()) &&
			(var2.isValid()) &&
			(var3.isValid()))
		{
			this->currentSetupShown.newStep.targetTp.tp = (jvxComponentType)var1.toInt();
			this->currentSetupShown.newStep.targetTp.slotid = var2.toInt();
			this->currentSetupShown.newStep.targetTp.slotsubid = var3.toInt();
		}
	}
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected another step type in step editor.
 *///====================================================================

void
jvx_sequencer_widget::newSelection_elementtype_step(int val)
{
	assert(val < JVX_SEQUENCER_TYPE_COMMAND_LIMIT);
	jvxSequencerElementType oldTp = this->currentSetupShown.newStep.elementTp;
	this->currentSetupShown.newStep.elementTp = (jvxSequencerElementType)val;
	if (oldTp != this->currentSetupShown.newStep.elementTp)
	{
		// Reset required
		if (this->currentSetupShown.newStep.elementTp == JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT)
		{
			this->currentSetupShown.newStep.labelTrue = "My Message";
			this->currentSetupShown.newStep.labelFalse = "";
		}
	}
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has pushed button to add an (empty) sequence
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_seqadd()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxSize cnt;
	std::string d, l;
	jvxSize j;
	if(!this->checkRightStateForEdit())
	{
		return;
	}

	theSequencer->number_sequences(&num);
	cnt = 0;
	while (1)
	{
		d = currentSetupShown.newSequence.description;
		if (cnt > 0)
		{
			d += "_" + jvx_size2String(cnt);
		}
		jvxBool avail = true;
		jvxApiString descrLoc;
		for (j = 0; j < num; j++)
		{
			theSequencer->description_sequence(j, &descrLoc, NULL, NULL, NULL, NULL);
			if (d == descrLoc.std_str())
			{
				avail = false;
				cnt++;
				break;
			}
		}
		if (avail)
		{
			break;
		}
	}

	if (!currentSetupShown.newSequence.label.empty())
	{
		cnt = 0;
		while (1)
		{
			l = currentSetupShown.newSequence.label;
			if (cnt > 0)
			{
				l += "_" + jvx_size2String(cnt);
			}
			jvxBool avail = true;
			jvxApiString labLoc;
			for (j = 0; j < num; j++)
			{
				theSequencer->description_sequence(j, NULL, &labLoc, NULL, NULL, NULL, NULL);
				if (l == labLoc.std_str())
				{
					avail = false;
					cnt++;
					break;
				}
			}
			if (avail)
			{
				break;
			}
		}
	}
	res = theSequencer->add_sequence(d.c_str(), l.c_str());
	if (res == JVX_NO_ERROR)
	{
		res = theSequencer->number_sequences(&num);
		currentSetupShown.selection.sequenceId = (jvxInt16)num - 1;
		currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
		currentSetupShown.selection.queueSelection = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	}
	else
	{
		this->reportError(std::string("Adding a new sequence failed, error: ") + jvxErrorType_descr(res));
	}

	this->updateWindow_rebuild();
}

/**
 * User has pushed button to reset current sequence.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_seqreset()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->reset_sequences();
	assert(res == JVX_NO_ERROR);
	this->resetCurrentSetup();

	this->updateWindow_rebuild();
}

/**
 * User has pushed button to remove a sequence.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_seqremove()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->number_sequences(&num);
	if (res == JVX_NO_ERROR)
	{
		if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId) && (currentSetupShown.selection.sequenceId < num))
		{
			res = theSequencer->remove_sequence(currentSetupShown.selection.sequenceId);
			assert(res == JVX_NO_ERROR);
			currentSetupShown.selection.sequenceId = currentSetupShown.selection.sequenceId - 1;
		}
		currentSetupShown.selection.stepId = JVX_SIZE_UNSELECTED;
		currentSetupShown.selection.queueSelection = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	}

	this->updateWindow_rebuild();
}

/**
 * User has specified new desription for a sequence.
 *///====================================================================

void
jvx_sequencer_widget::newText_description_sequence()
{
	currentSetupShown.newSequence.description = lineEdit_sequencename->text().toLatin1().constData();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newText_label_sequence()
{
	currentSetupShown.newSequence.label = lineEdit_sequencelabel->text().toLatin1().constData();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has selected current sequence to be part of the process.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_add_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->number_sequences(&num);
	if (res == JVX_NO_ERROR)
	{
		if (currentSetupShown.selection.sequenceId < num)
		{
			res = theSequencer->mark_sequence_process(currentSetupShown.selection.sequenceId, true);
		}
	}

	this->updateWindow_rebuild();
}

/**
 * User has pushed button to start process
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_start_process()
{
	// TODO myParent->control_startSequencer();
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	theSequencer->status_process(&stat, NULL, NULL, NULL, NULL);
	if (stat == JVX_SEQUENCER_STATUS_NONE)
	{
		if (theControl)
		{
			jvxErrorType res = theControl->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START, NULL);
			if (res != JVX_NO_ERROR)
			{
				this->reportError("Failed to start sequencer - maybe no sequence has been engaged?");
			}
		}
	}
}

/**
 * User has pushed button to start process
 *///====================================================================
jvxErrorType
jvx_sequencer_widget::control_start_process(jvxSize period_msec, jvxSize granularity_state_report)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	JVX_GET_TICKCOUNT_US_SETREF(&runtime.myTimerRef);
	runtime.firstError = "";

	JVX_GET_TICKCOUNT_US_SETREF(&others.tStamp);
	res = theSequencer->run_process(static_cast<IjvxSequencer_report*>(this), (jvxCBitField)-1, granularity_state_report);
	
	start_timer(period_msec);

	runtime.externalTrigger = true;
	if (res == JVX_NO_ERROR)
	{
		emit timerExpired_local();
	}
	else
	{
		this->reportError(std::string("Starting process sequence failed, error: ") + jvxErrorType_descr(res));
	}
	return(res);
}

/**
 * User has pushed button to stop process
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_stop_process()
{
	// myParent->control_stopSequencer();
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	theSequencer->status_process(&stat, NULL, NULL, NULL, NULL);
	if (stat != JVX_SEQUENCER_STATUS_NONE)
	{
		if (theControl)
		{
			jvxErrorType res = theControl->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP, NULL);
			assert(res == JVX_NO_ERROR);
		}
	}
}

/**
 * User has pushed button to stop process
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_stop_sequence()
{
	// myParent->control_stopSequencer();
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	theSequencer->status_process(&stat, NULL, NULL, NULL, NULL);
	if (stat != JVX_SEQUENCER_STATUS_NONE)
	{
		if (theControl)
		{
			jvxErrorType res = theControl->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_CONT, NULL);
			assert(res == JVX_NO_ERROR);
		}
	}
}

jvxErrorType
jvx_sequencer_widget::control_stop_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	res = theSequencer->trigger_abort_process();
	if (res == JVX_NO_ERROR)
	{
		// Speed up stop by calling an extra update
		emit emit_timerExpired_local();
	}
	else
	{
		this->reportError(std::string("Stopping process sequence failed, error: ") + jvxErrorType_descr(res));
	}
	return(res);
}

jvxErrorType
jvx_sequencer_widget::control_continue_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	res = theSequencer->trigger_abort_sequence();
	if (res == JVX_NO_ERROR)
	{
		emit emit_timerExpired_local();
	}
	else
	{
		this->reportError(std::string("Stopping process sequence failed, error: ") + jvxErrorType_descr(res));
	}
	return(res);
}

jvxErrorType 
jvx_sequencer_widget::immediate_sequencer_step()
{
	if (runtime.theTimerSeq)
	{
		timerExpired();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvx_sequencer_widget::wait_for_process_stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	res = theSequencer->acknowledge_completion_process();
	if ((res != JVX_NO_ERROR) && (res != JVX_ERROR_NOT_READY))
	{
		this->reportError(std::string("Waiting for process to stop failed, error: ") + jvxErrorType_descr(res));
	}
	return(res);
}

/**
 * User has pushed button to deactivate current sequence in process.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_remove_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->number_sequences(&num);
	if (res == JVX_NO_ERROR)
	{
		if (currentSetupShown.selection.sequenceId < num)
		{
			res = theSequencer->mark_sequence_process(currentSetupShown.selection.sequenceId, false);
		}
	}

	this->updateWindow_rebuild();
}

/**
 * User has specified new description text.
 *///====================================================================
void
jvx_sequencer_widget::newText_description_step()
{
	this->currentSetupShown.newStep.description = this->lineEdit_description_step->text().toLatin1().constData();
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * User has pushed button to move sequence up one position.
 *///====================================================================
void
jvx_sequencer_widget::newButtonPush_up_sequence()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->number_sequences(&num);
	if (currentSetupShown.selection.sequenceId < num)
	{
		if (currentSetupShown.selection.sequenceId > 0)
		{
			res = theSequencer->switch_sequences(currentSetupShown.selection.sequenceId - 1, currentSetupShown.selection.sequenceId);
			assert(res == JVX_NO_ERROR);
			currentSetupShown.selection.sequenceId = currentSetupShown.selection.sequenceId - 1;
		}
	}

	this->updateWindow_rebuild();
}

/**
 * User has pushed button to move sequence up one position.
 *///====================================================================
void
	jvx_sequencer_widget::newButtonPush_down_sequence()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	if(!this->checkRightStateForEdit())
	{
		return;
	}

	res = theSequencer->number_sequences(&num);
	if (currentSetupShown.selection.sequenceId < num)
	{
		if (currentSetupShown.selection.sequenceId < (num - 1))
		{
			res = theSequencer->switch_sequences(currentSetupShown.selection.sequenceId, currentSetupShown.selection.sequenceId + 1);
			assert(res == JVX_NO_ERROR);
			currentSetupShown.selection.sequenceId = currentSetupShown.selection.sequenceId + 1;
		}
	}

	this->updateWindow_rebuild();
}

/**
 * User has toggled loop flag.
 *///====================================================================
void
jvx_sequencer_widget::newSelectionCheckbox_loop(bool flag)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

	theSequencer->enable_loop_process(flag);

	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

/**
 * At this point, update callbacks are caught. Be careful, these callbacks are not with9in QT thread.
 *///====================================================================

jvxErrorType
jvx_sequencer_widget::report_event(jvxSequencerStatus stat, jvxCBitField event_mask, const char* description,
	jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, 
	jvxSequencerElementType stp, jvxSize fId, jvxSize operation_state, jvxBool inMainLoop)
{
	std::string txt;
	JVX_THREAD_ID idT = JVX_GET_CURRENT_THREAD_ID();

	/*
	if(idT == others.idQtThread)
	{
		report_in_thread = true;
	}*/

	jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.myTimerRef);
	jvxData tt_msec = (jvxData)tt * 0.001; // in msecs
	txt = "[" + jvx_data2String(tt_msec, 2) + " msec]";

	if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
	{
		txt += "Startup process complete, message: ";
		txt += description;

		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
	{
		txt += "Startup sequence complete, message: ";
		txt += description;

		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask, sequenceId, stepId, tp);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE)
	{
		txt = jvx_create_text_seq_report("Report Operation State while Waiting", sequenceId, stepId, tp, stp, fId, operation_state, description);
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
	{
		txt = jvx_create_text_seq_report("Successful completion sequence step", sequenceId, stepId, tp, stp, fId, operation_state, description);

		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);

		if(stp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER)
		{
			if (theControl)
			{
				theControl->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_VIEWER, NULL);
			}
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
	{
		txt = jvx_create_text_seq_report("Incomplete completion sequence", sequenceId, stepId, tp, stp, fId, operation_state, description);
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);
		
		if (inMainLoop)
		{
			if (runtime.firstError.empty())
			{
				runtime.firstError = description;
			}
		}
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
	{
		txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", message ";
		txt += description;
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask,sequenceId,stepId, tp);
		
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
	{
		txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) +" due to error, message: ";
		txt += description;
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);
		
	}

	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
	{
		txt += "Incomplete completion sequence "  + jvx_size2String(sequenceId) + " due to abort, message: ";
		txt += description;
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);
	}

	if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
	{
		txt += "Successful completion process.";
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask, sequenceId,stepId, tp);
		
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
	{
		txt += "Incomplete completion process error, message: ";
		txt += description;
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);

		this->updateStateSequencer_outThread(event_mask, sequenceId, stepId, tp);

		if (theSeqRep)
		{
			theSeqRep->report_sequencer_error(txt.c_str(), runtime.firstError.c_str());			
		}
	}
	if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
	{
		txt += "Process aborted.";
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
		this->updateStateSequencer_outThread(event_mask,sequenceId,stepId, tp);
	}
	if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		txt += "Process terminated.";
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);

		this->updateStateSequencer_outThread(event_mask,sequenceId,stepId, tp);
		theReport->report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
	}

	if(event_mask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
	{
		if(checkBox_debug_sequencer->isChecked())
		{
			txt += description;
			theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
		}
	}

	if (event_mask & JVX_SEQUENCER_EVENT_INFO_TEXT)
	{
		txt += description;
		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
	}
	
	if (event_mask & JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK)
	{
		txt += "Entered Break Condition, message: ";
		txt += description;

		theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		this->updateStateSequencer_outThread(event_mask, sequenceId, stepId, tp);
	}
	//JVX_SEQUENCER_EVENT_SEQUENCE_LEAVE_STEPS_STARTED = 0x80,
	//JVX_SEQUENCER_EVENT_PROCESS_TERMINATED

	return(JVX_NO_ERROR);
}

jvxErrorType 
jvx_sequencer_widget::sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)
{
	if(theSeqRep)
	{
		theSeqRep->report_sequencer_callback(functionId);
	}
	/*
	if(this->myParent->subWidgets.main.theWidget)
	{
		this->myParent->subWidgets.main.theWidget->inform_sequencer_callback(functionId);
	}
	*/
	return(JVX_NO_ERROR);
}

/**
 * Map event callback to QT thread.
 *///====================================================================
void
jvx_sequencer_widget::updateStateSequencer_outThread(jvxCBitField event_mask, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType queuetp)
{
	emit emit_updateStateSequencer(event_mask, sequenceId, stepId, queuetp);
}

/**
 * Event callback within QT thread context.
 *///====================================================================
void
jvx_sequencer_widget::updateStateSequencer_inThread(jvxCBitField event_mask, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType queuetp)
{
	jvxBitField btf;
	if(event_mask == JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize num = 0;
		theSequencer->acknowledge_completion_process();

		if (runtime.theTimerSeq)
		{
			stop_timer();

			if (theSeqRep)
			{
				theSeqRep->report_sequencer_stopped();
			}
		}
	}
	
	jvx_bitZSet(btf, JVX_UPDATE_STATUS_SEQUENCER);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::timerExpired_local()
{
	timerExpired();
}

void
jvx_sequencer_widget::timerExpired()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&others.tStamp);

	res = theSequencer->trigger_step_process_extern(tStamp);
	if ((res == JVX_ERROR_PROCESS_COMPLETE) || (res == JVX_ERROR_ABORT))
	{
		theSequencer->trigger_complete_process_extern(tStamp);
	}
}

void 
jvx_sequencer_widget::timerExpiredWait()
{
}

void
jvx_sequencer_widget::setTimeout()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	std::string txt;
	jvxSize selectionIdx = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	jvxInt64 timeoutms = 0;

	if (!theSequencer)
		return;

	if(JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		switch (currentSetupShown.selection.queueSelection)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			res = theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &num, NULL, NULL);
			if (res == JVX_NO_ERROR)
			{
				if (currentSetupShown.selection.stepId < num)
				{
					res = theSequencer->edit_step_sequence(currentSetupShown.selection.sequenceId, JVX_SEQUENCER_QUEUE_TYPE_RUN,
						currentSetupShown.selection.stepId,
						JVX_SEQUENCER_TYPE_COMMAND_NONE,
						JVX_COMPONENT_UNKNOWN,
						NULL, 0, currentSetupShown.newStep.timeout_ms,
						NULL, NULL, NULL, false, 0,
						JVX_SEQUENCER_EDIT_STEP_TIMEOUT);
				}
			}
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			res = theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, NULL, &num, NULL);
			if (res == JVX_NO_ERROR)
			{
				if (currentSetupShown.selection.stepId < num)
				{
					res = theSequencer->edit_step_sequence(currentSetupShown.selection.sequenceId, JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
						currentSetupShown.selection.stepId,
						JVX_SEQUENCER_TYPE_COMMAND_NONE,
						JVX_COMPONENT_UNKNOWN,
						NULL, 0, currentSetupShown.newStep.timeout_ms,
						NULL, NULL, NULL, false, 0,
						JVX_SEQUENCER_EDIT_STEP_TIMEOUT);
				}
			}
			break;

		}
	}
}

void
jvx_sequencer_widget::status_process(jvxSequencerStatus* statOnReturn, jvxSize* stackDepth)
{
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	if (theSequencer)
	{
		theSequencer->status_process(&stat, NULL, NULL, NULL, NULL);
		theSequencer->current_step_depth(stackDepth);
	}
	if (statOnReturn)
		*statOnReturn = stat;
}

void
jvx_sequencer_widget::newText_process_match()
{
	jvxBitField btf;

	currentSetupShown.newStep.labelTrue = lineEdit_process_descriptor->text().toLatin1().data();
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newSelection_process(int sel)
{
	jvxBitField btf;
	if (sel < comboBox_process_step->count())
	{
		currentSetupShown.newStep.labelTrue = comboBox_process_step->itemText(sel).toLatin1().data();
		jvx_bitFull(btf);
		this->updateWindow(btf);
	}
}

void
jvx_sequencer_widget::pushed_print_code()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	std::string txt;
	jvxSize selectionIdx = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	jvxInt64 timeoutms = 0;
	jvxApiString descr;
	jvxApiString lab;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize j;
	jvxSequencerElementType elemTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification cpId(JVX_COMPONENT_UNKNOWN);
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxApiString labelName;
	jvxApiString labelTrue;
	jvxApiString labelFalse;
	jvxBool b_action = false;
	jvxCBitField assoc_mode = 0;

	if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		theSequencer->number_sequences(&num);
		if (currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->description_sequence(currentSetupShown.selection.sequenceId, &descr, &lab, &numStepsRun, &numStepsLeave, NULL);

			std::cout << "// Description: " << descr.std_str() << ", Label: " << lab.std_str() << std::endl;
			std::cout << "#define NUM_RUN_STEPS " << numStepsRun << std::endl;
			std::cout << "jvxOneSequencerStepDefinition run_steps[NUM_RUN_STEPS] = " << std::endl;
			std::cout << "{" << std::endl;
			for (j = 0; j < numStepsRun; j++)
			{
				theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId, j, JVX_SEQUENCER_QUEUE_TYPE_RUN, &descr,
					&elemTp, &cpId, &funcId, &timeoutms, &labelName, &labelTrue, &labelFalse, &b_action, &assoc_mode);
				std::cout << "\t{ " <<
					jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_RUN].full << ", " <<
					jvxSequencerElementType_str[elemTp].full << ", " <<
					jvxComponentType_str()[cpId.tp].full << ", " <<
					cpId.slotid << ", " <<
					cpId.slotsubid << ", " <<
					"\"" << descr.std_str() << "\", " <<
					"\"" << labelName.std_str() << "\", " <<
					funcId << ", " <<
					timeoutms << ", " <<
					"\"" << labelTrue.std_str() << "\", " <<
					"\"" << labelFalse.std_str() << "\", " <<
					(b_action ? "true" : "false") << ", " << 
					jvx_cbitfield2Hexstring(assoc_mode) << "}" << std::flush;
				if (j < (numStepsRun - 1))
				{
					std::cout << "," << std::flush;
				}
				std::cout << std::endl;
			}
			std::cout << "};" << std::endl;

			std::cout << "#define NUM_LEAVE_STEPS " << numStepsLeave << std::endl;
			std::cout << "jvxOneSequencerStepDefinition leave_steps[NUM_LEAVE_STEPS] = " << std::endl;
			std::cout << "{" << std::endl;
			for (j = 0; j < numStepsLeave; j++)
			{
				theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId, j, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, &descr,
					&elemTp, &cpId, &funcId, &timeoutms, &labelName, &labelTrue, &labelFalse, &b_action, &assoc_mode);
				std::cout << "\t{ " <<
					jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_LEAVE].full << ", " <<
					jvxSequencerElementType_str[elemTp].full << ", " <<
					jvxComponentType_str()[cpId.tp].full << ", " <<
					cpId.slotid << ", " <<
					cpId.slotsubid << ", " <<
					"\"" << descr.std_str() << "\", " <<
					"\"" << labelName.std_str() << "\", " <<
					funcId << ", " <<
					timeoutms << ", " <<
					"\"" << labelTrue.std_str() << "\", " <<
					"\"" << labelFalse.std_str() << "\", " <<
					(b_action ? "true" : "false") << ", " <<
					jvx_cbitfield2Hexstring(assoc_mode) << "}" << std::flush;
				if (j < (numStepsLeave - 1))
				{
					std::cout << "," << std::flush;
				}
				std::cout << std::endl;
			}
			std::cout << "};" << std::endl;

		}
	}
}

void
jvx_sequencer_widget::pushed_duplicate_seq()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxSize cnt;
	std::string d,l;
	jvxInt64 timeoutms = 0;
	jvxApiString descr;
	jvxApiString lab;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize j;
	jvxSequencerElementType elemTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification cpId(JVX_COMPONENT_UNKNOWN);
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxApiString labelName;
	jvxApiString labelTrue;
	jvxApiString labelFalse;
	jvxBool b_action = false;
	jvxCBitField assoc_mode = 0;

	if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		theSequencer->number_sequences(&num);
		if (currentSetupShown.selection.sequenceId < num)
		{
			jvxSize id_from = currentSetupShown.selection.sequenceId;
			jvxSize id_to = num;

			theSequencer->description_sequence(id_from, &descr, &lab, &numStepsRun, &numStepsLeave, NULL, NULL);

			cnt = 0;

			while (1)
			{
				d = descr.std_str();
				if (cnt > 0)
				{
					d += "_" + jvx_size2String(cnt);
				}
				jvxBool avail = true;
				jvxApiString descrLoc;
				for (j = 0; j < num; j++)
				{
					theSequencer->description_sequence(j, &descrLoc, NULL, NULL, NULL, NULL);
					if (d == descrLoc.std_str())
					{
						avail = false;
						cnt++;
						break;
					}
				}
				if (avail)
				{
					break;
				}
			}

			if (!lab.std_str().empty())
			{
				cnt = 0;
				while (1)
				{
					l = lab.std_str();
					if (cnt > 0)
					{
						l += "_" + jvx_size2String(cnt);
					}
					jvxBool avail = true;
					jvxApiString labLoc;
					for (j = 0; j < num; j++)
					{
						theSequencer->description_sequence(j, NULL, &labLoc, NULL, NULL, NULL, NULL);
						if (l == labLoc.std_str())
						{
							avail = false;
							cnt++;
							break;
						}
					}
					if (avail)
					{
						break;
					}
				}
			}
			theSequencer->add_sequence(d.c_str(), l.c_str());
			for (j = 0; j < numStepsRun; j++)
			{
				theSequencer->description_step_sequence(id_from, j, JVX_SEQUENCER_QUEUE_TYPE_RUN, &descr, &elemTp, &cpId, &funcId,
					&timeoutms, &labelName, &labelTrue, &labelFalse, &b_action, &assoc_mode);
				
				theSequencer->insert_step_sequence_at(id_to, JVX_SEQUENCER_QUEUE_TYPE_RUN, elemTp, cpId,
					descr.c_str(), funcId, timeoutms, labelName.c_str(), labelTrue.c_str(),
					labelFalse.c_str(), b_action, assoc_mode);
			}
			for (j = 0; j < numStepsLeave; j++)
			{
				theSequencer->description_step_sequence(id_from, j, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, &descr, &elemTp, &cpId, &funcId,
					&timeoutms, &labelName, &labelTrue, &labelFalse, &b_action, &assoc_mode);
				theSequencer->insert_step_sequence_at(id_to, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, elemTp, cpId,
					descr.c_str(), funcId, timeoutms, labelName.c_str(), labelTrue.c_str(),
					labelFalse.c_str(), b_action, assoc_mode);
			}
		}
		update_window_rebuild();
	}
}

void 
jvx_sequencer_widget::pushed_duplicate_step()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxSequencerElementType elemTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification cpId(JVX_COMPONENT_UNKNOWN);
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxApiString labelName;
	jvxApiString labelTrue;
	jvxApiString labelFalse;
	jvxApiString descr;
	jvxInt64 timeoutms = 0;
	jvxBool b_action = false;
	jvxCBitField assoc_mode = 0;
	if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		theSequencer->number_sequences(&num);
		if (currentSetupShown.selection.sequenceId < num)
		{
			res = theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId,
				currentSetupShown.selection.stepId,
				currentSetupShown.selection.queueSelection, &descr, &elemTp, &cpId, &funcId,
				&timeoutms, &labelName, &labelTrue, &labelFalse, &b_action, &assoc_mode);
			if (res == JVX_NO_ERROR)
			{
				res = theSequencer->insert_step_sequence_at(currentSetupShown.selection.sequenceId,
					currentSetupShown.selection.queueSelection, elemTp, cpId, (descr.std_str() + "-copy").c_str(), funcId, timeoutms,
					labelName.c_str(), labelTrue.c_str(), labelFalse.c_str(), 
					b_action, assoc_mode, currentSetupShown.selection.stepId + 1);
				if (res == JVX_NO_ERROR)
				{
					currentSetupShown.selection.stepId++;
				}
			}
		}
		update_window_rebuild();
	}
}

void
jvx_sequencer_widget::pushed_apply_seq()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	std::string d, l;
	jvxSize cnt;
	jvxSize j;

	if (JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		theSequencer->number_sequences(&num);
		if (currentSetupShown.selection.sequenceId < num)
		{
			theSequencer->number_sequences(&num);
			cnt = 0;
			while (1)
			{
				d = currentSetupShown.newSequence.description;
				if (cnt > 0)
				{
					d += "_" + jvx_size2String(cnt);
				}
				jvxBool avail = true;
				jvxApiString descrLoc;
				for (j = 0; j < num; j++)
				{
					theSequencer->description_sequence(j, &descrLoc, NULL, NULL, NULL, NULL);
					if (j != currentSetupShown.selection.sequenceId)
					{
						if (d == descrLoc.std_str())
						{
							avail = false;
							cnt++;
							break;
						}
					}
				}
				if (avail)
				{
					break;
				}
			}

			if (!currentSetupShown.newSequence.label.empty())
			{
				cnt = 0;
				while (1)
				{
					l = currentSetupShown.newSequence.label;
					if (cnt > 0)
					{
						l += "_" + jvx_size2String(cnt);
					}
					jvxBool avail = true;
					jvxApiString labLoc;
					for (j = 0; j < num; j++)
					{
						theSequencer->description_sequence(j, NULL, &labLoc, NULL, NULL, NULL, NULL);
						if (j != currentSetupShown.selection.sequenceId)
						{

							if (l == labLoc.std_str())
							{
								avail = false;
								cnt++;
								break;
							}
						}
					}
					if (avail)
					{
						break;
					}
				}
			}
			theSequencer->set_description_sequence(currentSetupShown.selection.sequenceId, currentSetupShown.newSequence.description.c_str(), currentSetupShown.newSequence.label.c_str());
		}
		update_window_rebuild();
	}
}

void
jvx_sequencer_widget::newText_labeltrue_p1()
{
	currentSetupShown.newStep.labelTrue = this->lineEdit_labeltrue_p1_step->text().toLatin1().constData();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newText_labelfalse_p2()
{
	currentSetupShown.newStep.labelFalse = this->lineEdit_labelfalse_p2_step->text().toLatin1().constData();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newText_callbackid()
{
	QString qStr = this->lineEdit_argstep->text();
	std::string tStr = qStr.toLatin1().data();
	if (tStr == "++")
	{
		currentSetupShown.newStep.fId = JVX_SIZE_STATE_INCREMENT;
	}
	else if (tStr == "--")
	{
		currentSetupShown.newStep.fId = JVX_SIZE_STATE_DECREMENT;
	}
	else
	{
		currentSetupShown.newStep.fId = qStr.toInt();
	}
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void 
jvx_sequencer_widget::toggle_step_active(bool tog)
{
	theSequencer->set_stepping_mode(tog, false);
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void 
jvx_sequencer_widget::toggle_break_active(bool tog)
{
	theSequencer->set_stepping_mode(false, tog);
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void 
jvx_sequencer_widget::trigger_continue()
{
	theSequencer->trigger_step_continue();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void 
jvx_sequencer_widget::toggle_break_seq(bool tog)
{
	currentSetupShown.newStep.break_active = tog;
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newSelectionAssociatedMode(int idx)
{
	if (jvx_bitTest(currentSetupShown.newStep.assoc_mode, idx))
	{
		jvx_bitClear(currentSetupShown.newStep.assoc_mode, idx);
	}
	else
	{
		jvx_bitSet(currentSetupShown.newStep.assoc_mode, idx);
	}
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::newSelectionRunMode(int idx)
{
	jvxCBitField run_mode = 0;
	theSequencer->get_processing_mode(&run_mode);
	if (jvx_bitTest(run_mode, idx))
	{
		jvx_bitClear(run_mode, idx);
	}
	else
	{
		jvx_bitSet(run_mode, idx);
	}
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::buttonPushedModeAllOff()
{
	jvx_bitFClear(currentSetupShown.newStep.assoc_mode);
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::buttonPushedModeAllOn()
{
	jvx_bitFSet(currentSetupShown.newStep.assoc_mode);
	activateApply_step = true;
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::start_timer(jvxSize period_msec)
{
	stop_timer_wait();
	runtime.theTimerSeq = new QTimer(this);
	connect(runtime.theTimerSeq, SIGNAL(timeout()), this, SLOT(timerExpired()));
	runtime.theTimerSeq->start((int)period_msec);
}

void
jvx_sequencer_widget::stop_timer()
{
	// Stop the QTimer thread
	runtime.theTimerSeq->stop();
	disconnect(this, SLOT(timerExpired()));
	runtime.theTimerSeq = NULL;

	start_timer_wait(1000);
}

void
jvx_sequencer_widget::start_timer_wait(jvxSize period_msec)
{
	theTimerWait = new QTimer(this);
	connect(theTimerWait, SIGNAL(timeout()), this, SLOT(timerExpiredWait()));
	theTimerWait->start((int)period_msec);
}

void
jvx_sequencer_widget::stop_timer_wait()
{
	theTimerWait->stop();
	disconnect(this, SLOT(timerExpiredWait()));
	theTimerWait = NULL;
}
