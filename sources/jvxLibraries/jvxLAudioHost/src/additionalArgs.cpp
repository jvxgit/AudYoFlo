#include "additionalArgs.h"
#include "uMainWindow.h"
#include "jvxQtAudioHost_common.h"
#include "jvxAudioHost_common.h"
// From genericWrapper tech

#define GET_FILE_SELECT_REF_PROPS(num, id, token, cg) \
	{ \
		jvxSelectionList selList; \
		jPAD ident(token); \
		jPD trans(true); \
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(cg, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), \
			ident, trans); \
		if(res == JVX_NO_ERROR) \
		{ \
			num = selList.strList.ll(); \
			id = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), num); \
		} \
		else \
		{ \
			num = 0; \
			id = JVX_SIZE_UNSELECTED; \
		} \
	}

additionalArgsWidget::additionalArgsWidget(uMainWindow* parent, jvxComponentIdentification*tpAllI): QDialog(parent), myParent(parent),
	tpAll(tpAllI)
	
{
	config.min_width = 640;
	config.min_height = 480;
}

void
additionalArgsWidget::init()
{
	jvxSize i;
	this->setupUi(this);

	for(i = 0; i < NUM_LOOKAHEAD_BUFFERS; i++)
	{
		this->comboBox_ifilelahead->addItem(jvx_int2String(tableLookaheadBuffers[i]).c_str());
		this->comboBox_outfilelahead->addItem(jvx_int2String(tableLookaheadBuffers[i]).c_str());
	}
}

QSize
additionalArgsWidget::sizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

QSize
additionalArgsWidget::minimumSizeHint() const
{
	return QSize(config.min_width, config.min_height);
}

void
additionalArgsWidget::refreshPropertyIds(jvxBool tecDevIsGenWrapper)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSize hdlIdx;
	jvxBool foundit = false;
	std::string str;
	jvxSize idx = 0;
	theHost = myParent->involvedHost.hHost;

	if(theHost)
	{
		theHost->state(&stat);
		if(stat == JVX_STATE_ACTIVE)
		{
			theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &stat);
			if(stat == JVX_STATE_ACTIVE)
			{
				if (tecDevIsGenWrapper)
				{
				}
			}
		}
	}
}

void
additionalArgsWidget::refreshPropertyIds_run(jvxBool tecDevIsGenWrapper)
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num;
	jvxSize hdlIdx;
	jvxBool foundit = false;
	std::string str;
	jvxSize idx = 0;
	theHost = myParent->involvedHost.hHost;

	if(theHost)
	{
		theHost->state(&stat);
		if(stat == JVX_STATE_ACTIVE)
		{
			theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &stat);
			if(stat == JVX_STATE_ACTIVE)
			{
				if (tecDevIsGenWrapper)
				{
					// Device

					theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
					if (stat > JVX_STATE_ACTIVE)
					{

					}
				}
			}
		}
	}
}

