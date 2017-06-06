# cinatra_copy

???????:
    ?? DISABLE_BOOST_ASIO ???????boost.asio???DISABLE_BOOST_ASIO=1,???????asio??
    ?? DISABLE_ASIO_OPENSSL ???????SSL???DISABLE_ASIO_OPENSSL=1,??SSL,??????https? ??????SHA1??,????????openssl?
    ?? ????run?????start+(run/run_once/poll/poll_one),????????
        ???io_service_work??,???io_service_pool??????
	io_service_pool???io_service_work,??????????
	???io_service_inplace???io_service_work,???asio.io_service,?????run/run_once/poll/poll_one?
	http_server??io_service_work????,???????io_service_pool?
	???,cinatra?http_server???start/run/run_once/poll/poll_once??,?????io_service_work??????

# cinatra
	
[![Build Status](https://travis-ci.org/topcpporg/cinatra.svg?branch=master)](https://travis-ci.org/topcpporg/cinatra)
[![Coverage Status](https://coveralls.io/repos/topcpporg/cinatra/badge.svg?branch=master&service=github)](https://coveralls.io/github/topcpporg/cinatra?branch=master)
a sinatra inspired modern c++ web framework

#Build on linux

### First: Boost library
#####Debian/Ubuntu:

```
sudo apt-get install libboost-dev
sudo apt-get install libboost-system-dev libboost-coroutine-dev libboost-thread-dev

```

#### ArchLinux
```
sudo pacman -S boost
```

### Then: Make binary
```
git clone https://github.com/topcpporg/cinatra.git
cd cinatra
mkdir build
cd build
cmake ..
make
```
##### Done!

##### chinese wiki:https://github.com/topcpporg/cinatra/wiki
##### english wiki:https://github.com/topcpporg/cinatra/wiki/Introduction

# Used open source library:
### boost: http://www.boost.org/

examples: [https://github.com/topcpporg/cinatra_example](https://github.com/topcpporg/cinatra_example)

QQ group£º340713904