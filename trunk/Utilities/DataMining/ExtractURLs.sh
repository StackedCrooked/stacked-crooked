cat $1 | perl -pe 's,",\n,g' | perl -pe "s,',\n,g" | grep ^http
