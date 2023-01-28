#import <Foundation/Foundation.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
//#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVAudioSession.h>

@interface CjvxIosAudioProcessorCore : NSObject
{
	// Audio SubSystem
	/*
  AudioComponentDescription	myAudioDesc;
	AudioComponent				myAudioComponent;
	AudioStreamBasicDescription myAudioFormat;
	AudioComponentInstance		myAudioUnit;
	AudioBufferList*			myAudioBufferList;


	// Format
	long				_sampleRate;
	long				_bufferSize;
	int					_bitsPerChannel;
	int					_channelsOut;
	int					_channelsIn;
	rtpDataFormat		_format;

	// Host-Referenz
	CrtpIosHost			* myHost;
  */
	AVAudioSession* myAudioSession;
}

- (BOOL)initialize;
- (BOOL)terminate;
- (void)beginInterruption;
- (void)endInterruption;
/*
- (void) setAudioProcessorSamplerate:(long)sampleRate;
- (void) setAudioProcessorBuffersize:(long)bufferSize;
- (void) setAudioProcessorBitsPerChannel:(int)bitsPerChannel;
- (void) setAudioProcessorInputChannels:(int)input;
- (void) createBufferList;
- (void) createAudioUnit;
- (void) closeAudioUnit;
- (void) setHost:(CrtpIosHost*)host;
*/

@end
