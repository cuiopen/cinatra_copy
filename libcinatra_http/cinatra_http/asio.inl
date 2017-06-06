
#pragma once

#if DISABLE_BOOST_ASIO

#include <asio.hpp>
#if !DISABLE_ASIO_OPENSSL
#include <asio/ssl.hpp>
#endif

#ifndef ASIO_NS
#	define ASIO_NS asio
#endif

namespace cinatra
{
	using error_code = asio::error_code;
}

#else

#include <boost/asio.hpp>
#if !DISABLE_ASIO_OPENSSL
#include <boost/asio/ssl.hpp>
#endif

#ifndef ASIO_NS
#	define ASIO_NS boost::asio
#endif

namespace cinatra
{
	using error_code = boost::system::error_code;
}

#endif
