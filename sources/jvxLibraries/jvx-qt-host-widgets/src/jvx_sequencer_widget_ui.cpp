#include "jvx_sequencer_widget.h"
#include "jvx-qt-helpers.h"

static std::string produceToolTip(jvxSequencerElementType elementType, jvxComponentIdentification targetComponentType,
		jvxSize functionId, jvxInt64 out_timeout_ms, std::string label_cond_true, std::string label_cond_false)
{
	std::string txtOut;

	txtOut += "Type: ";
	txtOut += jvxSequencerElementType_txt(elementType);
	txtOut += ", Target: ";
	txtOut += jvxComponentIdentification_txt(targetComponentType);
	txtOut += ", Function Id: " + jvx_size2String(functionId);
	if(elementType == JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC)

	{
		txtOut += ", Command id: " + jvx_size2String(functionId);
	}
	if(
		(elementType == JVX_SEQUENCER_TYPE_CONDITION_JUMP) ||
		(elementType == JVX_SEQUENCER_TYPE_CONDITION_WAIT))
	{
		txtOut += ", Condition id: " + jvx_size2String(functionId);
	}

	if(elementType == JVX_SEQUENCER_TYPE_CONDITION_JUMP)
	{
		txtOut += ", Target<true>: " + label_cond_true;
		txtOut += ", Target<false>: " + label_cond_true;
	}

	if(elementType == JVX_SEQUENCER_TYPE_JUMP)
	{
		txtOut += ", Target: " + label_cond_true;
	}

	return(txtOut);
}

