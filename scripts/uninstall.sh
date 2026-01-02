#use this script, if you want delete libs needs to work with Pluto SDR (SoapySDR, SoapyPlutoSDR, libad9361, libiio)

#delete from /usr/local/lib/
sudo rm -v /usr/local/lib/libSoapySDR*.so*
sudo rm -v /usr/local/lib/libad*.so*
sudo rm -rv /usr/local/lib/SoapySDR

#delete from /usr/local/include/
sudo rm -rv /usr/local/include/SoapySDR
sudo rm -rv /usr/local/include/ad9361.h

#delete from /usr/local/bin/
sudo rm -v /usr/local/bin/SoapySDR*

sudo ldconfig