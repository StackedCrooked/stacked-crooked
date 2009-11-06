cd /usr/local/src
sudo wget http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/dahdi-linux-complete-2.2.0.2+2.2.0.tar.gz
sudo tar zxvf dahdi-linux-complete-2.2.0.2+2.2.0.tar.gz
cd dahdi-linux-complete-2.2.0.2+2.2.0
sudo make all
sudo make install
sudo make config

