#!/bin/bash
# pirnt current directory
pwd
#print home directory
echo $HOME

sudo id

sudo apt install -y python3-pip python3-setuptools python3-wheel ninja-build build-essential flex bison git cmake libsctp-dev libgnutls28-dev libgcrypt-dev libssl-dev libidn11-dev libmongoc-dev libbson-dev libyaml-dev libnghttp2-dev libmicrohttpd-dev libcurl4-gnutls-dev libnghttp2-dev libtins-dev libtalloc-dev meson

source_dir="$HOME/open5gs_source_code" # source code directory

# check "$HOME/open5gs" directory exist or not, if not, create it
if [ ! -d "$HOME/open5gs" ]; then
    cp -r $source_dir "$HOME/open5gs"
    rm -f "$HOME/open5gs/build_open5gs.sh"
fi

# if current directory is not "$HOME/open5gs", then change to "$HOME/open5gs"
if [ "$PWD" != "$HOME/open5gs" ]; then
    cd $HOME/open5gs
fi
pwd
meson build --prefix=`pwd`/install
ninja -C build install