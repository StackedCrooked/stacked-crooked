ipfw pipe 1 config bw 32KByte/s
ipfw add 1 pipe 1 src-port 80
echo "Type \"ipfw delete 1\" to remove the limitation."