void
jvx_sequencer_widget::updateWindow(jvxBitField whattoupdate)
{
	jvxSize i;
	std::string txt;
	IjvxSequencer* sequencer = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool valB;
	jvxSequencerStatus stat;
	jvxSize idSeq;
	jvxSize idStep;
	jvxBool actStep = false;
	jvxBool actBreak = false;
	jvxCBitField run_mode;
	jvxBool correctedSetting = false;

	jvxSequencerQueueType tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;

	if (theHostRef)
	{
		if (jvx_bitTest(whattoupdate, JVX_UPDATE_UI_ELEMENTS))
		{
			
			lineEdit_process_descriptor->clear();
			comboBox_process_step->clear();
			lineEdit_process_descriptor->setEnabled(false);
			comboBox_process_step->setEnabled(false);

			this->lineEdit_sequencename->setText(this->currentSetupShown.newSequence.description.c_str());
			this->lineEdit_sequencelabel->setText(this->currentSetupShown.newSequence.label.c_str());
			this->lineEdit_description_step->setText(this->currentSetupShown.newStep.description.c_str());
			txt = this->currentSetupShown.newStep.label;
			if (txt.empty())
			{
				txt = "#Label_" + jvx_int2String(runtime.cntLabels);
			}
			this->lineEdit_name_label->setText(txt.c_str());

			this->comboBox_elementtype_step->clear();
			this->comboBox_commandid_step->clear();
			this->comboBox_component_step->clear();
			this->comboBox_labelfalse_p2_step->clear();
			this->comboBox_labeltrue_p1_step->clear();
			this->lineEdit_labeltrue_p1_step->clear();
			this->lineEdit_labelfalse_p2_step->clear();

			label_labeltrue_p1->setText("--");
			label_labelfalse_p2->setText("--");

			for (i = 0; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
			{
				this->comboBox_elementtype_step->addItem(jvxSequencerElementType_txt(i));
			}
			this->comboBox_elementtype_step->setCurrentIndex(this->currentSetupShown.newStep.elementTp);

			this->comboBox_component_step->clear();

			this->comboBox_labelfalse_p2_step->show();
			this->comboBox_labeltrue_p1_step->show();
			this->lineEdit_labeltrue_p1_step->hide();
			this->lineEdit_labelfalse_p2_step->hide();

			label_command_condition_arg->setText("--");
			comboBox_commandid_step->show();
			lineEdit_argstep->hide();

			for (i = 0; i < JVX_SEQUENCER_NUMBER_MODES; i++)
			{
				QFont myCustomFont = comboBox_assoc_mode->font();
				if (jvx_bitTest(this->currentSetupShown.newStep.assoc_mode, i))
				{
					myCustomFont.setBold(true);
				}
				else
				{
					myCustomFont.setBold(false);
				}

#ifdef JVX_OS_LINUX
				comboBox_assoc_mode->setEditable(true);
#endif
				comboBox_assoc_mode->setItemData(i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
				comboBox_assoc_mode->setEditable(false);
#endif
			}

			theSequencer->get_processing_mode(&run_mode);
			for (i = 0; i < JVX_SEQUENCER_NUMBER_MODES; i++)
			{
				QFont myCustomFont = comboBox_run_mode->font();
				if (jvx_bitTest(run_mode, i))
				{
					myCustomFont.setBold(true);
				}
				else
				{
					myCustomFont.setBold(false);
				}

#ifdef JVX_OS_LINUX
				comboBox_run_mode->setEditable(true);
#endif
				comboBox_run_mode->setItemData(i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
				comboBox_run_mode->setEditable(false);
#endif
			}

			switch (this->currentSetupShown.newStep.elementTp)
			{
			case JVX_SEQUENCER_TYPE_COMMAND_NONE:
			case JVX_SEQUENCER_TYPE_WAIT_FOREVER:
				break;
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE:
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE:
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START:
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP:
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS:
			case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE:

				jvx_qt_listComponentsInComboBox_(true, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);

				break;

			case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE:
			case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START:
			case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP:
			case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS:

				lineEdit_process_descriptor->setEnabled(true);
				comboBox_process_step->setEnabled(true);
				jvx_qt_listComponentsInComboBox_(true, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);
				jvx_qt_listProcessesInComboBox_(true, comboBox_process_step, theHostRef,
					this->currentSetupShown.newStep.targetTp, currentSetupShown.newStep.labelTrue);
				lineEdit_process_descriptor->setText(currentSetupShown.newStep.labelTrue.c_str());
				//		listComponentsInComboBox(true);
				break;

			case JVX_SEQUENCER_TYPE_CONDITION_WAIT:

				jvx_qt_listComponentsInComboBox_(false, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);
				//listComponentsInComboBox(false);
				listConditionsInComboBox(this->currentSetupShown.newStep.targetTp, correctedSetting);

				break;
			case JVX_SEQUENCER_TYPE_CONDITION_JUMP:

				jvx_qt_listComponentsInComboBox_(false, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);
				//listComponentsInComboBox(false);
				label_command_condition_arg->setText("Condition Id");
				listConditionsInComboBox(this->currentSetupShown.newStep.targetTp, correctedSetting);
				listAllLabelsInSequence(this->comboBox_labeltrue_p1_step, lineEdit_labeltrue_p1_step, this->currentSetupShown.newStep.labelTrue, label_labeltrue_p1, "Jump Target True");
				listAllLabelsInSequence(this->comboBox_labelfalse_p2_step, lineEdit_labelfalse_p2_step, this->currentSetupShown.newStep.labelFalse, label_labelfalse_p2, "Jump Target False");
				break;
			case JVX_SEQUENCER_TYPE_JUMP:

				listAllLabelsInSequence(this->comboBox_labeltrue_p1_step, lineEdit_labeltrue_p1_step, this->currentSetupShown.newStep.labelTrue, label_labeltrue_p1, "Jump Target");
				break;

			case JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC:

				jvx_qt_listComponentsInComboBox_(false, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);
				//listComponentsInComboBox(false);
				label_command_condition_arg->setText("Command Id");
				listCommandsInComboBox(this->currentSetupShown.newStep.targetTp, correctedSetting);
				break;

			case JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP:

				jvx_qt_listComponentsInComboBox_(false, comboBox_component_step, theHostRef, this->currentSetupShown.newStep.targetTp);
				//listComponentsInComboBox(false);
				listRelativeJumpsInComboBox(this->currentSetupShown.newStep.targetTp);
				break;

			case JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT:
				lineEdit_labeltrue_p1_step->setText(this->currentSetupShown.newStep.labelTrue.c_str());
				lineEdit_labeltrue_p1_step->show();
				comboBox_labeltrue_p1_step->hide();
				label_labeltrue_p1->setText("Message");
				break;
			case JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY:
				lineEdit_labeltrue_p1_step->setText(this->currentSetupShown.newStep.labelTrue.c_str());
				lineEdit_labeltrue_p1_step->show();
				comboBox_labeltrue_p1_step->hide();
				label_labeltrue_p1->setText("Property");
				break;
			case JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR:
			case JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR:
				lineEdit_labeltrue_p1_step->setText(this->currentSetupShown.newStep.labelTrue.c_str());
				lineEdit_labeltrue_p1_step->show();
				comboBox_labeltrue_p1_step->hide();
				label_labeltrue_p1->setText("Error");
				break;
			case JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE:
				listAllLabelsSequence(this->comboBox_labeltrue_p1_step, lineEdit_labeltrue_p1_step, this->currentSetupShown.newStep.labelTrue, label_labeltrue_p1, "Call Target");
				break;
			case JVX_SEQUENCER_TYPE_CALLBACK:
				label_command_condition_arg->setText("Callback Id");
				comboBox_commandid_step->hide();
				lineEdit_argstep->show();
				lineEdit_argstep->setText(jvx_size2String(currentSetupShown.newStep.fId).c_str());
				break;

			case JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE:
				
				label_command_condition_arg->setText("New State [Id]");
				comboBox_commandid_step->hide();
				lineEdit_argstep->show();
				if (JVX_CHECK_SIZE_STATE_INCREMENT(currentSetupShown.newStep.fId))
				{
					lineEdit_argstep->setText("++");
				}
				else if (JVX_CHECK_SIZE_STATE_DECREMENT(currentSetupShown.newStep.fId))
				{
					lineEdit_argstep->setText("--");
				}
				else
				{
					lineEdit_argstep->setText(jvx_size2String(currentSetupShown.newStep.fId).c_str());
				}
				break;

			case JVX_SEQUENCER_TYPE_COMMAND_LIMIT:
				assert(0);
				break;
			}
			
			viewTimeout(lineEdit_timeout_step);
		}

		if (jvx_bitTest(whattoupdate, JVX_UPDATE_STATUS_SEQUENCER))
		{
			// Set all columns to neutral
			this->updateWindow_highlight_reset(Qt::white);

			// Highlight the selection
			this->updateWindow_highlight_set(currentSetupShown.selection.sequenceId,
				currentSetupShown.selection.stepId, currentSetupShown.selection.queueSelection, Qt::gray);

			theSequencer->status_process(&stat, &idSeq, &tp, &idStep, &valB);
			txt = jvxSequencerStatus_txt(stat);
			this->lineEdit_process_status->setText(txt.c_str());
			if ((stat >= JVX_SEQUENCER_STATUS_NONE) && (stat < JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS))
			{
				if (JVX_CHECK_SIZE_SELECTED(idSeq) && JVX_CHECK_SIZE_SELECTED(idStep))
				{
					//tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
					this->updateWindow_highlight_set(idSeq, idStep, tp, Qt::yellow);
				}
			}
			else
			{
				if (JVX_CHECK_SIZE_SELECTED(idSeq) && JVX_CHECK_SIZE_SELECTED(idStep))
				{
					tp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;
					this->updateWindow_highlight_set(idSeq, idStep, tp, Qt::yellow);
				}
			}

			this->checkBox_loop->setChecked(valB);
			theSequencer->get_stepping_mode(&actStep, &actBreak);
			checkBox_break->setChecked(actBreak);
			checkBox_step->setChecked(actStep);
			checkBox_break_step->setChecked(currentSetupShown.newStep.break_active);
		}
	}
	if (correctedSetting)
	{
		activateApply_step = true;
	}
	if (activateApply_step)
	{
		pushButton_edit->setEnabled(true);
	}
	else
	{
		pushButton_edit->setEnabled(false);
	}
}

void
jvx_sequencer_widget::updateWindow_rebuild()
{
	jvxSize num = 0;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxApiString fldStr;
	jvxApiString fldStrLab;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i =0, j = 0;
	jvxBool markedForProcess = false;
	jvxApiString fld_label_name;
	jvxApiString fld_label_cond_true;
	jvxApiString fld_label_cond_false;
	int idStep = 0;
	jvxSize labelId = 0;
	jvxSequencerElementType elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification compTp;
	jvxSize funcId = 0;
	jvxInt64 timeout = -1;
	jvxBool isDefault = false;
	jvxBool b_active = false;
	jvxCBitField assoc_mode = 0;

	treeWidget->clear();
	num = 0;
	res = theSequencer->number_sequences(&num);
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < num; i++)
		{
			numStepsRun = 0;
			numStepsLeave = 0;
			res = theSequencer->description_sequence(i, &fldStr, &fldStrLab, &numStepsRun, &numStepsLeave, &markedForProcess, &isDefault);
			if (res == JVX_NO_ERROR)
			{
				jvxSize idSeq = i;

				std::string descSequence = fldStr.std_str();
				std::string labSequence = fldStrLab.std_str();

				QTreeWidgetItem* topL = new QTreeWidgetItem;
				if (isDefault)
				{
					topL->setText(0, ("Sequence id#" + jvx_size2String(i) + "<D>").c_str());
				}
				else
				{
					topL->setText(0, ("Sequence id#" + jvx_size2String(i)).c_str());
				}
				topL->setText(1, descSequence.c_str());
				topL->setText(2, labSequence.c_str());
				QFont ft = topL->font(0);
				ft.setBold(true);
				topL->setFont(0, ft);
				topL->setFont(1, ft);
				topL->setFont(2, ft);
				if (markedForProcess)
				{
					QBrush br(Qt::green);
					topL->setForeground(0, br);
				}
				else
				{
					QBrush br(Qt::gray);
					topL->setForeground(0, br);
				}
				topL->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				topL->setData(0, Qt::UserRole + 1, QVariant(-1));
				topL->setData(0, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_NONE));
				topL->setData(1, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				topL->setData(1, Qt::UserRole + 1, QVariant(-1));
				topL->setData(1, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_NONE));
				topL->setData(2, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				topL->setData(2, Qt::UserRole + 1, QVariant(-1));
				topL->setData(2, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_NONE));


				QTreeWidgetItem* seqRun = new QTreeWidgetItem(topL);
				seqRun->setText(0, "Entries mode \"Run\"");
				seqRun->setText(1, "---");
				seqRun->setText(2, "---");
				seqRun->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqRun->setData(0, Qt::UserRole + 1, QVariant(-1));
				seqRun->setData(0, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));
				seqRun->setData(1, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqRun->setData(1, Qt::UserRole + 1, QVariant(-1));
				seqRun->setData(1, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));
				seqRun->setData(2, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqRun->setData(2, Qt::UserRole + 1, QVariant(-1));
				seqRun->setData(2, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));

				QTreeWidgetItem* seqLeave = new QTreeWidgetItem(topL);
				seqLeave->setText(0, "Entries mode \"Leave\"");
				seqLeave->setText(1, "---");
				seqLeave->setText(2, "---");
				seqLeave->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqLeave->setData(0, Qt::UserRole + 1, QVariant(-1));
				seqLeave->setData(0, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));
				seqLeave->setData(1, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqLeave->setData(1, Qt::UserRole + 1, QVariant(-1));
				seqLeave->setData(1, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));
				seqLeave->setData(2, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
				seqLeave->setData(2, Qt::UserRole + 1, QVariant(-1));
				seqLeave->setData(2, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));


				for (j = 0; j < numStepsRun; j++)
				{
					idStep = (int)j;
					labelId = 0;
					elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
					compTp = JVX_COMPONENT_UNKNOWN;
					funcId = 0;
					timeout = -1;
					res = theSequencer->description_step_sequence(i, j, JVX_SEQUENCER_QUEUE_TYPE_RUN, &fldStr, &elmTp, 
						&compTp, &funcId, &timeout, &fld_label_name, &fld_label_cond_true, &fld_label_cond_false, &b_active, 
						&assoc_mode);

					if (res == JVX_NO_ERROR)
					{
						std::string descStep = fldStr.std_str();
						std::string label_name = fld_label_name.std_str();
						std::string label_cond_true = fld_label_cond_true.std_str();
						std::string label_cond_false = fld_label_cond_false.std_str();

						QString tt = (produceToolTip(elmTp, compTp, funcId, timeout, label_cond_true, label_cond_false)).c_str();

						QTreeWidgetItem* oneStep = new QTreeWidgetItem(seqRun);
						oneStep->setText(0, ("Step" + jvx_int2String((int)j)).c_str());
						oneStep->setText(1, descStep.c_str());
						if (label_name.empty())
						{
							oneStep->setText(2, "-- no label --");
						}
						else
						{
							oneStep->setText(2, label_name.c_str());
						}
						oneStep->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(0, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(0, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));
						oneStep->setData(1, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(1, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(1, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));
						oneStep->setData(2, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(2, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(2, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_RUN));
						oneStep->setToolTip(0, tt);
						oneStep->setToolTip(1, tt);
						oneStep->setToolTip(2, tt);
						seqRun->addChild(oneStep);
					}
				}

				for (j = 0; j < numStepsLeave; j++)
				{
					idStep = (int)j;
					labelId = 0;
					elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
					compTp = JVX_COMPONENT_UNKNOWN;
					funcId = 0;
					timeout = -1;
					res = theSequencer->description_step_sequence(i, j, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, &fldStr, 
						&elmTp, &compTp, &funcId, &timeout, &fld_label_name, &fld_label_cond_true, &fld_label_cond_false,
						&b_active, &assoc_mode);

					if (res == JVX_NO_ERROR)
					{
						std::string descStep = fldStr.std_str();
						std::string label_name = fld_label_name.std_str();
						std::string label_cond_true = fld_label_cond_true.std_str();
						std::string label_cond_false = fld_label_cond_false.std_str();

						QString tt = (produceToolTip(elmTp, compTp, funcId, timeout, label_cond_true, label_cond_false)).c_str();

						QTreeWidgetItem* oneStep = new QTreeWidgetItem(seqLeave);
						oneStep->setText(0, ("Step" + jvx_int2String((int)j)).c_str());
						oneStep->setText(1, descStep.c_str());
						if (label_name.empty())
						{
							oneStep->setText(2, "-- no label --");
						}
						else
						{
							oneStep->setText(2, label_name.c_str());
						}
						oneStep->setData(0, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(0, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(0, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));
						oneStep->setData(1, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(1, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(1, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));
						oneStep->setData(2, Qt::UserRole, QVariant(JVX_SIZE_INT(idSeq)));
						oneStep->setData(2, Qt::UserRole + 1, QVariant(JVX_SIZE_INT(idStep)));
						oneStep->setData(2, Qt::UserRole + 2, QVariant(JVX_SEQUENCER_QUEUE_TYPE_LEAVE));
						oneStep->setToolTip(0, tt);
						oneStep->setToolTip(1, tt);
						oneStep->setToolTip(2, tt);

						seqRun->addChild(oneStep);
					}
				}
				//topL->addChild(seqRun);
				//topL->addChild(seqLeave);

				treeWidget->addTopLevelItem(topL);
			}//if((res == JVX_NO_ERROR) && (fldStr))


		}// for(i = 0; i < num; i++)
	}//if(res == JVX_NO_ERROR)

	treeWidget->expandAll();
	jvxBitField btf;
	jvx_bitFull(btf);
	this->updateWindow(btf);
}

void
jvx_sequencer_widget::updateWindow_highlight_set(jvxSize& sequenceId, jvxSize& stepId, jvxSequencerQueueType& tp, QColor col)
{
	QTreeWidgetItem* currentItem = NULL;
	int numEntries = treeWidget->topLevelItemCount();
	if(sequenceId < (jvxSize)numEntries)
	{
		currentItem = treeWidget->topLevelItem((int)sequenceId);
		numEntries = currentItem->childCount();
		assert(numEntries == 2);
		if(tp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
		{
			currentItem = currentItem->child(0);
		}
		else
		{
			currentItem = currentItem->child(1);
		}
		if(JVX_CHECK_SIZE_SELECTED(stepId))
		{
			numEntries = currentItem->childCount();
			if(stepId < (jvxSize)numEntries)
			{
				currentItem = currentItem->child((int)stepId);
			}
		}
	}
	if(currentItem)
	{
		this->treeWidget->setCurrentItem(currentItem);
		currentItem->setBackground(0, QBrush(col));
		currentItem->setBackground(1, QBrush(col));
		currentItem->setBackground(2, QBrush(col));
	}
	else
	{
		sequenceId = JVX_SIZE_UNSELECTED;
		stepId = JVX_SIZE_UNSELECTED;
		tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	}
}

void
jvx_sequencer_widget::updateWindow_highlight_reset(QColor col)
{
	int i,j,k;
	QTreeWidgetItem* currentItem1 = NULL;
	QTreeWidgetItem* currentItem2 = NULL;
	QTreeWidgetItem* currentItem3 = NULL;

	int numEntries1, numEntries2, numEntries3;

	numEntries1 = treeWidget->topLevelItemCount();

	for(i = 0; i < numEntries1; i++)
	{
		currentItem1 = treeWidget->topLevelItem(i);
		currentItem1->setBackground(0, QBrush(col));
		currentItem1->setBackground(1, QBrush(col));
		currentItem1->setBackground(2, QBrush(col));
		numEntries2 = currentItem1->childCount();
		for(j = 0; j < numEntries2; j++)
		{
			currentItem2= currentItem1->child(j);

			currentItem2->setBackground(0, QBrush(col));
			currentItem2->setBackground(1, QBrush(col));
			currentItem2->setBackground(2, QBrush(col));
			numEntries3 = currentItem2->childCount();

			for(k = 0; k < numEntries3; k++)
			{
				currentItem3 = currentItem2->child(k);

				currentItem3->setBackground(0, QBrush(col));
				currentItem3->setBackground(1, QBrush(col));
				currentItem3->setBackground(2, QBrush(col));
			}
		}
	}
}

void
jvx_sequencer_widget::getReferenceComponentType(jvxComponentIdentification tp, IjvxObject*& theObj, bool isStandardCommand)
{
	jvxErrorType res = JVX_NO_ERROR;
	theObj = NULL;
	IjvxToolsHost* theToolsHost = NULL;
	switch(tp.tp)
	{
#include "codeFragments/components/Hjvx_caseStatement_maincomponents.h"

		res = theHostRef->request_object_selected_component(tp, &theObj);
		break;
#include "codeFragments/components/Hjvx_caseStatement_othercomponents.h"
		if (tp.tp == JVX_COMPONENT_DATALOGGER)
		{
			if (!isStandardCommand)
			{
				res = theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
				if ((res == JVX_NO_ERROR) && theToolsHost)
				{
					theToolsHost->reference_tool(JVX_COMPONENT_DATALOGGER, &theObj, 0, NULL);
					theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
				}
			}
		}
		break;
	default:
		assert(0);
	}
}

void
jvx_sequencer_widget::returnReferenceComponentType(jvxComponentIdentification tp, IjvxObject* theObj, bool isStandardCommand)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxToolsHost* theToolsHost = NULL;

	switch(tp.tp)
	{
#include "codeFragments/components/Hjvx_caseStatement_maincomponents.h"

		res = theHostRef->return_object_selected_component(tp, theObj);
		break;
#include "codeFragments/components/Hjvx_caseStatement_othercomponents.h"
		if (tp.tp == JVX_COMPONENT_DATALOGGER)
		{
			if (!isStandardCommand)
			{
				res = theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
				if ((res == JVX_NO_ERROR) && theToolsHost)
				{
					theToolsHost->return_reference_tool(JVX_COMPONENT_DATALOGGER, theObj);
					theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
				}
			}
		}
		break;
	default:
		assert(0);
	}
}

/*
void
jvx_sequencer_widget::listComponentsInComboBox(bool isStandardCommand)
{
	jvxSize i;
	IjvxObject* theObj = NULL;

	for(i = 0; i < JVX_COMPONENT_LIMIT; i++)
	{
		this->comboBox_component_step->addItem(jvxComponentType_txt(i).c_str());
	}

	for(i = 0; i < JVX_COMPONENT_LIMIT; i++)
	{
		QFont myCustomFont;

		// Ok, we know that this component has been selected
		theObj = NULL;
		this->getReferenceComponentType((jvxComponentType)i, theObj, isStandardCommand);

		if(theObj)
		{
			myCustomFont.setBold(true);
			myCustomFont.setItalic(false);
			this->returnReferenceComponentType((jvxComponentType)i, theObj, isStandardCommand);
		}
		else
		{
			myCustomFont.setBold(false);
			myCustomFont.setItalic(true);
		}
#ifdef JVX_OS_LINUX
                this->comboBox_component_step->setEditable(true);
#endif
		this->comboBox_component_step->setItemData(i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
                this->comboBox_component_step->setEditable(false);
#endif
	}
	this->comboBox_component_step->setCurrentIndex(this->currentSetupShown.newStep.targetTp);
}
*/

void
jvx_sequencer_widget::listConditionsInComboBox(jvxComponentIdentification tp, jvxBool& correctedSettings)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObj = NULL;
	IjvxSequencerControl* theControl = NULL;
	jvxHandle* theHdl = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldStr;
	std::string txt;
	jvxSize uniqueId = 0;

	this->comboBox_commandid_step->clear();
	getReferenceComponentType(tp, theObj, false);
	if(theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				jvxSize selectCurrent = JVX_SIZE_UNSELECTED;
				jvxSize selectCurrent_ifnone = JVX_SIZE_UNSELECTED;
				theControl->number_conditions(&num);
				for(i = 0; i < num; i++)
				{
					txt = "Unknown";
					theControl->description_condition(i, &uniqueId, &fldStr, NULL);
					if (JVX_CHECK_SIZE_UNSELECTED(selectCurrent_ifnone))
					{
						selectCurrent_ifnone = uniqueId;
					}
					txt = fldStr.std_str();
					if (uniqueId == currentSetupShown.newStep.fId)
					{
						selectCurrent = i;
					}
					this->comboBox_commandid_step->addItem(txt.c_str());
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant(0), Qt::UserRole);
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant((int)uniqueId), Qt::UserRole+1);
				}
				if (JVX_CHECK_SIZE_UNSELECTED(selectCurrent))
				{
					selectCurrent = selectCurrent_ifnone;
					currentSetupShown.newStep.fId = selectCurrent;
					correctedSettings = true;
				}

				QFont ft = this->comboBox_commandid_step->font();
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(true);
#endif
				for (i = 0; i < comboBox_commandid_step->count(); i++)
				{
					if (i == selectCurrent)
					{
						ft.setBold(true);
					}
					else
					{
						ft.setBold(false);
					}
					comboBox_commandid_step->setItemData((int)i, QVariant(ft), Qt::FontRole);
				}
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(false);
#endif
				if (selectCurrent < comboBox_commandid_step->count())
				{
					comboBox_commandid_step->setCurrentIndex(selectCurrent);
				}

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
				theControl = NULL;
			}
			theHdl = NULL;
		}
		returnReferenceComponentType(tp, theObj, false);
	}
}

