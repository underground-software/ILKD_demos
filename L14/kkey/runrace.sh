#!/bin/sh

trap 'echo "$count attempts at race"' EXIT
count=0
while true; do

./reset
./examples/race
cat /dev/kkey000 > race.mid

OFFS=$(midicsvpy race.mid /dev/stdout | grep -c off)
ONS=$(midicsvpy race.mid /dev/stdout | grep -c on)

# if [ "$OFFS" != "3840" -o "$ONS" != "3840" ]; then
if [ "$OFFS" != "128" -o "$ONS" != "128" ]; then
	break
fi

count=$((count + 1))
done

echo "Race detected: offs $OFFS ons $ONS"
