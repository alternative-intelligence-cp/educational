#!/usr/bin/env bash

#update packages
sudo apt update
sudo apt upgrade -y
sudo apt autoremove -y
sudo apt autoclean -y
#install apache2 http/s server
sudo apt install apache2 --install-recommends -y &&
#enable apache2 service to start on boot and restart on crash
sudo systemctl enable apache2.service
#start apache2 service 
sudo systemctl start apache2.service &&
#allow traffic to apache through firewall
sudo ufw allow apache2/tcp
sudo ufw allow apache2/udp
#open root user file explorer in web data folder
sudo nemo /var/www/html

 
