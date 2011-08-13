#!/bin/sh
wget --output-document=result.xml --user=AnimeRatings --password=Chrome $@ 1>/dev/null 2>/dev/null
cat result.xml
rm result.xml
