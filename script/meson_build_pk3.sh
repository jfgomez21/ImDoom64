#!/bin/sh
set -e

MESON_SOURCE_ROOT=$1
MESON_BUILD_ROOT=$2

cd "${MESON_SOURCE_ROOT}/data/imdoom64.pk3"
zip -r "${MESON_BUILD_ROOT}/imdoom64.pk3" .