void
jvx_sequencer_widget::listCommandsInComboBox(jvxComponentIdentification tp, jvxBool& correctedSettings)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObj = NULL;
	IjvxSequencerControl* theControl = NULL;
	jvxHandle* theHdl = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldStr;
	std::string txt;
	jvxSize uniqueId = 0;

	this->comboBox_commandid_step->clear();
	getReferenceComponentType(tp, theObj, true);
	if(theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				jvxSize selectCurrent = JVX_SIZE_UNSELECTED;
				jvxSize selectCurrent_ifnone = JVX_SIZE_UNSELECTED;

				theControl->number_commands(&num);
				for(i = 0; i < num; i++)
				{
					theControl->description_command(i, &uniqueId, &fldStr, NULL);
					if (JVX_CHECK_SIZE_UNSELECTED(selectCurrent_ifnone))
					{
						selectCurrent_ifnone = uniqueId;
					}
					txt = "Unknown";
					if(uniqueId == currentSetupShown.newStep.fId)
					{
						selectCurrent = (int)i;
					}
					txt = fldStr.std_str();
					this->comboBox_commandid_step->addItem(txt.c_str());
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant(1), Qt::UserRole);
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant((int)uniqueId), Qt::UserRole+1);
				}
				if (JVX_CHECK_SIZE_UNSELECTED(selectCurrent))
				{
					selectCurrent = selectCurrent_ifnone;
					currentSetupShown.newStep.fId = selectCurrent;
					correctedSettings = true;
				}

				QFont ft = this->comboBox_commandid_step->font();
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(true);
#endif
				for (i = 0; i < comboBox_commandid_step->count(); i++)
				{
					if (i == selectCurrent)
					{
						ft.setBold(true);
					}
					else
					{
						ft.setBold(false);
					}
					comboBox_commandid_step->setItemData((int)i, QVariant(ft), Qt::FontRole);
				}
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(false);
#endif
				if (selectCurrent < comboBox_commandid_step->count())
				{
					comboBox_commandid_step->setCurrentIndex(selectCurrent);
				}
				/*
				if(JVX_CHECK_SIZE_SELECTED(selectCurrent))
				{
					this->comboBox_commandid_step->setCurrentIndex(JVX_SIZE_INT(selectCurrent));
					ft.setBold(true);
				}
				else
				{
					std::cout << "Warning: No condition selected!" << std::endl;
					ft.setBold(false);
				}
				this->comboBox_commandid_step->setFont(ft);
				*/

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
				theControl = NULL;
			}
			theHdl = NULL;
		}
		returnReferenceComponentType(tp, theObj, false);
	}
}