void
additionalArgsWidget::updateWindow()
{
	jvxSize i;
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
    jvxValueInRange valRange;
	jvxBool foundit = false;
	std::string str;
	jvxInt32 valI32;
	jvxInt16 valI16;
	jvxSize valSz = 0;
	jvxApiStringList fldStrList;
	jvxApiString  fldStr;
	jvxSize idx = 0;
	jvxCBool valB = c_false;
	std::string token;
	jvxData valD = 0;
	jvxSelectionList sel;
	jvxCallManagerProperties callGate;

	theHost = myParent->involvedHost.hHost;
	comboBox_formathw->clear();

	theHost->state(&stat);

	this->checkBox_infile_boost->setEnabled(false);
	this->checkBox_infileloop->setEnabled(false);
	this->checkBox_pauseonstart->setEnabled(false);
	this->checkBox_infile_boost->setChecked(false);
	this->checkBox_infileloop->setChecked(false);
	this->checkBox_pauseonstart->setChecked(false);
	this->comboBox_ifilelahead->setEnabled(false);
	lineEdit_infilemode->setText("--");

	this->checkBox_outfile_boost->setEnabled(false);
	this->checkBox_outfile_boost->setChecked(false);
	this->comboBox_outfilelahead->setEnabled(false);
	this->lineEdit_outchannels->setEnabled(false);
	this->lineEdit_outfile_rate->setEnabled(false);
	comboBox_outmode->setEnabled(false);
	comboBox_outmode->clear();

	this->pushButton_inc_outchannels->setEnabled(false);
	this->pushButton_dec_outchannels->setEnabled(false);

	this->comboBox_bsizes->clear();
	this->comboBox_srates->clear();
	this->comboBox_resampler_in->clear();
	this->comboBox_resampler_out->clear();

	this->pushButton_bwd->setEnabled(false);
	this->pushButton_fwd->setEnabled(false);
	this->pushButton_play->setEnabled(false);
	this->pushButton_pause->setEnabled(false);
	this->pushButton_restart->setEnabled(false);
	checkBox_stopsilence->setEnabled(false);
	lineEdit_stopsilence->setEnabled(false);
	lineEdit_stopsilence->setText("*unsupported*");

	checkBox_audioparams->setEnabled(false);

	this->pushButton_asio_control_open->setEnabled(false);
	this->comboBox_alsa_acc_tp->setEnabled(false);
	this->comboBox_alsa_datatp->setEnabled(false);
	this->lineEdit_alsa_periodsi->setEnabled(false);
	this->lineEdit_alsa_periodso->setEnabled(false);
	this->checkBox_alsa_allchannelsi->setEnabled(false);
	this->checkBox_alsa_allchannelso->setEnabled(false);

	comboBox_srates->clear();
	lineEdit_srates->setText("--");
	stackedWidget_rate_st->setCurrentIndex(1);

    comboBox_bsizes->clear();
    lineEdit_bsize->setText("--");
	stackedWidget_bsize_st->setCurrentIndex(1);

	if(stat == JVX_STATE_ACTIVE)
	{
		theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &stat);
		if(stat == JVX_STATE_ACTIVE)
		{
			jvxApiStringList fNames;

			if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
			{
				comboBox_srates->setEnabled(true);
				lineEdit_srates->setEnabled(true);
				comboBox_bsizes->setEnabled(true);
				lineEdit_bsize->setEnabled(true);
				comboBox_formathw->setEnabled(true);
				checkBox_audioparams->setEnabled(true);
				comboBox_resampler_in->setEnabled(true);
				comboBox_resampler_out->setEnabled(true);
				pushButton_asio_control_open->setEnabled(true);
			}
			if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT))
			{

				jvxSize num_input_files;
				jvxSize id_infile;

				GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);
				if (JVX_CHECK_SIZE_SELECTED(id_infile))
				{
					this->checkBox_infile_boost->setEnabled(true);
					this->checkBox_infileloop->setEnabled(true);
					this->checkBox_pauseonstart->setEnabled(true);
					this->comboBox_ifilelahead->setEnabled(true);

					ident.reset("/properties_selected_input_file/lookahead");
					trans.reset(true, id_infile);

					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
					assert(res == JVX_NO_ERROR);
					assert(valI16 < comboBox_ifilelahead->count());
					this->comboBox_ifilelahead->setCurrentIndex(valI16);

					ident.reset("/properties_selected_input_file/boost_prio");
					trans.reset(true, id_infile);

					valB = c_false;
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->checkBox_infile_boost->setChecked(valB == c_true);

					ident.reset("/properties_selected_input_file/pause_on_start");
					trans.reset(true, id_infile);

					valB = c_false;
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->checkBox_pauseonstart->setChecked(valB == c_true);

					ident.reset("/properties_selected_input_file/loop");
					trans.reset(true, id_infile);

					valB = c_false;
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->checkBox_infileloop->setChecked(valB == c_true);

					ident.reset("/properties_selected_input_file/mode");
					trans.reset(true, id_infile);

					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING), ident, trans);
					assert(res == JVX_NO_ERROR);

					lineEdit_infilemode->setText(fldStr.c_str());

				}

				jvxSize num_output_files;
				jvxSize id_outfile;
				GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);
				if (JVX_CHECK_SIZE_SELECTED(id_outfile))
				{
					this->checkBox_outfile_boost->setEnabled(true);
					this->comboBox_outfilelahead->setEnabled(true);
					this->lineEdit_outchannels->setEnabled(true);
					this->pushButton_inc_outchannels->setEnabled(true);
					this->pushButton_dec_outchannels->setEnabled(true);
					this->lineEdit_outfile_rate->setEnabled(true);
					comboBox_outmode->setEnabled(true);

					ident.reset("/properties_selected_output_file/samplerate");
					trans.reset(true, id_outfile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->lineEdit_outfile_rate->setText(jvx_size2String(valSz).c_str());

					ident.reset("/properties_selected_output_file/num_channels");
					trans.reset(true, id_outfile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->lineEdit_outchannels->setText(jvx_size2String(valSz).c_str());

					ident.reset("/properties_selected_output_file/lookahead");
					trans.reset(true, id_outfile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
					assert(res == JVX_NO_ERROR);
					assert(valI16 < comboBox_outfilelahead->count());
					this->comboBox_outfilelahead->setCurrentIndex(valI16);
					
					ident.reset("/properties_selected_output_file/boost_prio");
					trans.reset(true, id_outfile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
					assert(res == JVX_NO_ERROR);
					this->checkBox_outfile_boost->setChecked(valB == c_true);

					ident.reset("/properties_selected_output_file/mode");
					trans.reset(false, id_outfile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
					assert(res == JVX_NO_ERROR);
					comboBox_outmode->clear();
					for (i = 0; i < sel.strList.ll(); i++)
					{
						comboBox_outmode->addItem(sel.strList.std_str_at(i).c_str());
					}
					jvxSize selId = jvx_bitfieldSelection2Id(sel.bitFieldSelected(), sel.strList.ll());
					if (JVX_CHECK_SIZE_SELECTED(selId))
					{
						comboBox_outmode->setCurrentIndex((int)selId);
					}
				}
			}

			theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
			if(stat >= JVX_STATE_ACTIVE)
			{
				if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT))
				{
					this->pushButton_restart->setEnabled(false);
					this->checkBox_infileloop->setEnabled(true);
					this->checkBox_pauseonstart->setEnabled(true);
				}

				if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
				{
					// Rates
					if (myParent->subWidgets.theAudioDialog->currentPropsDev)
					{
						ident.reset("/JVX_GENW/system/sel_rate");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (jvx_check_property_access_ok(res,callGate.access_protocol))
						{
							stackedWidget_rate_st->setCurrentIndex(0);
							for (i = 0; i < selList.strList.ll(); i++)
							{
								comboBox_srates->addItem(selList.strList.std_str_at(i).c_str());
								if (jvx_bitTest(selList.bitFieldSelected(), i))
								{
									idx = i;
								}
							}

							this->comboBox_srates->setCurrentIndex(JVX_SIZE_INT(idx));
						}
						else
						{
							stackedWidget_rate_st->setCurrentIndex(1);
							ident.reset("/JVX_GENW/system/rate");
							trans.reset();
							res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE),
								ident, trans);
							if (jvx_check_property_access_ok(res,callGate.access_protocol))
							{
								lineEdit_srates->setText(jvx_int2String(valI32).c_str());
								lineEdit_srates->setEnabled(true);
							}
							else
							{
								// There is not device involved, e.g. since there is file io only
								lineEdit_srates->setText("--");
								lineEdit_srates->setEnabled(false);
							}
						}

						ident.reset("/JVX_GENW/system/sel_framesize");
						trans.reset( );
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (jvx_check_property_access_ok(res,callGate.access_protocol))
						{
							stackedWidget_bsize_st->setCurrentIndex(0);
							for (i = 0; i < selList.strList.ll(); i++)
							{
								comboBox_bsizes->addItem(selList.strList.std_str_at(i).c_str());
								if (jvx_bitTest(selList.bitFieldSelected(), i))
								{
									idx = i;
								}
							}

							this->comboBox_bsizes->setCurrentIndex(JVX_SIZE_INT(idx));
						}
						else
						{
							stackedWidget_bsize_st->setCurrentIndex(1);
							ident.reset("/JVX_GENW/system/framesize");
							trans.reset();
							res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE),
								ident, trans);
							if (jvx_check_property_access_ok(res,callGate.access_protocol))
							{
								lineEdit_bsize->setText(jvx_int2String(valI32).c_str());
								lineEdit_bsize->setEnabled(true);
							}
							else
							{
								// There is not device involved, e.g. since there is file io only
								lineEdit_bsize->setText("--");
								lineEdit_bsize->setEnabled(false);
							}
						}

						ident.reset("/JVX_GENW/system/sel_dataformat");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (jvx_check_property_access_ok(res,callGate.access_protocol))
						{
							for (i = 0; i < selList.strList.ll(); i++)
							{
								comboBox_formathw->addItem(selList.strList.std_str_at(i).c_str());
								if (jvx_bitTest(selList.bitFieldSelected(), i))
								{
									idx = i;
								}
							}

							this->comboBox_formathw->setCurrentIndex(JVX_SIZE_INT(idx));
							this->comboBox_formathw->setEnabled(true);
						}
						else
						{
							this->comboBox_formathw->clear();
							this->comboBox_formathw->addItem("*None*");
							this->comboBox_formathw->setEnabled(false);
						}

						ident.reset("/JVX_GENW_DEV_QRESAMPLER_IN");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_QRESAMPLER_IN", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							comboBox_resampler_in->setEnabled(true);
							for (i = 0; i < selList.strList.ll(); i++)
							{
								comboBox_resampler_in->addItem(selList.strList.std_str_at(i).c_str());
								if (jvx_bitTest(selList.bitFieldSelected(), i))
								{
									idx = i;
								}
							}

							this->comboBox_resampler_in->setCurrentIndex(JVX_SIZE_INT(idx));
						}
						else
						{
							comboBox_resampler_in->clear();
							comboBox_resampler_in->setEnabled(false);
						}

						ident.reset("/JVX_GENW_DEV_QRESAMPLER_OUT");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_QRESAMPLER_OUT", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							comboBox_resampler_out->setEnabled(true);
							for (i = 0; i < selList.strList.ll(); i++)
							{
								comboBox_resampler_out->addItem(selList.strList.std_str_at(i).c_str());
								if (jvx_bitTest(selList.bitFieldSelected(), i))
								{
									idx = i;
								}
							}

							this->comboBox_resampler_out->setCurrentIndex(JVX_SIZE_INT(idx));
						}
						else
						{
							comboBox_resampler_out->setEnabled(false);
							comboBox_resampler_out->clear();
						}

						// Rates
						ident.reset("/JVX_GENW_DEV_RESAMPLER");
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_RESAMPLER", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
							{
								// Resampler
								setBackgroundLabelColor(Qt::yellow, this->label_resampling);
							}
							else
							{
								setBackgroundLabelColor(Qt::black, this->label_resampling);
							}
						}
						else
						{
							setBackgroundLabelColor(Qt::gray, this->label_resampling);
						}

						// Rates
						ident.reset("/JVX_GENW_DEV_REBUFFER");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_REBUFFER", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
							{
								// Resampler
								setBackgroundLabelColor(Qt::yellow, this->label_rebuffer);
							}
							else
							{
								setBackgroundLabelColor(Qt::black, this->label_rebuffer);
							}
						}
						else
						{
							setBackgroundLabelColor(Qt::gray, this->label_rebuffer);
						}

						ident.reset("/JVX_GENW_DEV_AUTOHW");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_AUTOHW", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							checkBox_audioparams->setEnabled(true);
							if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
							{
								// Resampler
								checkBox_audioparams->setChecked(true);
							}
							else
							{
								checkBox_audioparams->setChecked(false);
							}
						}
						else
						{
							checkBox_audioparams->setChecked(false);
							checkBox_audioparams->setEnabled(false);
						}

						valB = c_false;
						ident.reset("/auto_stop");
						trans.reset();
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRIO<jvxCBool>(valB),
							ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/auto_stop", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							checkBox_autostop->setEnabled(true);
							checkBox_autostop->setChecked(valB == c_true);
						}
						else
						{
							checkBox_autostop->setEnabled(false);
							checkBox_autostop->setChecked(false);
						}

						valB = c_false;
						token = "/JVX_GENW/system/act_fillup_silence_stop";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							checkBox_stopsilence->setChecked(valB == c_true);
							checkBox_stopsilence->setEnabled(true);
						}
						else
						{
							checkBox_stopsilence->setChecked(false);
							checkBox_stopsilence->setEnabled(false);
						}

						valD = 0;
						token = "/JVX_GENW/system/per_fillup_silence_stop";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							lineEdit_stopsilence->setText(jvx_data2String(valD, 1).c_str());
							lineEdit_stopsilence->setEnabled(true);
						}
						else
						{
							lineEdit_stopsilence->setText("--");
							lineEdit_stopsilence->setEnabled(false);
						}

						if (stat == JVX_STATE_ACTIVE)
						{
							this->progressBar_infile->setValue(0);
							this->progressBar_load->setValue(0);
						}

