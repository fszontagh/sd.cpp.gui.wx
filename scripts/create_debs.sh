#!bin/bash

#$1 version

SD_VERSION="$1"
SD_DESCRIPTION="Stable Diffusion C++ Desktop Gui"

SD_BASEDIR="./dist/"
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
Depends: libsdcpp-avx | libsdcpp-avx2 | libsdcpp-avx512 | libsdcpp-cuda | libsdcpp-rocm, libexiv2-27, libcurl4, libgtk-3-0
EOF

cat << EOF > ${PACKPATH}/usr/share/applications/sd.ui.desktop
[Desktop Entry]
Version=${SD_VERSION}
Type=Application
Categories=Application;Education
Terminal=false
Exec=/usr/bin/sd.ui
Path=/usr/bin
Name=Stable Diffusion Gui
Comment=${SD_DESCRIPTION}
Icon=/usr/share/pixmaps/sd.ui.png
EOF

cp -p dist/all/sd.ui ${PACKPATH}/usr/bin/
cp -p platform/linux/sd_ui.png ${PACKPATH}/usr/share/pixmaps/sd.ui.png

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