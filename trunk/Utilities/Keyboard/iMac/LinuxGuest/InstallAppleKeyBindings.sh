#!/bin/sh
ln -fs `pwd`/_xmodmaprc $HOME/.xmodmaprc
xmodmap $HOME/.xmodmaprc
