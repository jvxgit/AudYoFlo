include_directories(
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileReader/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileWriter/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTDataConverter/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTResampler/)
	
set(LOCAL_LIBS ${LOCAL_LIBS}
	jvxTrtAudioFileReader_static
	jvxTrtAudioFileWriter_static
	jvxAuTGenericWrapper_static
	jvxTDataConverter_static
	jvxTResampler_static)	
