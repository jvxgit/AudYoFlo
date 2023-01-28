#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__


#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
#include "jvxAudioChannelHandler/IjvxExtInputChannelHandler.h"
#include "jvxAudioChannelHandler/IjvxExtOutputChannelHandler.h"
#include "mainCentral_host_ww.h"


#include <string>

#include <QList>
#include <QTimer>
#include <QDir>

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define MAX_NUMBER_OF_ITEMS 9

	typedef std::vector<std::vector<jvxData>> ItemSignals;

	struct TestItem {
		int itemIndex;
		QString filepath = "";
		std::string type;
		double volume = 1;
		std::vector<jvxData*> pointersToItemSignalChannels;
		std::list<ItemSignals>::iterator iterItemSignal;
		//int ItemSignalsIndex = -1;
		int lengthSignal = 0;
		int numChannels = 0;
		int sampleRate = 0;
		jvxData durationInSec = 0;
	};

	

	

	class myCentralWidget : public mainCentral_host_ww, public Ui::Form_myMainWidget
	{
	private:
		std::vector<TestItem> vectorOfItems;
		std::list<ItemSignals> listOfItemSignals;
		IjvxAccessProperties* thePropRef_algo;
		IjvxAccessProperties* thePropRef_dev;

		int currentTestItem = -1;
		jvxSize const maxNumOfItems = MAX_NUMBER_OF_ITEMS;

		QLabel* arrayOfLabels[MAX_NUMBER_OF_ITEMS];
		QPushButton* arrayOfButtons[MAX_NUMBER_OF_ITEMS];
		QSlider* arrayOfVolumeSlider[MAX_NUMBER_OF_ITEMS];
		QSpinBox* arrayOfVolumeSpinBox[MAX_NUMBER_OF_ITEMS];
		QPushButton* arrayOfFileButtons[MAX_NUMBER_OF_ITEMS];
		QLabel* arrayOfFilenameLabels[MAX_NUMBER_OF_ITEMS];
		QLabel* arrayOfdBLables[MAX_NUMBER_OF_ITEMS];
		QPushButton* arrayOfRemoveItemButtons[MAX_NUMBER_OF_ITEMS];

		jvxData maxSignalLengthInSec = 0;
		int longestSignalItem = -1;
		
		jvxBool audioIsPlaying = false;
		jvxBool audioIsPaused = false;

		int defaultVolumeUI = 0;

		jvxBool blindModeActivated = false;

		void init_listArrays();

		std::vector<IjvxExtInputChannelHandler*>vecOfInputHandlers;
		IjvxExtOutputChannelHandler* outputHandler;

		void readAudioFile(TestItem & newItem, ItemSignals & newSignals);

		IjvxExternalAudioChannels* theExtRefChannels;

		void newVolume(int itemIndex, jvxData volume);

		jvxSize getInputChannelOffset(int itemIndex);

		void setPlayProperties(int itemIndex);

		void setPlayMode(bool on);

		bool isWavFile(QString fileName);

		void updateEnableDisableItemWidgets();

		void addNewTestItem(int itemIndex, QString fileName);

		void updatePointersToSignals();

		void updateActiveLabels();

		void updateMaxSignalLength();

		

		QList <QWidget*> listDisableOnPlayback;

		QTimer* timeDisplayUpdateTimer;
		int updateTimerInterval = 20;
		int clippingReleaseTime = 150;

		jvxSize samplerateProcessing;

		void updateLoopAndPlaySliderValues();

		int timeSliderMaxValue = 999;
		void setLoopStart(jvxData positionInSec);
		void setLoopEnd(jvxData positionInSec);

		jvxData loopStartInSec = 0;
		jvxData loopEndInSec = 0;

		void setLoopStartKeyPress();
		void resetLoopStartKeyPress();
		void setLoopEndKeyPress();
		void resetLoopEndKeyPress();

		jvxData getDurationFromSliderValue(int value);

		int getSliderValueFromDuration(jvxData duration);

		void updateInputHandlerLoops();

		QTimer* clippingUpdateTimer;
		QTimer* clippingReleaseTimer;
		
		void setClippingIndicator();
		
		void setCurrentPlayTime(jvxData playTime);
		jvxData getCurrentPlayTime();
		jvxData lastCurrentPlayTimeInSec;

		jvxSize getNumberOfActiveOutputChannels();
		void turnOffDeviceInputChannels();


		QDir lastSelectFileDir;

		void updatePlaybackControlWidgets();

		Q_OBJECT

	public:

		myCentralWidget(QWidget* parent);
		~myCentralWidget();


		virtual void init_submodule(IjvxHost* theHost)override;

		virtual void inform_update_window(jvxCBitField prio)override;

		virtual void inform_update_window_periodic()override;

		virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

		virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

		virtual void inform_about_to_shutdown()override;

		virtual void inform_bootup_complete(jvxBool* wantsToAdjustSize)override;

		virtual void inform_sequencer_about_to_start()override;

		virtual void inform_sequencer_stopped()override;

		void attachAudioInputReferences();
		void detachAudioInputReferences();

		void turnOnAllInternalInputChannels();

		void turnOnAllDeviceOutputChannels();

		// override drag and drop function
		virtual void dragEnterEvent(QDragEnterEvent* e)override;
		virtual void dropEvent(QDropEvent* e)override;

		void connectButtons();
		void connectVolume();


		void updateFilenameLabelDisplay();
		void updateVolumeDisplay();


		QString createQStringForTime(jvxData durationInSec);

		void hideItems();
		void showItems();

		void resetHost();

		

		void startPlaying();

		void stopPlaying();

		void pausePlaying();

		void unpausePlaying();

	protected:
		virtual void keyPressEvent(QKeyEvent* event) override;

		virtual bool eventFilter(QObject *obj, QEvent *ev)override;

	signals:
		void buttonItemEvent(int buttonIndex);
		void buttonRemoveItemEvent(int itemIndex);
		void buttonFileItemEvent(int buttonIndex);
		void volumeSpinBoxEvent(int itemIndex);
		void volumeSliderEvent(int itemIndex);

	public slots:

		void buttonItemClicked(int buttonIndex);

		void volumeSpinboxChanged(int itemIndex);

		void volumeSliderChanged(int itemIndex);

		void buttonRemoveItemClicked(int itemIndex);

		void selectFileItem(int itemIndex);

		void buttonPlayPauseClicked();

		void buttonStopClicked();

		

		void button_hideShow_clicked();

		void shuffleTestItems();

		void updateTimeDisplay();

		void loopStartSliderMoved(int value);
		void loopEndSliderMoved(int value);
		void curPlayPosSliderMoved(int value);

		
		void updateClippingIndicator();
		void resetClippingIndicator();

		void updatePlayMonoAsStereoClicked(int state);

	};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
