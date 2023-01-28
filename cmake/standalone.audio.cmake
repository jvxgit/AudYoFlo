header_message("Configuring audio submodule")

# include platform specific stuff
include(${JVX_SUBPRODUCT_ROOT}/cmake/${JVX_OS}.${JVX_PRODUCT}.cmake)

include_directories(
  ${JVX_SUBPRODUCT_ROOT}/software
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-product/include
  )
  
###
# configure packages specific to rtproc subproduct
###

# configure libsndfile
#find_sndfile()

# configure platform specific stuff (e.g. alsa)
find_platform_specific()

