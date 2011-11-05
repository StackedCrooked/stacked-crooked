#!/bin/sh
# Clustersize seems to be 512 bytes.
# Formula: RequestedMB * 2048 = NumberOfClusters
# Create a 1 GB volume by specifying ram://2048000
diskutil erasevolume HFS+ "RAMDisk" `hdiutil attach -nomount ram://5000000`

