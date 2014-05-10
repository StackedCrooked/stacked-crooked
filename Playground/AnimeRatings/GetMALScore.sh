#!/bin/bash
user_file="$HOME/.mal/user"
[ -f $user_file ] || { echo "$user_file not found. Exiting" 1>&2 ; exit ; }
user="$(cat $user_file)"

password_file="$HOME/.mal/password"
[ -f $password_file ] || { echo "$password_file not found. Exiting" 1>&2 ; exit ; }
password="$(cat $password_file)"

url='http://myanimelist.net/api/anime/search.xml'
wget -nv --output-document=result.xml --user=$user --password=$password "$url?q=$1" 
cat result.xml 
