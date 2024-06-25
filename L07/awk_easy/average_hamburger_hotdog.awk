#!/bin/awk -f

BEGIN {
	printf("Calculating average hamburger and hotdog values\n")
}

# Do this to each input record (by default, each newline-separated string)
# record separator can be changed by setting RS value
{
	# skip first line
	if (NR == 1)
		next

	# $2, $3 refer to fields 2 and 3 in a given record (field separator defaults to space)
	# field separator can be changed by setting FS value
	# variables initialized as 0 on first reference
	hamburger += $2
	hamburger_count++
	hotdog += $3
	hotdog_count++
}

END {
	# integer division
	printf("Average hamburger: %d, Average hotdog: %d\n",
	       	hamburger/hamburger_count, hotdog/hotdog_count);
}
