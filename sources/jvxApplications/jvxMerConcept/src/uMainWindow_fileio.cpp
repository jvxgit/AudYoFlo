#include "uMainWindow.h"
#include "sndfile.h"
#include "CjvxOneAudioBuffer.h"
#include <QtWidgets/QMessageBox>
#include "uLoadSaveDialog.h"

static jvxData oneBuf[JVX_STANDALONE_HOST_READ_LENGTH_CHUNKS_FILE * JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];

void
uMainWindow::readAudioFile(std::string& fileName)
{
	SF_INFO info;
	memset(&info, 0, sizeof(SF_INFO));
	SNDFILE* fHandle = sf_open(fileName.c_str(), SFM_READ, &info) ;
	jvxData progress = 0.0;
	jvxData progressSteps = 0.1;
	jvxData progressReport = progress + progressSteps;
	jvxInt64 fCount = 0;
	if(fHandle)
	{
		if(
			(info.channels == JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS) &&
			(info.samplerate == JVX_STANDALONE_HOST_SAMPLERATE))
		{
			uLoadSaveDialog * theDialog = new uLoadSaveDialog(this);
			theDialog->setupUi();
			theDialog->setText(fileName);
			theDialog->setProgress(0.0);
			const char* tag = sf_get_string(fHandle, SF_STR_COMMENT);
			std::string txt = "";
			if(tag)
			{
				txt = tag;
			}
			CjvxOneAudioBuffer* newBuffer = new CjvxOneAudioBuffer;

			newBuffer->configure(JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS, JVX_STANDALONE_HOST_SAMPLERATE, JVX_STANDALONE_HOST_SEGMENT_LENGTH_SECONDS, txt.c_str(), fileName.c_str());
			newBuffer->prepare_write(false);

			jvxBool moreToRead = true;
			theDialog->setStopVariable(&moreToRead);
			theDialog->show();
			theDialog->repaint();
			qApp->processEvents();
			while(moreToRead)
			{
				jvxSize haveRead = sf_readf_data(fHandle, oneBuf, JVX_STANDALONE_HOST_READ_LENGTH_CHUNKS_FILE);
				newBuffer->add_samples_i(oneBuf, JVX_STANDALONE_HOST_READ_LENGTH_CHUNKS_FILE);
				if(haveRead != JVX_STANDALONE_HOST_READ_LENGTH_CHUNKS_FILE)
				{
					break;
				}

				fCount += haveRead;
				progress = (jvxData)fCount/(jvxData)info.frames;
				if(progress >= progressReport)
				{
					progressReport += progressSteps;					
					theDialog->setProgress(progress);
					qApp->processEvents();
				}
			}
			delete(theDialog);

			newBuffer->postprocess_write();
			jvxErrorType res = readwrite.theBuffers.addEntry(newBuffer);
			if(res != JVX_NO_ERROR)
			{
				delete(newBuffer);
			}
		}
		else
		{
			QMessageBox msgBox(QMessageBox::Critical, ("Failed to load file " + fileName).c_str(), "File does not fulfill the constraints");
			msgBox.exec();
		}
		sf_close(fHandle);
	}
}

void
uMainWindow::writeAudioFile(std::string& fileName)
{
	SF_INFO info;
	memset(&info, 0, sizeof(SF_INFO));
	info.channels = JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;

	info.frames = 0;
	info.samplerate = JVX_STANDALONE_HOST_SAMPLERATE;
	info.sections = 0;
	info.seekable = 0;
	jvxErrorType res = JVX_NO_ERROR;

	SNDFILE* fHandle = sf_open(fileName.c_str(), SFM_WRITE, &info) ;
	jvxData progress = 0.0;
	jvxData progressSteps = 0.1;
	jvxData progressReport = progress + progressSteps;
	jvxInt64 fCount = 0;
	if(fHandle)
	{
		uLoadSaveDialog * theDialog = new uLoadSaveDialog(this);
		theDialog->setupUi();
		theDialog->setText(fileName);
		theDialog->setProgress(0.0);
		

		CjvxOneAudioBuffer* newBuffer = NULL;
		std::string tag;
		res = this->readwrite.theBuffers.borrowActiveEntry(&newBuffer);
		if((res == JVX_NO_ERROR) && newBuffer)
		{
			sf_set_string(fHandle, SF_STR_COMMENT, tag.c_str());

			jvxSize sz = 0;
			newBuffer->prepare_read(NULL);
			newBuffer->length_samples(sz);

			jvxBool moreToWrite = true;
			jvxSize num = 0;
			
			theDialog->setStopVariable(&moreToWrite);
			theDialog->show();
			theDialog->repaint();
			qApp->processEvents();
			while(moreToWrite)
			{
				res = newBuffer->get_samples_i(oneBuf, JVX_STANDALONE_HOST_READ_LENGTH_CHUNKS_FILE, false, num);
				if((res == JVX_NO_ERROR) || (res == JVX_ERROR_END_OF_FILE))
				{
					jvxSize haveWritten = sf_writef_data(fHandle, oneBuf, num);
					if(res == JVX_ERROR_END_OF_FILE)
					{
						moreToWrite = false;
						break;
					}
					else
					{
						fCount += haveWritten;
						
						progress = (jvxData)fCount/(jvxData)sz;
						if(progress >= progressReport)
						{
							progressReport += progressSteps;
							theDialog->setProgress(progress);
							qApp->processEvents();
						}
					}
				}

			}			
			delete(theDialog);
			

			newBuffer->postprocess_read();
			this->readwrite.theBuffers.returnActiveEntry(newBuffer);
		}
		sf_close(fHandle);
	}
}