# cinatra_copy

相对原版修改说明:
    一、 DISABLE_BOOST_ASIO 宏控制是否使用boost.asio。定义DISABLE_BOOST_ASIO=1,直接使用独立版的asio。
    二、 DISABLE_ASIO_OPENSSL 宏控制是否使用SSL。定义DISABLE_ASIO_OPENSSL=1,禁用SSL,从而也放弃支持https。将来打算替换SHA1实现，以便于完全不依赖openssl。
    三、 将原版的run过程拆分为start+(run/run_once/poll/poll_one)，以便于在外部控制循环过程。
        增加io_service_work接口，替换原版io_service_pool的相同功能。
	    原版的io_service_pool派生自io_service_work，提供原版完整的逻辑。
	    增加的io_service_inplace派生自io_service_work，提供单一的asio.io_service，并完全实现run/run_once/poll/poll_one功能。
	    http_server基于io_service_work工作，提供原版io_service_pool相同的功能。
	    因此,cinatra和http_server均增加start/run/run_once/poll/poll_once函数,以便于操作io_service_work相同的函数。

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

QQ group：340713904