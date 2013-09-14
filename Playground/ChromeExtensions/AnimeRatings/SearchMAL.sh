#!/bin/bash
USERFILE="$HOME/.mal/user"
PASSWORDFILE="$HOME/.mal/password"
URL=http://myanimelist.net/api/anime/search.xml
[ -f $USERFILE ] || { echo "$USERFILE not found. Exiting" 1>&2 ; exit ; }
[ -f $PASSWORDFILE ] || { echo "$PASSWORDFILE not found. Exiting" 1>&2 ; exit ; }
USER=$(cat $USERFILE)
PASSWORD=$(cat $PASSWORDFILE)
wget -nv --user=$USER --password=$PASSWORD -O - "$URL?q=$1" 
