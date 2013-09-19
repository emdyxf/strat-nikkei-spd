#!/bin/sh 

if [ $# != 3 ]; then
	echo "Usage: $0 <strat-Log> <fc-log> <om-log>"
	exit
fi

# Input files
strat_log_file=$1
fc_log_file=$2
om_log_file=$3

# Output files
latency_details=Hedge_Response_Details.txt
latency_summary=Hedge_Response_Summary[us].csv

echo "S/N,STRAT_ID,QUOTE_FILL_TIME,HEDGE_SENT_TIME,HEDGE_RESPONSE(us)" > $latency_summary
echo "NKSP Strat Log: $strat_log_file" > $latency_details
echo "FIX fc Log: $fc_log_file" >> $latency_details
echo "+-+-+-+-+-+-+-+-+-+-+-+-+" >> $latency_details

# Temp files
filled_order_ids=filled_ids.tmp
quote_filled_ids=quote_filled_ids.tmp
exec_strat_ids=exec_strat_ids.tmp

grep EVENT_STREET_FILLS $strat_log_file | cut -d\( -f2 | cut -d\) -f1 > $filled_order_ids

fgrep -f $filled_order_ids $strat_log_file | grep PrintStreetExec | grep QUOTE | awk '{print $8 $21 $11 $12}' > $quote_filled_ids

fgrep -f $filled_order_ids $strat_log_file | grep PrintStreetExec | grep QUOTE | awk '{print $8 $12 $21}' | cut -d[ -f4 | cut -d] -f1 | cut -dQ -f1 > $exec_strat_ids


exec_counts=0

for VARIABLE in `cat $exec_strat_ids`
do
	quote_oid=`grep $VARIABLE $quote_filled_ids | cut -d[ -f2 | cut -d] -f1`

	quote_fill_time=`grep 39=2 $fc_log_file | grep $quote_oid | awk '{print $1}' | cut -d[ -f2 | cut -d] -f1 | sed 's/\./\:/g' | sed 's/\:/\ /g'`

	hedge_oid=`grep $VARIABLE $strat_log_file | grep "SendOutStreetOrder" | grep "HEDGE" | awk '{print $19}' | cut -d[ -f2 | cut -d] -f1`

	hedge_sent_time=`grep 35=D $fc_log_file | grep $hedge_oid | awk '{print $1}' | cut -d[ -f2 | cut -d] -f1 | sed 's/\./\:/g' | sed 's/\:/\ /g'`
	
	latency_us=`echo "$VARIABLE"QUTOE_FILL [ $quote_fill_time ] "$VARIABLE"HEDGE_SENT [ $hedge_sent_time ] \
		| awk '{print $10*1000000*3600 + $11*1000000*60 + $12*1000000 + $13 \
			- $3*1000000*3600 - $4*1000000*60 - $5*1000000 - $6}'`
	
	exec_counts=`echo $exec_counts | awk '{print $1 + 1}'`
	
	echo $exec_counts "," `echo $VARIABLE | sed 's/_/\ /g' | awk '{print $3}'` "," `echo $quote_fill_time | sed 's/\ /:/g'` "," `echo $hedge_sent_time | sed 's/\ /:/g'` "," $latency_us >> $latency_summary

	echo ["#"$exec_counts] "Hedge Response(us)": [$latency_us] -">" "$VARIABLE"QUTOE_FILL:[`echo $quote_fill_time | sed 's/\ /:/g'`] "$VARIABLE"HEDGE_SENT:[`echo $hedge_sent_time | sed 's/\ /:/g'`] >> $latency_details
	grep 39=2 $fc_log_file | grep $quote_oid >> $latency_details
	grep 35=D $fc_log_file | grep $hedge_oid >> $latency_details
	echo " " >> $latency_details
		
	echo -n $exec_counts | awk '{printf(" [%s] Record processed...\r",$0)}'
	
done

printf "\nHedge Response Summarized in : [$latency_summary]\n"
printf "Hedge Response Details in : [$latency_details]\n"


# Remove temp files
rm -rf $filled_order_ids; rm -rf $quote_filled_ids; rm -rf $exec_strat_ids;

