#!/bin/bash

PASSES=0
FAILS=0
for i in *sol; do
	echo "Trying to execute ${i/sol/sql}"
	SOL=`sqlite3 db.sq3 < ${i/sol/sql}`

	echo "${SOL}" | diff -u $i -

	if [ $? -eq 0 ] ; then 
		echo "PASS" ; 
		((PASSES++))
	else 
		echo "FAIL" ; 
		((FAILS++))
	fi
done

echo "Result: ${PASSES} passes and ${FAILS} fails"
