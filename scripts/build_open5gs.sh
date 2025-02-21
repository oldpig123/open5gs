#!/bin/bash
# pirnt current directory
pwd
#print home directory
echo $HOME

sudo id

sudo apt install -y python3-pip python3-setuptools python3-wheel ninja-build build-essential flex bison git cmake libsctp-dev libgnutls28-dev libgcrypt-dev libssl-dev libidn11-dev libmongoc-dev libbson-dev libyaml-dev libnghttp2-dev libmicrohttpd-dev libcurl4-gnutls-dev libnghttp2-dev libtins-dev libtalloc-dev meson

source_dir="$HOME/open5gs_source_code" # source code directory
roaming=1 # use as a flag to check if this is a roaming or non-roaming setup

# check "$HOME/open5gs" directory exist or not, if not, create it
if [ ! -d "$HOME/open5gs" ]; then
    cp -r $source_dir "$HOME/open5gs"
    rm -f "$HOME/open5gs/scripts/build_open5gs.sh"
fi

# if current directory is not "$HOME/open5gs", then change to "$HOME/open5gs"
if [ "$PWD" != "$HOME/open5gs" ]; then
    cd $HOME/open5gs
fi
pwd
meson build --prefix=`pwd`/install
ninja -C build install

# set alias of "./build/tests/registration/registration -c ./build/configs/examples/gnb-999-70-ue-001-01.yaml simple-test" to "roaming-test"
# check "roaming-test" alias exist or not, if not, then check roaming flag is set or not, if set then create alias
if [ ! -x "$(command -v roaming-test)" ]; then
    if [ $roaming -eq 1 ]; then
        echo "alias roaming-test='./build/tests/registration/registration -c ./build/configs/examples/gnb-999-70-ue-001-01.yaml simple-test'" >> ~/.bashrc
    fi
fi


#print few empty lines and a seperation line
echo -e "\n
----------------------------------------------
open5gs build is completed successfully
----------------------------------------------
\n
"
#print support message to remind user to set up tun interface and forwarding with set_tun.sh
echo "Please set up tun interface and forwarding with set_tun.sh before start the NFs.\n"

echo "To start the NFs, use 'start_NFs.sh' script.\n"
echo "To start the NFs without SEPP, use 'start_NFs_without_SEPP.sh' script.\n"

# if alias is set then print the message
if [ $roaming -eq 1 ]; then
    echo "To run the roaming test, use 'roaming-test' command\n"
fi

