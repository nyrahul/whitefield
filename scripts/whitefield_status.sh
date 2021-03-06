#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
BR_ID=${1-0}

function msgq_status()
{
	ipcs -q
}

function get_route_list()
{
	rtsize=`sl_cmd "$BR_ID:cmd_rtsize"`
	echo "Route entries on node $BR_ID: $rtsize"
}

function main()
{
	elap_time
	echo "Simulation elapsed time: $wf_elap_time"

	#msgq_status

	get_node_list
	echo "Node count: $nodecnt"
	[[ $dead_nodecnt -gt 0 ]] && echo "ALARM: $dead_nodecnt NODES DEFUNCT! CHECK STACKLINE."
	get_route_list
	echo ;

	al_cmd "cmd_mac_stats"
	echo ;
	echo ;

	str=`uptime`
	pcpu=`ps -h -p $wfpid -o "%C"`
	memusage=`pmap $wfpid | tail -1 | awk '{print $2}'`
	echo "System load avg:${str/*:/}"
	echo "Airline CPU:$pcpu%, Memory:$memusage"
	echo ;

	echo ;
	echo "Press [Ctrl-C] to Exit, use [$DIR/wfshell] command to use Whitefield Shell..."
}

#Processing begins here...
main $*
