#!/bin/bash
if [ ! -d "flutter-${JVX_FLUTTER_VERSION}" ]; then
	curl -kLSs  https://storage.googleapis.com/flutter_infra_release/releases/stable/linux/flutter_linux_${JVX_FLUTTER_VERSION}-stable.tar.xz -o flutter.tar.xz
	xz -d flutter.tar.xz
	tar -xf flutter.tar
	sleep 1
	mv flutter flutter-${JVX_FLUTTER_VERSION}
	rm flutter.tar
fi
