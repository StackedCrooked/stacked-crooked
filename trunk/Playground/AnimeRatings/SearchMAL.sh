#!/bin/sh
URL=http://myanimelist.net/api/anime/search.xml
wget --output-document=result.xml --user=AnimeRatings --password=Chrome "$URL?q=$@" 1>/dev/null 2>/dev/null
cat result.xml
rm result.xml
