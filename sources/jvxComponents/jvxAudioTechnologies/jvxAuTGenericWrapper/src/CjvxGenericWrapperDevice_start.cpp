#include "CjvxGenericWrapperDevice.h"
#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapper_common.h"

jvxErrorType
CjvxGenericWrapperDevice::start()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::start();
	if(res == JVX_NO_ERROR)
	{
		if (onInit.connectedDevice)
		{
			res = onInit.connectedDevice->start();
		}
		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::stop()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::stop();
 
	if(res == JVX_NO_ERROR)
	{
		if (onInit.connectedDevice)
		{
			res = onInit.connectedDevice->stop();
		}
		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR, resL;

	resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	// Locked after state prepared has been completed
	for (i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	{
		if (onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		{
			resL = onInit.theFilesRef->theInputFiles[i].theReader->start();
			assert(resL == JVX_NO_ERROR);
		}
	}

	for (i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	{
		if (onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
		{
			resL = onInit.theFilesRef->theOutputFiles[i].theWriter->start();
			assert(resL == JVX_NO_ERROR);
		}
	}

	// Locked after state prepared has been completed
	for (i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
	{
		if (onInit.theExtRef->theInputReferences[i].processing.selected)
		{
			resL = onInit.theExtRef->theInputReferences[i].theCallback->start(true, onInit.theExtRef->theInputReferences[i].uniqueIdHandles);
			assert(resL == JVX_NO_ERROR);
		}
	}

	for (i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
	{
		if (onInit.theExtRef->theOutputReferences[i].processing.selected)
		{
			resL = onInit.theExtRef->theOutputReferences[i].theCallback->start(true, onInit.theExtRef->theOutputReferences[i].uniqueIdHandles);
			assert(resL == JVX_NO_ERROR);
		}
	}
	// Indicate that no file has triggered end yet
	this->proc_fields.threads.endTriggered = false;

	CjvxAudioDevice_genpcg::properties_active.lostBuffers.value = 0;
	//	processingControl.performance.numAudioThreadFailed = 0;


	// All threads are in startup position but the very first data is only going to flow if the connected device has been started
	if (onInit.connectedDevice)
	{
		// At this point, really start the threads and whatever
		switch (proc_fields.doSelection)
		{
		case SELECTION_ASYNC_MODE:

			processingControl.deltaWrite = 0;
			if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.nonBlockingAsync.value.selection() & 0x1))
			{
				// Non-blocking
				jvx_thread_initialize(&proc_fields.threads.thread_handle, &proc_fields.threads.thread_handler, reinterpret_cast<jvxHandle*>(this), true, false);
			}
			else
			{
				jvx_thread_initialize(&proc_fields.threads.thread_handle, &proc_fields.threads.thread_handler, reinterpret_cast<jvxHandle*>(this), true, true, true);
			}
			jvx_thread_start(proc_fields.threads.thread_handle, JVX_INFINITE_MS);
			jvx_thread_set_priority(proc_fields.threads.thread_handle, JVX_THREAD_PRIORITY_REALTIME);

		case SELECTION_SYNC_MODE:

			break;

		case SELECTION_FILEIO_MODE:

			// Same as file io mode
			res = _start_chain_master_no_link();
			if (res != JVX_NO_ERROR)
			{
				resL = _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				assert(resL == JVX_NO_ERROR);
				goto leave_error;
			}

			assert(proc_fields.doSelection == SELECTION_FILEIO_MODE);
			jvx_thread_initialize(&proc_fields.threads.thread_handle, &proc_fields.threads.thread_handler, reinterpret_cast<jvxHandle*>(this), true, false);
			jvx_thread_start(proc_fields.threads.thread_handle, 0); // This causes the main processing to happen in the timeout function
			goto leave_no_error;
			break;
		default:
			assert(0);
			break;
		}

		res = _start_chain_master_no_link();
		if (res != JVX_NO_ERROR)
		{
			goto leave_error;
		}

		// This connects all elements for "start" and THEN starts the processing
		res = theRelocator.x_start_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			resL = theRelocator.x_stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resL == JVX_NO_ERROR);
			goto leave_error;
		}
	}
	else
	{
		res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			resL = _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resL == JVX_NO_ERROR);
			goto leave_error;
		}

		assert(proc_fields.doSelection == SELECTION_FILEIO_MODE);
		jvx_thread_initialize(&proc_fields.threads.thread_handle, &proc_fields.threads.thread_handler, reinterpret_cast<jvxHandle*>(this), true, false);
		jvx_thread_start(proc_fields.threads.thread_handle, 0); // This causes the main processing to happen in the timeout function
	}

leave_no_error:
	return res;

leave_error:
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool locres = _check_stop_chain_master();
	assert(locres);

	if (onInit.connectedDevice)
	{


		switch (proc_fields.doSelection)
		{
		case SELECTION_ASYNC_MODE:

			// This does at first stop the device
			res = theRelocator.x_stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Stopping chain failed!" << std::endl;
			}

			jvx_thread_stop(proc_fields.threads.thread_handle);
			jvx_thread_terminate(proc_fields.threads.thread_handle);
			break;

		case SELECTION_SYNC_MODE:

			// This does at first stop the device
			res = theRelocator.x_stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Stopping chain failed!" << std::endl;
			}

			break;
		case SELECTION_FILEIO_MODE:

			assert(proc_fields.doSelection == SELECTION_FILEIO_MODE);
			jvx_thread_stop(proc_fields.threads.thread_handle);
			jvx_thread_terminate(proc_fields.threads.thread_handle);

			res = _stop_chain_master_no_link();
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Stopping chain failed!" << std::endl;
			}
			break;
		}
	}
	else
	{
		assert(proc_fields.doSelection == SELECTION_FILEIO_MODE);
		jvx_thread_stop(proc_fields.threads.thread_handle);
		jvx_thread_terminate(proc_fields.threads.thread_handle);

		res = _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Stopping chain failed!" << std::endl;
		}
	}

	// Locked after state prepared has been completed
	for (i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
	{
		if (onInit.theExtRef->theInputReferences[i].processing.selected)
		{
			onInit.theExtRef->theInputReferences[i].theCallback->stop(true, onInit.theExtRef->theInputReferences[i].uniqueIdHandles);
		}
	}

	for (i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
	{
		if (onInit.theExtRef->theOutputReferences[i].processing.selected)
		{
			onInit.theExtRef->theOutputReferences[i].theCallback->stop(false, onInit.theExtRef->theInputReferences[i].uniqueIdHandles);
		}
	}

	// Locked after state prepared has been completed
	for (i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	{
		if (onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		{
			onInit.theFilesRef->theInputFiles[i].theReader->stop();
		}
	}

	for (i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	{
		if (onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
		{
			onInit.theFilesRef->theOutputFiles[i].theWriter->stop();
		}
	}

	if (onInit.connectedDevice)
	{
		_stop_chain_master_no_link();
	}
	
	assert(_common_set_min.theState == JVX_STATE_PROCESSING);

	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericWrapperDevice::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	_common_set.theUniqueId->obtain_unique_id(&idRuntime, "GW");

	CjvxInputOutputConnector::_start_connect_icon(false, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	// If a device is involved, pass chaining frowad to involved hardware device
	if (onInit.connectedDevice)
	{
		// Change over to host relocator
		res = theRelocator.start_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
};

jvxErrorType
CjvxGenericWrapperDevice::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;

	CjvxInputOutputConnector::_stop_connect_icon(false, &idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	_common_set.theUniqueId->release_unique_id(idRuntime);

	// If a device is involved, pass chaining frowad to involved hardware device
	if (onInit.connectedDevice)
	{
		// Change over to host relocator
		res = theRelocator.stop_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
};