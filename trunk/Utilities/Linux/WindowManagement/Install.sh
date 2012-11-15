type xdotool &>/dev/null || { echo "xdotool not found." >&2; exit 1; }
for i in `ls w*` ; do ln -fs `pwd`/$i ~/bin/ ; done
