imdisk -a -s 2000M -m M: -p "/fs:ntfs /q /c /y"
mkdir M:\tetris-challenge
xcopy C:\svn\tetris-challenge M:\tetris-challenge /E
start M: