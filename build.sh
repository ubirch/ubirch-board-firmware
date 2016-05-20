#! /bin/sh
TOOLCHAIN=../ubirch-arm-toolchain/cmake/ubirch-arm-gcc-toolchain.cmake
BOARD="ubirch1r02"
# do an out-of-source build for all configurations
if [ "$1" == "-a" ]
then BUILDS="Debug Release MinSizeRel RelWithDebInfo"
else BUILDS=MinSizeRel
fi
[ -d "build/$BOARD" ] && rm -r "build/$BOARD"
for BUILD_TYPE in $BUILDS
do
  (mkdir -p "build/$BOARD/$BUILD_TYPE"; cd "build/$BOARD/$BUILD_TYPE"; \
  cmake ../../.. -DCMAKE_TOOLCHAIN_FILE="../../../$TOOLCHAIN" \
	-DBOARD="$BOARD" \
	-DCMAKE_BUILD_TYPE="$BUILD_TYPE"; \
  make)
done
