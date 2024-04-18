#!/bin/bash 
sudo id

# if currrent directotry is no $HOME/open5gs then change to it
if [ "$PWD" != "$HOME/open5gs" ]; then
    cd $HOME/open5gs
fi

sudo ./install/bin/open5gs-nrfd & sudo ./install/bin/open5gs-scpd & sudo ./install/bin/open5gs-amfd & sudo ./install/bin/open5gs-smfd & sudo ./install/bin/open5gs-upfd & sudo ./install/bin/open5gs-ausfd & sudo ./install/bin/open5gs-udmd & sudo ./install/bin/open5gs-pcfd & sudo ./install/bin/open5gs-nssfd & sudo ./install/bin/open5gs-bsfd & sudo ./install/bin/open5gs-udrd