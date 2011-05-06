#!/bin/bash
DEPENDENCIES="git curl hg bzr"
for dep in $DEPENDENCIES; do
    type ${dep} &>/dev/null || { echo "${dep} not found. Dependencies: ${DEPENDENCIES}." >&2; exit 1; }
done

VIM_ADDONS=$HOME/vim-addons
if [ -d $VIM_ADDONS ]; then
    echo "The directory $VIM_ADDONS already exists."
    echo "Exiting."
    exit 1
fi
mkdir $VIM_ADDONS
VIM_ADDON_MANAGER=$VIM_ADDONS/vim-addon-manager
mkdir $VIM_ADDON_MANAGER
git clone git://github.com/MarcWeber/vim-addon-manager.git $VIM_ADDON_MANAGER
