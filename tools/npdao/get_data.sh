#!/bin/bash

[[ "$2" == "" || ! -f "$1" ]] && echo "Usage: $0 <cfg_file> <label>" && exit
DIR=`dirname $0`
DIR=`realpath $DIR`
BASE=$DIR/../..
SHCMD="$BASE/scripts/wfshell"
INVOKE_WF="$BASE/invoke_whitefield.sh"
WAIT_ELAP_TIME=120
SAMPLE_INTERVAL=20
SAMPLE_COUNT=60
NUM_OF_RUNS=5
CFG_FILE=`realpath $1`
LABEL="$2"

get_estimated_time()
{
    SECONDS=0
    est_time_sec=`echo "(($SAMPLE_INTERVAL*$SAMPLE_COUNT)+$WAIT_ELAP_TIME)*$NUM_OF_RUNS*2" | bc -q`
    est_time_hms=`printf '%02d:%02d:%02d' $(($est_time_sec/3600)) $(($est_time_sec%3600/60)) $(($est_time_sec%60))`
}

start_wf()
{
    $SHCMD stop_whitefield
    $INVOKE_WF $CFG_FILE
}

shift_node()
{
    [[ "$1" == "" ]] && echo "need an arg" && return
    org_pos=`$SHCMD cmd_node_position $1 | grep "loc=" | cut -d ' ' -f 3-6`
    $SHCMD cmd_set_node_position $1 1000 1000
    sleep 180
    $SHCMD cmd_set_node_position $1 $org_pos
}

chg_node_pos()
{
    sleep $WAIT_ELAP_TIME
    sleep 10
    $SHCMD cmd_set_node_position 
    TMP_FILE=def$$.txt
    for((i=0;i<8;i++)); do
        for n in `$SHCMD cmd_def_route  | grep "fe80::" | sed 's/.*://g'` ; do 
            printf "%d\n" 0x$n 
        done | sort -n | uniq -c | sort -n | tail -1 | tr -s ' ' | cut -d ' ' -f 3 > $TMP_FILE
        nid=`cat $TMP_FILE`
        rm $TMP_FILE
        shift_node $nid
    done
}

take_data()
{
    [[ "$1" == "" ]] && echo "take_data requires label as input" && exit
    for((i=0;i<$NUM_OF_RUNS;i++)); do
        start_wf
        #chg_node_pos &
        prn_time_info
        $BASE/scripts/get_connectivity_snapshot.sh -e $WAIT_ELAP_TIME -i $SAMPLE_INTERVAL -c $SAMPLE_COUNT -o $DIR/$LABEL/$1$i.csv
    done
    wait
}

set_dco_conf()
{
    [[ ! "$1" =~ ^0$|^1$ ]] && echo "Need to pass with 0 or 1" && exit
    TMP_FILE=/tmp/prj_conf_$$.h
    PROJECT_CONF=$BASE/thirdparty/contiki/examples/ipv6/rpl-udp/project-conf.h
    sed -e "s/^#.*define.*RPL_CONF_WITH_DCO.*$/#define RPL_CONF_WITH_DCO $1/g" $PROJECT_CONF > $TMP_FILE
    cp $TMP_FILE $PROJECT_CONF
    rm $TMP_FILE
    make -C $BASE contiki_clean
    make -C $BASE
    [[ $? -ne 0 ]] && echo "make failed!!" && exit
}

prn_time_info()
{
    echo "Estimated Time: Sec=${est_time_sec} HH:MM:SS=$est_time_hms curr_exec_time=$SECONDS"
}

get_estimated_time
mkdir -p $DIR/$LABEL 2>/dev/null
cp $CFG_FILE $DIR/$LABEL/

set_dco_conf 1
take_data "dco_"

set_dco_conf 0
take_data "npdao_"

