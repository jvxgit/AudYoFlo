set(JVX_DETECT_PEAKS_STATIC_LIBS
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-detect-peaks
  )

if(JVX_RELEASE_ARMCORTEXM4)
    set(JVX_DETECT_PEAKS_STATIC_LIBS ${JVX_DETECT_PEAKS_STATIC_LIBS}
       ${CMAKE_CURRENT_SOURCE_DIR}/platform-packages/arm_cortex_m4/jvx-detect-peaks)
endif()

jvx_addSubdirs("Configuring static library for peak detector core functionality" "${JVX_DETECT_PEAKS_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-detect-peaks")
