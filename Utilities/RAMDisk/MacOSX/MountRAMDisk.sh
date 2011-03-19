# Clustersize seems to be 512 bytes. So ram://1165430 is actually 569 MB
# Formula: RequestedMB * 2048 = MagicNumber
diskutil erasevolume HFS+ "ramdisk" `hdiutil attach -nomount ram://2048000`

