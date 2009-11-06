wget http://archive.bigbluebutton.org/bigbluebutton.asc 
sudo apt-key add bigbluebutton.asc 
echo "deb http://archive.bigbluebutton.org/ bigbluebutton main" | sudo tee /etc/apt/sources.list.d/bigbluebutton.list
sudo apt-get update 
