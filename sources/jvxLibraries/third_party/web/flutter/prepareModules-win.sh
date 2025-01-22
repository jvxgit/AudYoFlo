#!/bin/bash
if [ ! -d "flutter-${JVX_FLUTTER_VERSION}" ]; then
	if [ ! -d "flutter" ]; then
		curl -kLSs  https://storage.googleapis.com/flutter_infra_release/releases/stable/windows/flutter_windows_${JVX_FLUTTER_VERSION}-stable.zip -o flutter.zip
		unzip flutter.zip
	fi

	sleep 5

	max=10
	for i in {1..$max}
	do
		if [ ! -d "flutter-${JVX_FLUTTER_VERSION}" ]; then
			echo "Trying to mv flutter to flutter-${JVX_FLUTTER_VERSION}, rum <$i>"
			mv flutter flutter-${JVX_FLUTTER_VERSION}
			if [ ! -d "flutter-${JVX_FLUTTER_VERSION}" ]; then
				sleep 5
			fi
		fi
	done

	rm flutter.zip
fi
