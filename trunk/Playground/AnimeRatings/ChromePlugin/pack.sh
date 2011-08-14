#!/bin/sh
rm -rf anime-ratings
rm -f anime-ratings.crx
mkdir anime-ratings
cp background.html content.js manifest.json anime-ratings/
/Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome --pack-extension=anime-ratings --pack-extension-key=/Volumes/MYBOOK/PrivateKeys/anime-ratings.pem
