#!/bin/bash
gen() {
    for file in ../test/func/sy/*.sy; do
        [ -e "$file" ] || continue
        head="${file%sy/*}"
        tail="${file#*sy/}"
        ../build/compiler -S -o "$head"asm/"${tail%.*}".s "$file" 
        # &
        # {
        #     sleep 1
        #     kill $! &
        # }
    done
}
rm -rf ../test/func/asm
mkdir -p ../test/func/asm
gen
./mvfunc2pi.sh
