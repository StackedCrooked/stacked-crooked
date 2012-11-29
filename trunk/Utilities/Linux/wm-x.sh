xwininfo -id $(xdotool getactivewindow) | grep X | grep -i Absolute | cut -d ' ' -f 7
