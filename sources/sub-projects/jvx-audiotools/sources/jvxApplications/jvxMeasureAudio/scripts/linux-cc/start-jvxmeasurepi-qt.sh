#!/bin/sh
export LD_LIBRARY_PATH=./bin/:/home/hauke/work/rpi/sdk/rpi64-qt5/sysroots/aarch64-poky-linux/usr/lib:$LD_LIBRARY_PATH

## export QT_LOGGING_RULES=qt.qpa.*=true
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_KMS_ATOMIC=1

## export QT_QPA_PLATFORM=linuxfb
export QT_QPA_FONTDIR=/usr/share/fonts/truetype
export QT_QPA_EGLFS_INTEGRATION=eglfs_kms
export QML_DISABLE_DISK_CACHE=1
export QT_QPA_EGLFS_ALWAYS_SET_MODE=1

bin/jvxMeasureAudio --jvxdir jvxComponents --config jvxmeasurepi.jvx --textlog --verbose_dll --textloglev 10
