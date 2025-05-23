if(JVX_USE_QT)
	if(NOT JVX_COMPILE_BUILDTOOLS)
		if(NOT JVX_RELEASE_SDK AND NOT JVX_USES_SDK)
		
			#install(DIRECTORY ${QT_INSTALL_PATH}/bin/ DESTINATION ${INSTALL_PATH_RUNTIME_LIBS} FILES_MATCHING PATTERN "*.dll")
			# We must be careful to copy only a subset of the QT dlls: It may be that we use another qt lib on the
			# development machine and then forget to copy the dll. In that case, program will run on development machine
			# but will fail at the customers machine
			set(JVX_QT_FILE_EXTENSION "")
			
					set(JVX_QT_FILE_EXTENSION "d")
			
				install(FILES
					${QT_INSTALL_PATH}/bin/Qt5Core${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Gui${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Widgets${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5PrintSupport${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Svg${JVX_QT_FILE_EXTENSION}.dll
					DESTINATION ${INSTALL_PATH_LIB_SHARED}
					CONFIGURATIONS Debug UnstableDebug 
					COMPONENT qtdllsmin)
				
				install(FILES
					${QT_INSTALL_PATH}/bin/libGLESv2${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/libEGL${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Concurrent${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5OpenGL${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Gamepad${JVX_QT_FILE_EXTENSION}.dll				
					${QT_INSTALL_PATH}/bin/Qt5Network${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Websockets${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DAnimation${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DCore${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DExtras${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DInput${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DLogic${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuick${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickAnimation${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickExtras${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickInput${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickRender${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickScene2D${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DRender${JVX_QT_FILE_EXTENSION}.dll				
				DESTINATION ${INSTALL_PATH_LIB_SHARED}
				CONFIGURATIONS Debug UnstableDebug 
				COMPONENT qtdlls)
			
				install(FILES
					${QT_INSTALL_PATH}/bin/Qt5Core.dll
					${QT_INSTALL_PATH}/bin/Qt5Gui.dll
					${QT_INSTALL_PATH}/bin/Qt5Widgets.dll
					${QT_INSTALL_PATH}/bin/Qt5PrintSupport.dll
					${QT_INSTALL_PATH}/bin/Qt5Svg.dll
					DESTINATION ${INSTALL_PATH_LIB_SHARED}
					CONFIGURATIONS Release UnstableRelease
					COMPONENT qtdllsmin)
				
				install(FILES
					${QT_INSTALL_PATH}/bin/libGLESv2.dll
					${QT_INSTALL_PATH}/bin/libEGL.dll
					${QT_INSTALL_PATH}/bin/Qt5Concurrent.dll
					${QT_INSTALL_PATH}/bin/Qt5OpenGL.dll
					${QT_INSTALL_PATH}/bin/Qt5Gamepad.dll				
					${QT_INSTALL_PATH}/bin/Qt5Network.dll
					${QT_INSTALL_PATH}/bin/Qt5Websockets.dll
					${QT_INSTALL_PATH}/bin/Qt53DAnimation.dll
					${QT_INSTALL_PATH}/bin/Qt53DCore.dll
					${QT_INSTALL_PATH}/bin/Qt53DExtras.dll
					${QT_INSTALL_PATH}/bin/Qt53DInput.dll
					${QT_INSTALL_PATH}/bin/Qt53DLogic.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuick.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickAnimation.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickExtras.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickInput.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickRender.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickScene2D.dll
					${QT_INSTALL_PATH}/bin/Qt53DRender.dll				
				DESTINATION ${INSTALL_PATH_LIB_SHARED}
				CONFIGURATIONS Release UnstableRelease
				COMPONENT qtdlls)
			#install(FILES
			#	${QT_INSTALL_PATH}/plugins/platforms/qminimald.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qoffscreend.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qwindowsd.dll
			#	DESTINATION ${INSTALL_PATH_BIN}/platforms
			#	CONFIGURATIONS Debug UnstableDebug)

			#install(FILES
			#	${QT_INSTALL_PATH}/plugins/platforms/qminimal.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qoffscreen.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qwindows.dll
			#	DESTINATION ${INSTALL_PATH_BIN}/platforms
			#	CONFIGURATIONS Release UnstableRelease)
				
			#
			set(JVX_QT_PLUGINS_DIR_COPY "platforms;styles;imageformats")
			jvx_install_qt_plugins(${QT_INSTALL_PATH} ${INSTALL_PATH_BIN}/plugins "JVX_QT_PLUGINS_DIR_COPY")			
		endif()
	endif()
endif()