#ifdef JVX_OS_LINUX
						token = "/JVX_GENW/activateAllChannelsInput";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRbIO(valB), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							checkBox_alsa_allchannelsi->setChecked(valB == c_true);
							checkBox_alsa_allchannelsi->setEnabled(true);
						}
						else
						{
							checkBox_alsa_allchannelsi->setEnabled(false);
						}

						token = "/JVX_GENW/activateAllChannelsOutput";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRBIO(valB), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							checkBox_alsa_allchannelso->setChecked(valB == c_true);
							checkBox_alsa_allchannelso->setEnabled(true);
						}
						else
						{
							checkBox_alsa_allchannelso->setEnabled(false);
						}

						token = "/JVX_GENW/JVX_ALSA_ACCESSTYPE";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(
							&selList, 1,
							JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							jvxSize idSel = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
							comboBox_alsa_acc_tp->clear();
							for (i = 0; i < selList.strList.ll(); i++)
							{
								std::string txt = selList.strList.std_str_at(i);
								if (i == idSel)
								{
									txt += "*";
								}
								comboBox_alsa_acc_tp->addItem(txt.c_str());
							}
							comboBox_alsa_acc_tp->setCurrentIndex(idSel);
							comboBox_alsa_acc_tp->setEnabled(true);
						}
						else
						{
							comboBox_alsa_acc_tp->setEnabled(false);
						}

						token = "/JVX_GENW/JVX_ALSA_DATATYPE";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(
							&selList, 1,
							JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							jvxSize idSel = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), selList.strList.ll());
							comboBox_alsa_datatp->clear();
							for (i = 0; i < selList.strList.ll(); i++)
							{
								std::string txt = selList.strList.std_str_at(i);
								if (i == idSel)
								{
									txt += "*";
								}
								comboBox_alsa_datatp->addItem(txt.c_str());
							}
							comboBox_alsa_datatp->setCurrentIndex(idSel);
							comboBox_alsa_datatp->setEnabled(true);
						}
						else
						{
							comboBox_alsa_datatp->setEnabled(false);
						}

						token = "/JVX_GENW/JVX_ALSA_PERIODS_IN";
						ident.reset(token.c_str());
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valRange, 1, JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
						  lineEdit_alsa_periodsi->setText(jvx_data2String(valRange.val(), 0).c_str());
							std::string txt = jvx_data2String(valRange.minVal, 0) + ".." + jvx_data2String(valRange.maxVal, 0);
							lineEdit_alsa_periodsi->setToolTip(txt.c_str());
							lineEdit_alsa_periodsi->setEnabled(true);
						}
						else
						{
							lineEdit_alsa_periodsi->setEnabled(false);
						}

						token = "/JVX_GENW/JVX_ALSA_PERIODS_OUT";
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(
							&valRange, 1, JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
						  lineEdit_alsa_periodso->setText(jvx_data2String(valRange.val(), 0).c_str());
							std::string txt = jvx_data2String(valRange.minVal, 0) + ".." + jvx_data2String(valRange.maxVal, 0);
							lineEdit_alsa_periodso->setToolTip(txt.c_str());
							lineEdit_alsa_periodso->setEnabled(true);
						}
						else
						{
							lineEdit_alsa_periodso->setEnabled(false);
						}
