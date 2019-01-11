#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

TOPDIR="${DIR}/../"
cd ${TOPDIR}
export LD_LIBRARY_PATH=${TOPDIR}/build/src

$@

