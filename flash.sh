#!/bin/bash

rm *.bin

echo
echo Compiling...
echo

output=$(particle compile photon .)

#filename=${foo##* }
matchline=$(echo "$output" | grep photon_firmware)

filename=$(echo "$matchline" | cut -d \  -f 3)

#echo "match $matchline"
#echo "parts $parts"

#echo "Filename: $filename"

if [[ $filename == *bin ]]
then

echo "flashing: $filename"

particle flash --usb $filename


else

echo "error in compilation: $output"

fi
