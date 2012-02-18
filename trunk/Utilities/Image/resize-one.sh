FILE=$1
MED_SIZE=2550x1440
LOW_SIZE=1280x720
mkdir -p Med Low
convert "High/${FILE}" -resize $MED_SIZE Med/${FILE}
convert "High/${FILE}" -resize $LOW_SIZE Low/${FILE}
