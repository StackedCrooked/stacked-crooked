cd /usr/local/src 
sudo wget http://downloads.asterisk.org/pub/telephony/asterisk/releases/asterisk-1.4.25.tar.gz
sudo tar zxvf asterisk-1.4.25.tar.gz
cd asterisk-1.4.25
sudo ./configure
sudo make 
sudo make install
sudo make samples
sudo make config
