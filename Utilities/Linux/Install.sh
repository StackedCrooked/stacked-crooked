type xdotool &>/dev/null || { echo "xdotool not found." >&2; exit 1; }
ln -s `pwd`/wleft ~/bin/wleft
ln -s `pwd`/wright ~/bin/wright
