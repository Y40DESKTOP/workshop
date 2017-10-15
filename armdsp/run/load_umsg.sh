#!/bin/sh

pname=${0%%load_umsg.sh}
driver=${pname}umsg.ko

if [[ $# -gt 0 ]]; then
    driver=${pname}$1
fi

set -x

insmod $driver
