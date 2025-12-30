#use this script, if you want run 2 sdr (one tx one rx)

sudo ./../../build/SDR usb:3.5.5 0 &
PID1=$!

sleep 0.01

sudo ./../../build/SDR usb:3.6.5 1 &
PID2=$!

wait $PID1 $PID2
