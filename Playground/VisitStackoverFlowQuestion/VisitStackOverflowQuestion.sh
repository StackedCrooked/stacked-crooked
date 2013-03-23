#!/bin/bash
cache_id() { (EPOCH=$(date +%s) && echo $(($EPOCH / 60))) ; }

URL_FILE="/tmp/URL_$(cache_id)"

[ -f ${URL_FILE} ] || { echo "Download new questions." && rm -f "/tmp/URL_*" && wget -q -O - "http://stackoverflow.com/questions/tagged/c++" \
    | egrep -e '/questions/[0-9]+' \
    | perl -p -e 's,.*href="(.+)"\s.*,\1,g' \
    | xargs -I {} echo "http://stackoverflow.com{}" \
    | xargs -I {} echo {} > "${URL_FILE}" ; }

get_open_command() {
    { [ `uname`    == "Linux"  ] && echo "xdg-open" ; } ||
    { [ `uname`    == "Darwin" ] && echo "open"     ; } ||
    { [ `uname -o` == "Cygwin" ] && echo "cygstart" ; } ||
    { echo "Unsupported platform." 1>&2 && exit 1 ; }
}

countdown() {
  for n in $(seq $1) ; do
    REMAINING=$(($1 - $n + 1))
    printf "\r$REMAINING\r"
    sleep 1
  done
}

for url in `cat ${URL_FILE}` ; do 
  read -p "Do you want to visit ${url} (Y/n)? " -n 1 -r
  if [[ $REPLY =~ ^[Yy]$ ]] ; then
    $(get_open_command) $url
  fi
  echo "" # ensure new line
done

