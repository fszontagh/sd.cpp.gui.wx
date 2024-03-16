#!bin/bash

#$1 version

SD_VERSION="$1"
SD_DESCRIPTION="Stable Diffusion C++ Desktop Gui"

SD_BASEDIR="./build/"
SD_SUFFIX="-${SD_VERSION}-amd64"

#######################################################################
# THE BINARY

# create directory structure

PACKNAME="sd.ui${SD_SUFFIX}"
PACKPATH="${SD_BASEDIR}${PACKNAME}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi


mkdir -p ${PACKPATH}/DEBIAN 
mkdir -p ${PACKPATH}/usr/{lib,bin,share}
mkdir -p ${PACKPATH}/usr/share/{pixmaps,applications}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: sd.cpp.ui
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
Depends: libsdcpp-avx | libsdcpp-avx2 | libsdcpp-avx512 | libsdcpp-cuda | libsdcpp-rocm, libexiv2-27, libcurl4, libcurlpp0, libgtk-3-0, libwxbase3.2-1, libwxgtk-gl3.2-1, libwxgtk-media3.2-1, libwxgtk-webview3.2-1, libwxgtk3.2-1
EOF

cp -p dist/all/sd.ui ${PACKPATH}/usr/bin/
cp -p dist/all/sd.ui.desktop ${PACKPATH}/usr/share/applications
cp -p dist/all/cube_256.png ${PACKPATH}/usr/share/pixmaps/sd.ui.png

dpkg-deb --root-owner-group --build ${PACKPATH}

#######################################################################
# THE LIBS


# CUDA 

PACKNAME="libsdcpp-cuda"
PACKPATH="${SD_BASEDIR}${PACKNAME}${SD_SUFFIX}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi

mkdir -p ${PACKPATH}/{DEBIAN,usr/lib}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: ${PACKNAME}
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
Depends: libcudart11.0, libcuda1, libnvidia-compute
EOF


cp -p dist/all/stable-diffusion_cuda.so ${PACKPATH}/usr/lib/

dpkg-deb --root-owner-group --build ${PACKPATH}

# THE AVX

PACKNAME="libsdcpp-avx"
PACKPATH="${SD_BASEDIR}${PACKNAME}${SD_SUFFIX}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi

mkdir -p ${PACKPATH}/{DEBIAN,usr/lib}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: ${PACKNAME}
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
EOF


cp -p dist/all/stable-diffusion_avx.so ${PACKPATH}/usr/lib/

dpkg-deb --root-owner-group --build ${PACKPATH}

# THE AVX2

PACKNAME="libsdcpp-avx2"
PACKPATH="${SD_BASEDIR}${PACKNAME}${SD_SUFFIX}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi

mkdir -p ${PACKPATH}/{DEBIAN,usr/lib}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: ${PACKNAME}
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
EOF


cp -p dist/all/stable-diffusion_avx2.so ${PACKPATH}/usr/lib/

dpkg-deb --root-owner-group --build ${PACKPATH}

# THE AVX512

PACKNAME="libsdcpp-avx512"
PACKPATH="${SD_BASEDIR}${PACKNAME}${SD_SUFFIX}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi

mkdir -p ${PACKPATH}/{DEBIAN,usr/lib}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: ${PACKNAME}
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
EOF


cp -p dist/all/stable-diffusion_avx512.so ${PACKPATH}/usr/lib/

dpkg-deb --root-owner-group --build ${PACKPATH}


# THE ROCM

PACKNAME="libsdcpp-rocm"
PACKPATH="${SD_BASEDIR}${PACKNAME}${SD_SUFFIX}"

echo "Creating ${PACKNAME} in ${PACKPATH}"

if [ -d "${PACKPATH}" ]; then
  rm -rf ${PACKPATH}
fi

mkdir -p ${PACKPATH}/{DEBIAN,usr/lib}

cat << EOF > ${PACKPATH}/DEBIAN/control
Package: ${PACKNAME}
Version: ${SD_VERSION}
Maintainer: Ferenc Szontagh <szf@fsociety.hu>
Architecture: amd64
Description: ${SD_DESCRIPTION}
EOF


cp -p dist/all/stable-diffusion_rocm.so ${PACKPATH}/usr/lib/


dpkg-deb --root-owner-group --build ${PACKPATH}