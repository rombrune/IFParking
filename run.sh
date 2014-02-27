#! /bin/sh
dir="bin"
target="parking"

ps -fu `whoami` | grep -v grep | grep $target > $dir/ps.before
ipcs > $dir/ipcs.before

echo "---------- Run -------------"
echo "Running $dir/$target..."
($dir/$target)

ps -fu `whoami` | grep -v grep | grep $target > $dir/ps.after
ipcs > $dir/ipcs.after

echo "---------- Report ----------"
psDiff=`diff $dir/ps.before $dir/ps.after`
if [ -z "$psDiff" ]; then
	echo "Process diff: OK"
	rm -f $dir/ps.error
else
	echo "Process diff:"
	echo "$psDiff"
	echo

	echo "$psDiff" > $dir/ps.error
fi

ipcsDiff=`diff $dir/ipcs.before $dir/ipcs.after`
if [ -z "$ipcsDiff" ]; then
	echo "IPCS diff: OK"
	rm -f $dir/ipcs.error
else
	echo "IPCS diff:"
	echo "$ipcsDiff"
	echo

	echo "$ipcsDiff" > $dir/ipcs.error
fi
echo
