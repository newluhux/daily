#!/bin/sh

POS=$(dirname $(realpath $0))

guix system vm $POS/testvm.scm --no-graphic --expose=$POS/=/src > $POS/vmstart.sh

sh $POS/vmstart.sh
