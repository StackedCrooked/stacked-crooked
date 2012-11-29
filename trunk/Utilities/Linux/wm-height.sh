xwininfo -id $(xdotool getactivewindow) | grep Height | cut -d ' ' -f4
