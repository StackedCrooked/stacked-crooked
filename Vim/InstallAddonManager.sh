#!/bin/bash
type git &>/dev/null || { echo "Please install git first." >&2; exit 1; }
type curl &>/dev/null || { echo "Please install curl first." >&2; exit 1; }
VIM_ADDONS=$HOME/vim-addons
if [ -d $VIM_ADDONS ]; then
    echo "$VIM_ADDONS already exists."
    exit 1
fi
mkdir $VIM_ADDONS
VIM_ADDON_MANAGER=$VIM_ADDONS/vim-addon-manager
mkdir $VIM_ADDON_MANAGER
git clone git://github.com/MarcWeber/vim-addon-manager.git $VIM_ADDON_MANAGER
