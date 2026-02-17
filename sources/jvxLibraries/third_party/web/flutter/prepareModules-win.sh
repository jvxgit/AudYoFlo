#!/bin/bash
JVX_FLUTTER_VERSION=$1

echo Running download to receive flutter version ${JVX_FLUTTER_VERSION}
 
if [ ! -d "flutter-${JVX_FLUTTER_VERSION}" ]; then
	if [ ! -d "flutter" ]; then
		wget https://storage.googleapis.com/flutter_infra_release/releases/stable/windows/flutter_windows_${JVX_FLUTTER_VERSION}-stable.zip 
		unzip flutter_windows_${JVX_FLUTTER_VERSION}-stable.zip
	fi

	sleep 5

	max=10
	target="flutter-${JVX_FLUTTER_VERSION}"

	for ((i=1; i<=max; i++)); do
		if [ -d "$target" ]; then
			echo "Target '$target' already exists; done."
			break
		fi

		echo "Trying to mv flutter -> $target, run <$i>"

		if mv -T flutter "$target" 2>/dev/null; then
			echo "Rename succeeded."
			break
		else
			# optional: Fehlermeldung anzeigen
			echo "mv failed (run <$i>), retrying..." >&2
			sleep 5
		fi
	done
	
	rm flutter.zip
fi
