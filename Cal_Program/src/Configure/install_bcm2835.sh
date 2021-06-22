sudo apt-get install -y html-xml-utils
wget -P /home/pi/Downloads/ http://www.airspayce.com/mikem/bcm2835/bcm2835-1.62.tar.gz;
cd /home/pi/Downloads
tar zxvf bcm2835*.tar.gz
cd /home/pi/Downloads/bcm2835*
/home/pi/Downloads/bcm2835-1.62/configure
make
sudo make check
sudo make install


#cleanup files
rm /home/pi/Downloads/stamp-h1
rm /home/pi/Downloads/config.status
rm /home/pi/Downloads/config.log
rm -r rm /home/pi/Downloads/src/
rm -r rm /home/pi/Downloads/doc/