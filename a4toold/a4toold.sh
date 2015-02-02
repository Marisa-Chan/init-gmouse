#!/bin/bash
# Writed by TimofeyTitovets <nefelim4ag@gmail.com> 2014

################################################################################
# a4tool daemon, for checking battery state
# Setting values duplicated in script for more clarity
################################################################################
export MB_FIFO=(100 100)  KB_FIFO=(100 100)
battery_check() { # $1 device name
    case $1 in
        Mouse)    FIFO=(${MB_FIFO[@]}) ;;
        Keyboard) FIFO=(${KB_FIFO[@]}) ;;
    esac
    FIFO=( ${FIFO[@]} `a4tool bat | grep -i $1 | tr -d '[%,a-z,A-Z,:, ]'` )
    if [ ! -z ${FIFO[$B_FIFO_S]} ]; then # if size > max, del first element
        FIFO[0]="" FIFO=(${FIFO[@]})
    fi
    case $1 in
        Mouse)    MB_FIFO=(${FIFO[@]}) ;;
        Keyboard) KB_FIFO=(${FIFO[@]}) ;;
    esac
    BL_A=0
    for n in ${FIFO[@]}; do
        BL_A=$[$BL_A+$n];
    done;
    BL_A=$[$BL_A/${#FIFO[@]}]

    echo  $BL_A > $work_dir/$1/bat &
    echo `a4tool mrr get  | awk '{print $3}'`        > $work_dir/mrr &
    echo `a4tool siglevel | tr -d '[%,a-z,A-Z,:, ]'` > $work_dir/siglevel &
}

. /etc/a4toold.conf || exec echo Config not exist

for n in Keyboard Mouse; do
    mkdir -p $work_dir/$n/
done

while true; do
    for dev in Mouse Keyboard; do
        dev=`a4tool bat | grep -i $dev | grep -e '[0-9]'`
        [ ! -z "$dev" ] && battery_check $dev
    done
    sleep $B_FIFO_P
done &

################################################################################
notify(){ # $1 - name of the device (Mouse | Keyboard)
    if [ -f $work_dir/$n/bat ]; then
        BL=`cat $work_dir/$n/bat`
    else
        return 0
    fi
    for val in ${notify_levels[@]}; do
        if [ "$BL" -le "$val" ]; then
            if [ ! -f $work_dir/$1/NC_$val ]; then
                echo 0 > $work_dir/$1/NC_$val
            fi
            NC=`cat $work_dir/$1/NC_$val`  # NC - notify counts
            if [[ $NC -le $NC_max ]]; then
                echo $[$NC+1] > $work_dir/$1/NC_$val &
                notify-send -i /usr/share/a4tool/$1.png "$1 battery level ${BL}% less than: ${val}%" &
                break
            fi
        else
            [ -f $work_dir/$1/NC_$val ] && rm -v $work_dir/$1/NC_$val
        fi
    done
}

while true; do
    for n in Keyboard Mouse; do
        notify $n &
    done
    sleep $NOTIFY_PAUSE
done