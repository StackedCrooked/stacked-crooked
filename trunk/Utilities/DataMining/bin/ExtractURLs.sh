[ "$#" == "0" ] || { echo "This script reads HTML data from stdin. Now parameters are allowed." 1>&1 ; exit 1 ; }
cat $1 | perl -pe 's,",\n,g' | perl -pe "s,',\n,g" | grep ^http | RemoveColorCodes.sh