#endif
					}
				} // if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
				else
				{
					valB = c_false;
					token = "/system/act_fillup_silence_stop";
					ident.reset(token.c_str());
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRIO<jvxCBool>(valB), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						checkBox_stopsilence->setChecked(valB == c_true);
						checkBox_stopsilence->setEnabled(true);
					}
					else
					{
						checkBox_stopsilence->setChecked(false);
						checkBox_stopsilence->setEnabled(false);
					}

					valD = 0;
					token = "/system/per_fillup_silence_stop";
					ident.reset(token.c_str());
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, token, myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						lineEdit_stopsilence->setText(jvx_data2String(valD, 1).c_str());
						lineEdit_stopsilence->setEnabled(true);
					}
					else
					{
						lineEdit_stopsilence->setEnabled(false);
					}

					if (stat == JVX_STATE_ACTIVE)
					{
						this->progressBar_infile->setValue(0);
						this->progressBar_load->setValue(0);
					}
				}

				if(stat >= JVX_STATE_PREPARED)
				{
					this->pushButton_restart->setEnabled(true);
					this->checkBox_infileloop->setEnabled(false);

					comboBox_ifilelahead->setEnabled(false);
					checkBox_infile_boost->setEnabled(false);
					comboBox_outfilelahead->setEnabled(false);
					lineEdit_outchannels->setEnabled(false);
					pushButton_inc_outchannels->setEnabled(false);
					pushButton_dec_outchannels->setEnabled(false);
					lineEdit_outfile_rate->setEnabled(false);
					checkBox_outfile_boost->setEnabled(false);
					comboBox_srates->setEnabled(false);
					lineEdit_srates->setEnabled(false);
					comboBox_bsizes->setEnabled(false);
					lineEdit_bsize->setEnabled(false);
					comboBox_formathw->setEnabled(false);
					checkBox_audioparams->setEnabled(false);
					comboBox_resampler_in->setEnabled(false);
					comboBox_resampler_out->setEnabled(false);
					pushButton_asio_control_open->setEnabled(false);

					this->pushButton_bwd->setEnabled(true);
					this->pushButton_fwd->setEnabled(true);
					this->pushButton_play->setEnabled(true);
					this->pushButton_pause->setEnabled(true);
					this->pushButton_restart->setEnabled(true);

				}
			}
		}
	}
}



