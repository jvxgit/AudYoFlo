# ============================================
# Run build script for standalone project
# ============================================

JVX_GENERATOR_TOKEN="RTAP_RPL_GENERATOR_TOKEN"
# "Unix Makefiles"
# "Eclipse CDT4 - Unix Makefiles"

JVX_SDK_PATH="RTAP_RPL_SDK_PATH"

echo "=============================="
echo "RTProc SDK path=%JVX_SDK_PATH%"
echo "RTProc cmake generator token=%JVX_GENERATOR_TOKEN%"
echo "=============================="

# ============================================
cmake -DCMAKE_BUILD_TYPE=Debug .. \
      -G "${JVX_GENERATOR_TOKEN}" -Wno-dev \
      -DJVX_SDK_RUNTIME_PATH="${JVX_SDK_PATH}"/runtime \
      -DJVX_SDK_PATH="${JVX_SDK_PATH}"/sdk \
      -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE 