void
jvx_sequencer_widget::listRelativeJumpsInComboBox(jvxComponentIdentification tp)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObj = NULL;
	IjvxSequencerControl* theControl = NULL;
	jvxHandle* theHdl = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldStr;
	std::string txt;
	jvxSize uniqueId = 0;

	this->comboBox_commandid_step->clear();
	getReferenceComponentType(tp, theObj, true);
	if(theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				jvxSize selectCurrent = JVX_SIZE_UNSELECTED;

				theControl->number_relative_jumps(&num);
				for(i = 0; i < num; i++)
				{
					theControl->description_relative_jump(i, &uniqueId, &fldStr, NULL);
					txt = "Unknown";
					if(uniqueId == currentSetupShown.newStep.fId)
					{
						selectCurrent = (int)i;
					}
					txt = fldStr.std_str();
					this->comboBox_commandid_step->addItem(txt.c_str());
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant(1), Qt::UserRole);
					comboBox_commandid_step->setItemData(comboBox_commandid_step->count()-1, QVariant((int)uniqueId), Qt::UserRole+1);
				}
				
				QFont ft = this->comboBox_commandid_step->font();
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(true);
#endif
				for (i = 0; i < comboBox_commandid_step->count(); i++)
				{
					if (i == selectCurrent)
					{
						ft.setBold(true);
					}
					else
					{
						ft.setBold(false);
					}
					comboBox_commandid_step->setItemData((int)i, QVariant(ft), Qt::FontRole);
				}
