#!/bin/sh
URL=http://myanimelist.net/api/anime/search.xml
wget --output-document=result.xml --user=$USER --password=$PASSWORD "$URL?q=$@"
