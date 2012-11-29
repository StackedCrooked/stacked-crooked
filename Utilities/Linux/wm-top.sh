W=`wm-desktopwidth.sh`
H=`wm-desktopwidth.sh`
xdotool windowmove $(xdotool getactivewindow) 0 0
xdotool windowsize $(xdotool getactivewindow) $W $(expr $H / 2)
