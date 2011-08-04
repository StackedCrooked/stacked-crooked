#!/bin/sh
echo "Requesting score for $1"
URL=http://myanimelist.net/api/anime/search.xml
wget --output-document=result.xml --user=$USER --password=$PASSWORD "$URL?q=$1" 2>/dev/null
cat result.xml | grep \<title\> | sed 's/^.*\<title\>//' | sed 's/\<\/title\>//'
cat result.xml | grep \<score\> | sed 's/^.*\<score\>//' | sed 's/\<\/score\>//'
