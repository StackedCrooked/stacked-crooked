set -x
type xdotool &>/dev/null || { echo "xdotool not found." >&2; exit 1; }
for i in `ls w*` ; do ln -s `pwd`/$i ~/bin/ ; done
