#!/bin/sh
# Builds the static libcurl for the Windows (MinGW) builds of Q3Rally.
#
# TLS goes through Windows Schannel (system certificate store), so the
# library needs no other libraries than Windows' own. Only the protocols the
# engine uses are built in: HTTP(S) for the ladder and downloads, FTP(S) for
# old download URLs.
#
# Used by the GitHub Actions build (MSYS2 MINGW64/MINGW32 shell). Works the
# same with a MinGW cross compiler on Linux (third argument, e.g.
# x86_64-w64-mingw32-).
#
#   engine/misc/build-curl-mingw.sh <x86_64|x86> <install dir> [cross prefix]
#   make -C engine release PLATFORM=mingw32 ARCH=x86_64 CURL_PREFIX=<install dir>
#
# Updating curl: set CURL_VERSION and CURL_SHA256 (sha256 of the .tar.xz from
# https://curl.se/download/ or the GitHub release; signed by Daniel Stenberg,
# key 27EDEAF22F3ABCEB50DB9A125CC908FDB71E12C2) and replace the headers in
# engine/code/curl-<version>/include/curl with the ones of the new release.
set -eu

CURL_VERSION=8.22.0
CURL_SHA256=f7ef3ae8a22e521f289803fe93543eb64c329b58aa73a9e224dfd915a2a5f4f7

if [ $# -lt 2 ]; then
	sed -n '2,20p' "$0"
	exit 1
fi
ARCH=$1
PREFIX=$2
CROSS=${3:-}

case "$ARCH" in
	x86_64|x86) ;;
	*) echo "unknown arch $ARCH" >&2; exit 1 ;;
esac

if [ -f "$PREFIX/lib/libcurl.a" ] && [ -f "$PREFIX/curl-version" ] &&
   [ "$(cat "$PREFIX/curl-version")" = "$CURL_VERSION-$ARCH" ]; then
	echo "libcurl $CURL_VERSION ($ARCH) already in $PREFIX"
	exit 0
fi

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
TAG=curl-$(echo "$CURL_VERSION" | tr . _)
curl -fsSL -o "$WORK/curl.tar.xz" \
	"https://github.com/curl/curl/releases/download/$TAG/curl-$CURL_VERSION.tar.xz"
echo "$CURL_SHA256  $WORK/curl.tar.xz" | sha256sum -c -
tar -xJf "$WORK/curl.tar.xz" -C "$WORK"

set --
if [ -n "$CROSS" ]; then
	set -- -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER="${CROSS}gcc" -DCMAKE_RC_COMPILER="${CROSS}windres"
fi
if command -v ninja >/dev/null 2>&1; then
	set -- "$@" -G Ninja
fi

cmake -S "$WORK/curl-$CURL_VERSION" -B "$WORK/build" "$@" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX="$PREFIX" \
	-DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON \
	-DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF \
	-DBUILD_LIBCURL_DOCS=OFF -DBUILD_MISC_DOCS=OFF -DENABLE_CURL_MANUAL=OFF \
	-DCURL_USE_SCHANNEL=ON -DCURL_USE_OPENSSL=OFF \
	-DCURL_USE_LIBPSL=OFF -DCURL_USE_LIBSSH2=OFF -DCURL_USE_LIBSSH=OFF \
	-DUSE_NGHTTP2=OFF -DUSE_LIBIDN2=OFF -DUSE_WIN32_IDN=OFF \
	-DCURL_ZLIB=OFF -DCURL_BROTLI=OFF -DCURL_ZSTD=OFF -DENABLE_UNICODE=OFF \
	-DCURL_DISABLE_LDAP=ON -DCURL_DISABLE_LDAPS=ON -DCURL_DISABLE_DICT=ON \
	-DCURL_DISABLE_GOPHER=ON -DCURL_DISABLE_IMAP=ON -DCURL_DISABLE_MQTT=ON \
	-DCURL_DISABLE_POP3=ON -DCURL_DISABLE_RTSP=ON -DCURL_DISABLE_SMB=ON \
	-DCURL_DISABLE_SMTP=ON -DCURL_DISABLE_TELNET=ON -DCURL_DISABLE_TFTP=ON \
	-DCURL_DISABLE_FILE=ON
cmake --build "$WORK/build"
cmake --install "$WORK/build"
echo "$CURL_VERSION-$ARCH" > "$PREFIX/curl-version"
echo "libcurl $CURL_VERSION ($ARCH) installed in $PREFIX"