void
additionalArgsWidget::updateWindow_periodic()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxBool foundit = false;
	std::string str;
	jvxInt32 valI32;
	jvxData dblVal = 0;
	jvxInt16* inBuf16 = NULL;
	jvxInt32* inBuf32 = NULL;
	jvxSize idx = 0;
	theHost = myParent->involvedHost.hHost;

	theHost->state(&stat);

	if(stat == JVX_STATE_ACTIVE)
	{
		theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY], &stat);
		if(stat == JVX_STATE_ACTIVE)
		{
			jvxSelectionList selList;
			jvxApiStringList fNames;
			if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_SHIFT))
			{
				ident.reset("/select_files/input_file");
				trans.reset(true);
				res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/select_files/input_file",
					myParent->subWidgets.theAudioDialog->currentPropsTech);
				assert(rr);

				jvxSize num_input_files = selList.strList.ll();
				jvxSize id_infile = jvx_bitfieldSelection2Id(selList.bitFieldSelected(), num_input_files);
				if (JVX_CHECK_SIZE_SELECTED(id_infile))
				{
					ident.reset("/properties_selected_input_file/progress");
					trans.reset(true, id_infile);
					res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);
					bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/progress",
						myParent->subWidgets.theAudioDialog->currentPropsTech);
					assert(rr);

					valI32 = dblVal * 100.0;
					valI32 = JVX_MIN(100, valI32);
					valI32 = JVX_MAX(0, valI32);
					this->progressBar_infile->setValue(valI32);
				}

				theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
				if (stat >= JVX_STATE_ACTIVE)
				{
					lineEdit_lostBuffers->setText("--/--");
					// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
					ident.reset("/JVX_GENW_DEV_LOAD");
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), 
						ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_LOAD", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						valI32 = dblVal;
						valI32 = JVX_MIN(100, valI32);
						valI32 = JVX_MAX(0, valI32);
						this->progressBar_load->setValue(valI32);
						this->label_load->setText((jvx_data2String(dblVal, 2) + " %").c_str());
						this->progressBar_load->setEnabled(true);
						this->label_load->setEnabled(true);
					}
					else
					{
						this->progressBar_load->setEnabled(false);
						this->label_load->setEnabled(false);

						this->progressBar_load->setValue(0);
						this->label_load->setText("*unsupported*");
					}
				
					valI32 = 0;
					ident.reset("/system/number_lost_buffer");
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/system/number_lost_buffer", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						str = jvx_int2String(valI32);
						
						valI32 = 0;
						ident.reset("/JVX_GENW/system/number_lost_buffer");
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/system/number_lost_buffer", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							str += "/";
							str += jvx_int2String(valI32);
						}
						lineEdit_lostBuffers->setText(str.c_str());
					}
					else
					{
						lineEdit_lostBuffers->setText("*unsupported*");
					}

					lineEdit_rate->setText("--/--");
					dblVal = 0;
					ident.reset("/current_rate_hw");
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);

					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/current_rate_hw", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						str = jvx_data2String(dblVal, 1);
						
						dblVal = 0;
						ident.reset("/current_rate_sw");
						trans.reset(true);
						res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);

						if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/current_rate_sw", myParent->subWidgets.theAudioDialog->currentPropsDev))
						{
							str += "/" + jvx_data2String(dblVal, 1);
						}
						lineEdit_rate->setText(str.c_str());
					}
					else
					{ 
						lineEdit_rate->setText("*unsupported*");
					}
				}
			}
			else
			{
				theHost->state_selected_component(tpAll[JVX_COMPONENT_AUDIO_DEVICE], &stat);
				if (stat >= JVX_STATE_ACTIVE)
				{
					lineEdit_lostBuffers->setText("unsupported");
					ident.reset("/JVX_GENW/loadpercent");
					trans.reset(true);
					// Show property with descriptor "/JVX_GENW_TECHNOLOGIES"
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA),
						ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/loadpercent", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						valI32 = dblVal;
						valI32 = JVX_MIN(100, valI32);
						valI32 = JVX_MAX(0, valI32);
						this->progressBar_load->setValue(valI32);
						this->label_load->setText((jvx_data2String(dblVal, 2) + " %").c_str());
						this->progressBar_load->setEnabled(true);
						this->label_load->setEnabled(true);
					}
					else
					{
						this->progressBar_load->setEnabled(false);
						this->label_load->setEnabled(false);

						this->progressBar_load->setValue(0);
						this->label_load->setText("*unsupported*");
					}



					valI32 = 0;
					ident.reset("/system/number_lost_buffer");
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/system/number_lost_buffer", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						str = jvx_int2String(valI32);
						lineEdit_lostBuffers->setText(str.c_str());
					}
					else
					{
						lineEdit_lostBuffers->setText("*unsupported*");
					}

					lineEdit_rate->setText("--/--");
					dblVal = 0;
					ident.reset("/current_rate_hw");
					trans.reset(true);
					res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&dblVal, 1, JVX_DATAFORMAT_DATA), ident, trans);

					if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/current_rate_hw", myParent->subWidgets.theAudioDialog->currentPropsDev))
					{
						str = jvx_data2String(dblVal, 1);
						lineEdit_rate->setText(str.c_str());
					}
					else
					{
						lineEdit_rate->setText("*unsupported*");
					}


				}
			}
		}
	}
}

void
additionalArgsWidget::new_input_lookahead(int sel)
{
	jvxErrorType res;
	jvxInt16 valI16;

	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;

	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = sel;
		ident.reset("/properties_selected_input_file/lookahead");
		trans.reset(true, id_infile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), 
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/lookahead",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
				
	}
	this->updateWindow();
	
}

