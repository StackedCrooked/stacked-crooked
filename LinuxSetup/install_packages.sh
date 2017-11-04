#!/bin/bash
sudo apt-get install -y $(cat packages.txt | xargs)
