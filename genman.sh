#!/bin/bash

# genman.sh builds application and (re-)generates its man-page

PROGPATH="./nview"
if [[ "$(uname)" == "Darwin" ]]; then
  PROGPATH="./nview.app/Contents/MacOS/nview"
fi

qmake && make -s && \
help2man -n "nano view" -N -o res/nview.1 ${PROGPATH}
exit ${?}
