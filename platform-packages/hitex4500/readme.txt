Here is what must be done to build the Cortex M4 processing:
================================================================
1) This project lies in "O:\jvx\audio\platform-packages\hitex4500\jvx-engine-pkg", hence the path
JVX_BASE_ROOT points to "O:\jvx\base" on my system.

2) The "lpcOpen" sdk folder is part of the Javox GIT lab repo. You can get it here:
git@gitlab.javox-solutions.com:hk/nxp-hitex-4500.git
I have checkout it out to folder S:\nxp\lpcOpenSdks

3) In order to build the application, you need to create a workspace folder somewhere.
lpcXPresso is supposed to be started in that folder. In the second step, the following projects must
be imported:
	- S:\nxp\lpcOpenSdks\lpcopen_2_19_lpcxpresso_hitex_4350-patched\lpc_board_hitex_eva_4350
	- S:\nxp\lpcOpenSdks\lpcopen_2_19_lpcxpresso_hitex_4350-patched\lpc_chip_43xx
	- O:\jvx\base\platform-packages\hitex4500\jvx-helpers\project
	- O:\jvx\base\platform-packages\hitex4500\jvx-dsp-base\project
	- O:\jvx\audio\platform-packages\hitex4500\jvx-dsp\project
	- O:\jvx\audio\platform-packages\hitex4500\jvx-engine-pkg

The projects must be built one after the other. jvx_audio is the project stored in folder jvx-engine-pkg and
requires all other libs to be linked in The link references happen via the "same workspace" mechanism.

For the header files, the JVX_BASE_ROOT system environement variable is used. The lpcOpen SDK is referenced via
workpace knowledge.

HK
