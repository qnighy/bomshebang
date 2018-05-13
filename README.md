# bomshebang

A proof-of-concept of allowing shabang with UTF-8 BOM.

# Usage (on Ubuntu)

```
sudo apt install build-essential binfmt-support
make
sudo make install
sudo make uninstall
./sample.pl # works
./sample.sh # doesn't work, sorry!
```
