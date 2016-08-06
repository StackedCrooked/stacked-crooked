# Emulate typing delay.
# Usage example:
#  >source delay # load the delay function
#  >delay 0.1 # set typing delay to 0.1 s
delay() { rm -f /tmp/fifo; mkfifo /tmp/fifo; while true ; do cat /tmp/fifo; sleep $1; done & while true ; do read -rsn1 n; { [ "$n" == "" ] && printf "\n" || printf "$n"; } >/tmp/fifo; done; }
