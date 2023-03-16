#!/bin/sh
bindir=$(pwd)
cd /home/nicolas/Bureau/HAI819I/TP/engine/engenius/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/nicolas/Bureau/HAI819I/TP/engine/build/engenius 
	else
		"/home/nicolas/Bureau/HAI819I/TP/engine/build/engenius"  
	fi
else
	"/home/nicolas/Bureau/HAI819I/TP/engine/build/engenius"  
fi
