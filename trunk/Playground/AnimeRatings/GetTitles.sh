#!/bin/sh
xpath Anime_of_2011.html '//a[@name="Pages_in_category"]/../..//li//a/@title' 2>/dev/null | sed "s/ title\=/\\`echo -e '\n\r'`/g" | sed 's/\"//g' | sed 's/\r//g'