void
additionalArgsWidget::new_input_boost()
{
	jvxErrorType res;
	jvxInt16 valI16;
	jvxCallManagerProperties callGate;
	jvxSize num_input_files;
	jvxSize id_infile;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = c_false;
		if (checkBox_infile_boost->isChecked())
			valI16 = c_true;

		ident.reset("/properties_selected_input_file/boost_prio");
		trans.reset(true, id_infile);

		ident.reset("/properties_selected_input_file/boost_prio");
		trans.reset(true);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/boost_prio",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();

}

void
additionalArgsWidget::new_input_loop()
{
	jvxInt16 valI16;
	jvxErrorType res;
	jvxCallManagerProperties callGate;
	jvxSize num_input_files;
	jvxSize id_infile;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = c_false;
		if (checkBox_infileloop->isChecked())
			valI16 = c_true;

		ident.reset("/properties_selected_input_file/loop");
		trans.reset(true, id_infile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/loop",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_select_auto_stop()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt16 valI16 = c_false;;
	jvxCallManagerProperties callGate;
	ident.reset("/auto_stop");
	trans.reset(true, 0);

	ident.reset("/auto_stop");
	trans.reset(true);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&valI16, 1,
		JVX_DATAFORMAT_16BIT_LE), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/auto_stop", myParent->subWidgets.theAudioDialog->currentPropsDev))
	{
		if(valI16)
			valI16 = c_false;
		else
			valI16 = c_true;
	}

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans );
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_pause_on_start()
{
	jvxInt16 valI16;
	jvxErrorType res;
	jvxCallManagerProperties callGate;
	jvxSize num_input_files;
	jvxSize id_infile;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = c_false;
		if (checkBox_pauseonstart->isChecked())
			valI16 = c_true;

		ident.reset("/properties_selected_input_file/pause_on_start");
		trans.reset(true, id_infile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( 
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans );
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/pause_on_start",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_dec_output_channels()
{
	jvxErrorType res;
	jvxSize valSz;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		ident.reset("/properties_selected_output_file/num_channels");
		trans.reset(true, id_outfile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG( &valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/num_channels",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

		valSz = JVX_MAX(1, valSz-1);

		ident.reset("/properties_selected_output_file/num_channels");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/num_channels",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_inc_output_channels()
{
	jvxErrorType res;
	jvxSize valSz;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		ident.reset("/properties_selected_output_file/num_channels");
		trans.reset(true, id_outfile);

		ident.reset("/properties_selected_output_file/num_channels");
		trans.reset(true, id_outfile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->get_property(callGate, jPRG( &valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/num_channels",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

		valSz++;
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valSz, 1, JVX_DATAFORMAT_SIZE),ident, trans );
		rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/num_channels",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}
void
additionalArgsWidget::new_output_channels_text()
{
	QString txt = this->lineEdit_outchannels->text();
	jvxErrorType res;
	jvxSize valSz;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		valSz = txt.toInt();
		valSz = JVX_MAX(valSz, 1);

		ident.reset("/properties_selected_output_file/num_channels");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valSz, 1, JVX_DATAFORMAT_SIZE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/num_channels",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_mode_output(int selId)
{
	jvxErrorType res;
	jvxSelectionList sel;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		jvx_bitZSet(sel.bitFieldSelected(), selId);

		ident.reset("/properties_selected_output_file/mode");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( &sel, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/mode",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_output_srate()
{
	QString txt = this->lineEdit_outfile_rate->text();
	jvxErrorType res;
	jvxSize valSz;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		valSz = txt.toInt();

		ident.reset("/properties_selected_output_file/samplerate");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valSz, 1, JVX_DATAFORMAT_SIZE),ident, trans );
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/samplerate",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_output_boost()
{
	jvxErrorType res;
	jvxInt16 valI16;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;
	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		valI16 = c_false;
		if (checkBox_outfile_boost->isChecked())
			valI16 = c_true;

		ident.reset("/properties_selected_output_file/boost_prio");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/boost_prio",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_format_hw(int sel)
{
	jvxErrorType res;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = (jvxBitField)1 << sel;

	ident.reset("/JVX_GENW/system/sel_dataformat");
	trans.reset(true, 0);

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, "/JVX_GENW/system/sel_dataformat",
		myParent->subWidgets.theAudioDialog->currentPropsDev);
	assert(rr);
	myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_check_autohw()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	ident.reset("/JVX_GENW_DEV_AUTOHW");
	trans.reset(true, 0);

	ident.reset("/JVX_GENW_DEV_AUTOHW");
	trans.reset(true);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
		ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_AUTOHW", myParent->subWidgets.theAudioDialog->currentPropsDev))
	{
		if (JVX_EVALUATE_BITFIELD(selList.bitFieldSelected() & 0x1))
		{
			selList.bitFieldSelected() = 0x2;
		}
		else
		{
			selList.bitFieldSelected() = 0x1;
		}
	}
	else
	{
		assert(0);
	}
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( 
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

#ifdef JVX_OS_LINUX
void
additionalArgsWidget::open_cpanel()
{
	// Nothing to do in Linux
}

#else

void
additionalArgsWidget::open_cpanel()
{
	IjvxHost* theHost = NULL;
	jvxState stat = JVX_STATE_INIT;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = 0x2;

	ident.reset("/JVX_GENW/JVX_CTRL_PANEL");
	trans.reset(true, 0);

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	this->updateWindow();
}
#endif

void
additionalArgsWidget::new_output_lookahead(int sel)
{
	jvxErrorType res;
	jvxInt16 valI16;
	jvxCallManagerProperties callGate;
	jvxSize num_output_files;
	jvxSize id_outfile;

	GET_FILE_SELECT_REF_PROPS(num_output_files, id_outfile, "/select_files/output_file",callGate);

	if (JVX_CHECK_SIZE_SELECTED(id_outfile))
	{
		valI16 = sel;

		ident.reset("/properties_selected_output_file/lookahead");
		trans.reset(true, id_outfile);

		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( &valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans );
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_output_file/lookahead",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);

	}
	this->updateWindow();
}

void
additionalArgsWidget::new_quality_resampler_in(int sel)
{
	jvxErrorType res;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = ((jvxBitField)1 << sel);

	ident.reset("/JVX_GENW_DEV_QRESAMPLER_IN");
	trans.reset(true, 0);

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_QRESAMPLER_IN",
		myParent->subWidgets.theAudioDialog->currentPropsDev);
	assert(rr);
	this->updateWindow();
}

void
additionalArgsWidget::new_quality_resampler_out(int sel)
{
	jvxErrorType res;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = ((jvxBitField)1 << sel);

	ident.reset("/JVX_GENW_DEV_QRESAMPLER_OUT");
	trans.reset(true, 0);

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( 
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW_DEV_QRESAMPLER_OUT",
		myParent->subWidgets.theAudioDialog->currentPropsDev);
	assert(rr);
	this->updateWindow();
}

void
additionalArgsWidget::new_samplerate_hw(int sel)
{
	jvxErrorType res;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = ((jvxBitField)1 << sel);

	ident.reset("/JVX_GENW/system/sel_rate");
	trans.reset(true, 0);

	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/system/sel_rate",
		myParent->subWidgets.theAudioDialog->currentPropsDev);
	assert(rr);
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_samplerate_hw()
{
	QString txt = this->lineEdit_srates->text();
	jvxInt32 valI32 = txt.toInt();
	jvxCallManagerProperties callGate;
	if(valI32 > 0) // <- on linux, this function is also called on show(). Then, however, the value is zero
	{
		jvxErrorType res;

		ident.reset("/JVX_GENW/system/rate");
		trans.reset(true, 0);

		res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( 
			&valI32, 1, JVX_DATAFORMAT_32BIT_LE), 
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/system/rate",
			myParent->subWidgets.theAudioDialog->currentPropsDev);
		assert(rr);
		this->myParent->subWidgets.theAudioDialog->updateWindow();
	}
}

void
additionalArgsWidget::new_buffersize_hw(int sel)
{
	jvxErrorType res;
	jvxSelectionList selList;
	jvxCallManagerProperties callGate;
	selList.bitFieldSelected() = ((jvxBitField)1 << sel);
	ident.reset("/JVX_GENW/system/sel_framesize");
	trans.reset(true, 0);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
		ident, trans);
	bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/system/sel_framesize",
		myParent->subWidgets.theAudioDialog->currentPropsDev);
	assert(rr);
	this->myParent->subWidgets.theAudioDialog->updateWindow();
}

void
additionalArgsWidget::new_buffersize_hw()
{
	QString txt = this->lineEdit_bsize->text();
	jvxInt32 valI32 = txt.toInt();
	jvxCallManagerProperties callGate;
	if(valI32 > 0) // <- on linux, this function is also called on show(). Then, however, the value is zero
	{
		jvxErrorType res;
		ident.reset("/JVX_GENW/system/framesize");
		trans.reset(true, 0);
		res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG( 
			&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/JVX_GENW/system/framesize",
			myParent->subWidgets.theAudioDialog->currentPropsDev);
		assert(rr);
		this->myParent->subWidgets.theAudioDialog->updateWindow();
	}
}

void
additionalArgsWidget::trigger_rewind()
{
	jvxInt16 valI16 = 0;
	jvxErrorType res;
	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);
	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = 1;
		ident.reset("/properties_selected_input_file/triggerBwd");
		trans.reset(true, id_infile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( 
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE), 
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/triggerBwd",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
}

void
additionalArgsWidget::trigger_play()
{
	jvxInt16 valI16 = 0;
	jvxErrorType res;
	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);
	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = 1;
		ident.reset("/properties_selected_input_file/triggerPlay");
		trans.reset(true, id_infile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( 
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE),
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/triggerPlay",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
}

void
additionalArgsWidget::trigger_pause()
{
	jvxInt16 valI16 = 0;
	jvxErrorType res;
	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);
	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = 1;
		ident.reset("/properties_selected_input_file/triggerPause");
		trans.reset(true, id_infile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG( 
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE),
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/triggerPause",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
}

void
additionalArgsWidget::trigger_forward()
{
	jvxInt16 valI16 = 0;
	jvxErrorType res;
	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);

	if(JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = 1;
		ident.reset("/properties_selected_input_file/triggerFwd");
		trans.reset(true, id_infile);
		res =myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE), 
			ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/triggerFwd",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
}

void
additionalArgsWidget::trigger_restart()
{
	jvxInt16 valI16 = 0;
	jvxErrorType res;
	jvxSize num_input_files;
	jvxSize id_infile;
	jvxCallManagerProperties callGate;
	GET_FILE_SELECT_REF_PROPS(num_input_files, id_infile, "/select_files/input_file",callGate);
	if (JVX_CHECK_SIZE_SELECTED(id_infile))
	{
		valI16 = 1;
		ident.reset("/properties_selected_input_file/triggerRestart");
		trans.reset(true, id_infile);
		res = myParent->subWidgets.theAudioDialog->currentPropsTech->set_property(callGate, jPRG(
			&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
		bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res,callGate.access_protocol, "/properties_selected_input_file/triggerRestart",
			myParent->subWidgets.theAudioDialog->currentPropsTech);
		assert(rr);
	}
}

void
additionalArgsWidget::new_period_silence_stop()
{
	QString txt = lineEdit_stopsilence->text();
	jvxData valD = txt.toData();
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
	{
		token = "/JVX_GENW/system/per_fillup_silence_stop";
	}
	else
	{
		token = "/system/per_fillup_silence_stop";
	}
	ident.reset(token.c_str());
	trans.reset(true, 0);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&valD, 1, JVX_DATAFORMAT_DATA), 
		ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
	this->updateWindow();
}

void
additionalArgsWidget::clicked_silence_stop_active(bool clicked)
{
	QString txt = lineEdit_stopsilence->text();
	jvxCBool valB = c_false;
	std::string token;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	if (clicked)
		valB = c_true;

	if (jvx_bitTest(myParent->subWidgets.theAudioDialog->modeShow, JVX_UI_SHOW_MODE_GENW_ANY_SHIFT))
	{
		token = "/JVX_GENW/system/act_fillup_silence_stop";
	}
	else
	{
		token = "/system/act_fillup_silence_stop";
	}
	
	ident.reset(token.c_str());
	trans.reset(true, 0);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
		&valB, 1, JVX_DATAFORMAT_16BIT_LE), 
		ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
	this->updateWindow();
}

void
additionalArgsWidget::closeEvent(QCloseEvent * event)
{
	QDialog::closeEvent(event);
	if (myParent->involvedHost.hHost)
	{
		myParent->updateWindow();
	}
};

#ifdef JVX_OS_LINUX
void
additionalArgsWidget::new_alsa_check_all_channelsi(bool tog)
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxInt16 valB = c_false;
    jvxCallManagerProperties callGate;

    token = "/JVX_GENW/activateAllChannelsInput";
    if(tog)
        valB = c_true;

    ident.reset(token.c_str());
	trans.reset(true, 0);
	
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(&valB, 1, JVX_DATAFORMAT_16BIT_LE),
										 ident, trans );
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

void
additionalArgsWidget::new_alsa_check_all_channelso(bool tog)
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxInt16 valB = c_false;
    jvxCallManagerProperties callGate;

    token = "/JVX_GENW/activateAllChannelsOutput";
    if(tog)
        valB = c_true;
    ident.reset(token.c_str());
	trans.reset(true, 0);
	res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(&valB, 1, JVX_DATAFORMAT_16BIT_LE),
										 ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

void
additionalArgsWidget::new_alsa_access_tp(int idx)
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxSelectionList selList;
    jvxCallManagerProperties callGate;

    token = "/JVX_GENW/JVX_ALSA_ACCESSTYPE";
    jvx_bitZSet(selList.bitFieldSelected(), idx);
    ident.reset(token.c_str());
     trans.reset(true, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);
     res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(
											     &selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
		ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

void
additionalArgsWidget::new_alsa_data_tp(int idx)
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxSelectionList selList;
    jvxCallManagerProperties callGate;

    token = "/JVX_GENW/JVX_ALSA_DATATYPE";
     jvx_bitZSet(selList.bitFieldSelected(), idx);
     ident.reset(token.c_str());
     trans.reset(true, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);
     res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(callGate, jPRG(&selList, 1, JVX_DATAFORMAT_SELECTION_LIST),
									     ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

void
additionalArgsWidget::new_alsa_period_i()
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxValueInRange valRange;
    jvxCallManagerProperties callGate;
    QString txt = lineEdit_alsa_periodsi->text();

    token = "/JVX_GENW/JVX_ALSA_PERIODS_IN";
     ident.reset(token.c_str());
     trans.reset(true, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);

    res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(
                &valRange, 1,
                JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);

									     valRange.val() = txt.toInt();
									     valRange.val() = JVX_MAX(valRange.minVal, valRange.val());
									     valRange.val() = JVX_MIN(valRange.maxVal, valRange.val());

    ident.reset(token.c_str());
     trans.reset(false, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);
    res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(
									    callGate,
									    jPRG(&valRange, 1, JVX_DATAFORMAT_VALUE_IN_RANGE),
		ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

void
additionalArgsWidget::new_alsa_period_o()
{
    std::string token;
    jvxErrorType res = JVX_NO_ERROR;
    jvxValueInRange valRange;
    jvxCallManagerProperties callGate;
    QString txt = lineEdit_alsa_periodso->text();

     token = "/JVX_GENW/JVX_ALSA_PERIODS_OUT";
     ident.reset(token.c_str());
     trans.reset(true, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);

    res = myParent->subWidgets.theAudioDialog->currentPropsDev->get_property(callGate, jPRG(
                &valRange, 1,
                JVX_DATAFORMAT_VALUE_IN_RANGE), ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);

									     valRange.val() = txt.toInt();
									     valRange.val() = JVX_MAX(valRange.minVal, valRange.val());
									     valRange.val() = JVX_MIN(valRange.maxVal, valRange.val());

    ident.reset(token.c_str());
     trans.reset(false, 0,JVX_PROPERTY_DECODER_SINGLE_SELECTION);
    res = myParent->subWidgets.theAudioDialog->currentPropsDev->set_property(
									    callGate,
									    jPRG(&valRange, 1, JVX_DATAFORMAT_VALUE_IN_RANGE),
									     ident, trans);
    JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, token);
    this->updateWindow();
}

#else

void
additionalArgsWidget::new_alsa_check_all_channelsi(bool tog)
{
	// Nothing to do in Windows
}

void
additionalArgsWidget::new_alsa_check_all_channelso(bool tog)
{
	// Nothing to do in Windows
}

void
additionalArgsWidget::new_alsa_access_tp(int idx)
{
	// Nothing to do in Windows
}

void
additionalArgsWidget::new_alsa_data_tp(int idx)
{
	// Nothing to do in Windows
}

void
additionalArgsWidget::new_alsa_period_i()
{
	// Nothing to do in Windows
}

void
additionalArgsWidget::new_alsa_period_o()
{
	// Nothing to do in Windows
}
#endif