#ifdef JVX_OS_LINUX
				comboBox_commandid_step->setEditable(false);
#endif

				if (selectCurrent < comboBox_commandid_step->count())
				{
					comboBox_commandid_step->setCurrentIndex(selectCurrent);
				}

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
				theControl = NULL;
			}
			theHdl = NULL;
		}
		returnReferenceComponentType(tp, theObj, false);
	}
}


void
jvx_sequencer_widget::listAllLabelsInSequence(QComboBox* theBox, QLineEdit* ledit, std::string& txtShow, QLabel* lab, const std::string& labTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldLabel;
	std::string txt;
	jvxSize selectionIdx = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;

	theBox->clear();
	theBox->show();
	ledit->hide();
	lab->setText(labTxt.c_str());
	if(JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
		res = theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &num, NULL, NULL);
		if (res == JVX_NO_ERROR)
		{

			for (i = 0; i < num; i++)
			{
				res = theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId, i,
					JVX_SEQUENCER_QUEUE_TYPE_RUN, NULL, NULL, NULL, NULL, NULL, &fldLabel, NULL, NULL, NULL, NULL);
				if (res == JVX_NO_ERROR)
				{
					txt = fldLabel.std_str();
				}
				if (!txt.empty())
				{
					if (txt == txtShow)
					{
						selectionIdx = (jvxInt32)cnt;
					}
					theBox->addItem(txt.c_str());
					theBox->setItemData(theBox->count() - 1, QVariant((int)i), Qt::UserRole);
					cnt++;
				}

			}
			if (JVX_CHECK_SIZE_SELECTED(selectionIdx))
			{
				theBox->setCurrentIndex(JVX_SIZE_INT(selectionIdx));
			}
		}
	}
}

