xwininfo -id $(xdotool getactivewindow) | grep Y | grep -i Absolute | cut -d ' ' -f 7
