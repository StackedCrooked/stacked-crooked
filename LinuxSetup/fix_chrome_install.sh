# Chrome can be downloaded as a .deb file which
# can be installed using this command:
#   sudo dpkg -i google-chrome-stable_current_amd64.deb
#
# However, this results in errors like:
#    google-chrome-stable depends on gconf-service; however:
#    
# Which can be fixed using the commands in this script (see https://askubuntu.com/questions/220960/cannot-install-google-chrome-how-do-i-fix-it)
sudo apt-get update
sudo apt-get install libgconf2-4 libnss3-1d libxss1
