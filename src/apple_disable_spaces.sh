#!/bin/sh
# disable spaces / multiple desktops in Mac
#Sets user argument for script
loggedInUser="$3"
#Sets defaults for selected user
sudo -u $loggedInUser defaults write com.apple.dock mcx-expose-disabled -boolean TRUE
sudo -u $loggedInUser defaults write com.apple.dashboard mcx-disabled -boolean TRUE
killall Dock
#Reads and reports to logs in Casper
sudo -u $loggedInUser defaults read com.apple.dock mcx-expose-disabled
sudo -u $loggedInUser defaults read com.apple.dashboard mcx-disabled
