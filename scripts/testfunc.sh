#!/bin/bash
runtest() {
    echo "--------------------------------------Func Test Report--------------------------------------" >>"$1"
    ac=0
    wa=0
    re=0
    ce=0
    ac_arr=()
    wa_arr=()
    re_arr=()
    ce_arr=()
    for file in ./func/asm/*.s; do
        [ -e "$file" ] || continue
        tail="${file#*asm/}"
        key="${tail%.*}"
        gcc -march=armv7-a -c "$file" -o "${file%.*}".o
        # &
        # {
        #     sleep 0.5
        #     kill $! &
        # }
        gcc "${file%.*}".o ./include/sylib.a -o ./func/bin/"$key"
        infile=./func/in/"$key".in
        outfile=./func/res/"$key".out
        ignore_list=()
        id="${key%%_*}"
        flag=0
        if [ -e "$infile" ]; then
            for value in "${ignore_list[@]}"; do
                [[ "$id" = "$value" ]] && ((flag = 1))
            done
            if [ $flag != 1 ]; then
                ./func/bin/"$key" <"$infile" >"$outfile"
                ret=$?
                test -n "$(tail $outfile -c 1)"
                if [ $? = 0 ]; then
                    echo "" >>"$outfile"
                fi
                echo "$ret" >>"$outfile"
            fi
        else
            for value in "${ignore_list[@]}"; do
                [[ "$id" = "$value" ]] && ((flag = 1))
            done
            if [ $flag != 1 ]; then
                ./func/bin/"$key" >"$outfile"
                ret=$?
                test -n "$(tail $outfile -c 1)"
                if [ $? = 0 ]; then
                    echo "" >>"$outfile"
                fi
                echo "$ret" >>"$outfile"
            fi
        fi
        echo -e "\033[1;34m$key\033[0m" >>"$1"
        if [ -e "$outfile" ]; then
            if ! diff "$outfile" ./func/out/"$key".out >/dev/null; then
                grep 127 "$outfile" || grep 139 "$outfile"
                if [ $? = 0 ]; then
                    echo -e "\033[1;33mRuntime Error!\033[0m" >>"$1"
                    echo "Diff INFO:" >>"$1"
                    diff "$outfile" ./func/out/"$key".out >>"$1"
                    ((re_arr[re] = id)) || true
                    ((re += 1)) || true
                else
                    echo -e "\033[1;31mWrong Answer!\033[0m" >>"$1"
                    echo "Diff INFO:" >>"$1"
                    diff "$outfile" ./func/out/"$key".out >>"$1"
                    ((wa_arr[wa] = id)) || true
                    ((wa += 1)) || true
                fi
            else
                echo -e "\033[1;32mAccept!\033[0m" >>"$1"
                ((ac_arr[ac] = id)) || true
                ((ac += 1)) || true
            fi
        else
            echo -e "\033[1;33mRuntime Error!\033[0m" >>"$1"
            ((re_arr[re] = id)) || true
            ((re += 1)) || true
        fi
    done
    ((ce = 140 - ac - wa - re)) || true
    echo "----------------------------------------Test Result----------------------------------------" >>"$1"
    echo -n "Final Score: " >>"$1"
    echo "scale=2;$ac/1.4"|bc >>"$1"
    echo -e "\033[1;32mAC:$ac\033[0m" >>"$1"
    echo -e "\033[1;31mWA:$wa\033[0m" >>"$1"
    echo -e "\033[1;33mRE:$re\033[0m" >>"$1"
    echo -e "\033[1;34mCE:$ce\033[0m" >>"$1"
    echo "------------------------------------------Test Log------------------------------------------" >>"$1"
    if [ $ce = 0 ]; then
        echo -e "\033[1;32mCongratulations! You don't have any compiler errors!\033[0m" >>"$1"
    else
        echo -e "\033[1;31mUnfortunately! You have some compiler errors!\033" >>"$1"
        tmpce=0
        for tmpfile in ./func/out/*.out; do
            [ -e "$tmpfile" ] || continue
            tmptail="${tmpfile#*out/}"
            tmpkey="${tmptail%.*}"
            tmpid="${tmpkey%%_*}"
            if [ ! -e ./func/asm/"$tmpkey".s ]; then
                ((ce_arr[tmpce] = tmpid)) || true
                ((tmpce += 1)) || true
            fi
        done
        echo -e "\033[1;34mThere are: ${ce_arr[*]}\033[0m" >>"$1"
    fi
    if [ $re = 0 ]; then
        echo -e "\033[1;32mCongratulations! You don't have any runtime errors!\033[0m" >>"$1"
    else
        echo -e "\033[1;31mUnfortunately! You have some runtime errors!\033" >>"$1"
        echo -e "\033[1;34mThere are: ${re_arr[*]}\033[0m" >>"$1"
    fi
    if [ $wa = 0 ]; then
        echo -e "\033[1;32mCongratulations! You don't have any wrong answers!\033[0m" >>"$1"
    else
        echo -e "\033[1;31mUnfortunately! You have some wrong answers!\033" >>"$1"
        echo -e "\033[1;34mThere are: ${wa_arr[*]}\033[0m" >>"$1"
    fi
}
cd test || exit
rm report
rm -rf ./func/bin
rm -rf ./func/res
mkdir ./func/bin
mkdir ./func/res
touch report
runtest report
rm -rf ./func/asm
cat report
