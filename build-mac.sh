#!/bin/sh

if ! [ -f /usr/local/bin/packagesbuild ]; then
	echo Please install the "packages" tool from http://s.sudre.free.fr/Software/Packages/about.html
	exit 2
fi

if [ -z "$QTDIR" ]; then
	QTDIR=~/Qt/5.1.1/clang_64
fi

if ! [ -f $QTDIR/bin/qmake ]; then
	echo qmake not found in QTDIR \($QTDIR\)
	exit 5
fi

s="$0"

d=`dirname "$s"`

cd "$d"

(
	cd ../pfs
	sh compile-mac
)

(
	cd pCloud
	if [ -f Makefile ]; then
		make distclean
	fi
	$QTDIR/bin/qmake
	make
	$QTDIR/bin/macdeployqt pCloud.app
	cd ..
)

/usr/local/bin/packagesbuild  -v  macpkg/pCloud.pkgproj

echo package is in $d/macpkg/build/pCloud.mpkg/Contents/Packages/pCloud.pkg

