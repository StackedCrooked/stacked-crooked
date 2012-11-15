xwininfo -id $(xdotool getactivewindow) | grep Width | cut -d ' ' -f4
