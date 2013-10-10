#!/bin/sh
USERFILE="$HOME/.mal/user"
[ -f $USERFILE ] || { echo "$USERFILE not found. Exiting" 1>&2 ; exit ; }
USER=$(cat $USERFILE)

PASSWORDFILE="$HOME/.mal/password"
[ -f $PASSWORDFILE ] || { echo "$PASSWORDFILE not found. Exiting" 1>&2 ; exit ; }
PASSWORD=$(cat $PASSWORDFILE)

URL=http://myanimelist.net/api/anime/search.xml
wget -nv --output-document=result.xml --user=$USER --password=$PASSWORD "$URL?q=$1" 
cat result.xml | grep \<title\> | sed 's/^.*\<title\>//' | sed 's/\<\/title\>//'
cat result.xml | grep \<score\> | sed 's/^.*\<score\>//' | sed 's/\<\/score\>//'