void
jvx_sequencer_widget::listAllLabelsSequence(QComboBox* theBox, QLineEdit* ledit, std::string& txtShow, QLabel* lab, const std::string& labTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	jvxSize i;
	jvxApiString fldLabel;
	std::string txt;
	jvxSize selectionIdx = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	jvxApiString labelSeq;
	theBox->clear();
	theBox->show();
	ledit->hide();
	lab->setText(labTxt.c_str());
	theSequencer->number_sequences(&num);

	for (i = 0; i < num; i++)
	{
		res = theSequencer->description_sequence(i, NULL, &fldLabel, NULL, NULL, NULL);
		if (res == JVX_NO_ERROR)
		{
			txt = fldLabel.std_str();
		}
		if (!txt.empty())
		{
			if (txt == txtShow)
			{
				selectionIdx = (jvxInt32)cnt;
			}
			theBox->addItem(txt.c_str());
			theBox->setItemData(theBox->count() - 1, QVariant((int)i), Qt::UserRole);
			cnt++;
		}
		if (JVX_CHECK_SIZE_SELECTED(selectionIdx))
		{
			theBox->setCurrentIndex(JVX_SIZE_INT(selectionIdx));
		}
	}
}

void
jvx_sequencer_widget::viewTimeout(QLineEdit* line)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	std::string txt;
	jvxSize selectionIdx = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	jvxInt64 timeoutms = 0;
	line->setText("--");

	if(JVX_CHECK_SIZE_SELECTED(currentSetupShown.selection.sequenceId))
	{
			switch(currentSetupShown.selection.queueSelection)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				res = theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, &num, NULL, NULL);
				if(res == JVX_NO_ERROR)
				{
					if(currentSetupShown.selection.stepId < num)
					{
						res = theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId, currentSetupShown.selection.stepId,
							JVX_SEQUENCER_QUEUE_TYPE_RUN, NULL, NULL, NULL, NULL, &timeoutms, NULL, NULL, NULL, NULL, NULL);
						if(res == JVX_NO_ERROR)
						{
							line->setText((jvx_int2String(timeoutms)).c_str());
						}
						else
						{
							assert(0);
						}
					}
				}
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				res = theSequencer->description_sequence(currentSetupShown.selection.sequenceId, NULL, NULL, NULL, &num, NULL);
				if(res == JVX_NO_ERROR)
				{
					if(currentSetupShown.selection.stepId < num)
					{
						res = theSequencer->description_step_sequence(currentSetupShown.selection.sequenceId, currentSetupShown.selection.stepId,
							JVX_SEQUENCER_QUEUE_TYPE_LEAVE, NULL, NULL, NULL, NULL, &timeoutms, NULL, NULL, NULL, NULL, NULL);
						if(res == JVX_NO_ERROR)
						{
							line->setText((jvx_int2String(timeoutms)).c_str());
						}
						else
						{
							assert(0);
						}
					}
				}
				break;

		}
	}
}

