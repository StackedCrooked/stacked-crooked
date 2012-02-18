find High -name "*\.jpg" -type f -print0 | xargs -0 -n 1 -P 8 sh -c 'echo $0 | sed "s/High\///g" | xargs ./resize-one.sh'

