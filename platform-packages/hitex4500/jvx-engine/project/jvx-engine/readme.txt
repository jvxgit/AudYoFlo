Building and running the project für HiTex Whistle (jvx-engine-pkg):
====================================================================

1) The project is located in the platform-packages subfolder in the jvx_rt (rtproc) git folder, e.g.,
   "O:\jvx\audio\platform-packages\hitex4500\jvx-engine-pkg". You need to create a workspace somewhere.
   I did it here: "O:\jvx\build\project_ucui\ws-nxp"

   At first, the project "jvx-engine-pkg" must be imported. This causes me to specify a general hint:

   !!!!!!!!!!ALWAYS USE THE NXP SHORTCUTS IN THE QUICKSTART PANEL IN THE LEFT LOWER CORNER!!!!!!!!!

   (if you do not use those, unexpected errors may be the result).
2) When compiling, you will see missing header files (board.h) which result from a missing link to the 
   Hitex4500 SDK. That SDK is stored under git at "git@gitlab.javox-solutions.com:hk/nxp-hitex-4500.git".
   You need to get that package and compile the libs first (-> "How to get started" in readme.txt in git repo). 
   Once done, the location must be setup in two environment variables:
   - Variable LPCOPEN_DSP_PATH to point to the soure code, e.g., 
	 LPCOPEN_DSP_PATH = S:\nxp\lpcOpenSdks\lpcopen_2_19_lpcxpresso_hitex_4350-patched
   - Variable LPCOPEN_DSP_WS_PATH to point to the project workspace to find the compiled libraries, e.g.,
     LPCOPEN_DSP_WS_PATH = S:\nxp\lpcOpenSdks\ws
	 
3) When building was successful, the debugger must be started. The debugger interface which I used is lpcLinkII which requires
   installation of a dedicated driver. And then:

   !!!!!!!!!! USE THE NXP SHORTCUTS IN THE QUICKSTART PANEL IN THE LEFT LOWER CORNER TO DEBUG!!!!!!!!!

4) Once up and running, the LINE IN should be output on the LINE OUT as well as the HEADPHONE out.

5) While operating, an RS-232 connector can be connected to output printf statements. Connection detail is:
   Rate =  115.200, 8 Bit, 1 Stop Bit no flow control
   

	
