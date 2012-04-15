#!/bin/sh

if [ "$(whoami)" != "root" ]; then
  echo "Permission denied. Please run this script with root priviliges.";
  exit 1
fi

add-apt-repository ppa:rabbitvcs/ppa
apt-get update
apt-get install rabbitvcs-cli
