#! /bin/sh
set -e

desktop-file-install --dir="${DESTDIR}/${1}" \
     "${MESON_SOURCE_ROOT}/data/poki-pona.desktop"
