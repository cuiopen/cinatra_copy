
#pragma once

#include <cinatra_http/io_service_pool.h>
#include <cinatra_http/connection.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <vector>

namespace cinatra
{

	class http_server : private boost::noncopyable
	{
	public:
		explicit http_server(const io_service_work_ptr & work);


#if !DISABLE_ASIO_OPENSSL
		enum ssl_method_t
		{
			sslv2 = ASIO_NS::ssl::context::sslv2_server,
			sslv3 = ASIO_NS::ssl::context::sslv3_server,
			tlsv1 = ASIO_NS::ssl::context::tlsv1_server,
			tlsv11 = ASIO_NS::ssl::context::tlsv11_server,
			tlsv12 = ASIO_NS::ssl::context::tlsv12_server,
			sslv23 = ASIO_NS::ssl::context::sslv23_server
		};

		struct ssl_config_t
		{
			ssl_method_t ssl_method;
			std::string private_key;
			std::string certificate_chain;
			bool is_file;
		};
#endif

		http_server& listen(const std::string& address, const std::string& port);
#if !DISABLE_ASIO_OPENSSL
		http_server& listen(const std::string& address, const std::string& port, const ssl_config_t& ssl_cfg);
		http_server& listen(const std::string& address, const std::string& port, ssl_method_t ssl_method,
			const std::string& private_key, const std::string& certificate_chain, bool is_file = true);
#endif

		void start();
		void run();
		size_t run_one();
		size_t poll();
		size_t poll_one();

		void request_handler(request_handler_t handler)
		{
			request_handler_ = std::move(handler);
		}

		void stop();

		void set_max_req_size(std::size_t sz)
		{
			max_req_size_ = sz;
		}

		void set_keep_alive_timeout(long seconds)
		{
			keep_alive_timeout_ = seconds;
		}
	private:
		void start_accept(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor);
#if !DISABLE_ASIO_OPENSSL
		void start_accept(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor,
			std::shared_ptr<ASIO_NS::ssl::context> const& ssl_ctx);
#endif

		void do_listen(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor,
			const std::string& address, const std::string& port);

		io_service_work_ptr io_service_pool_;
		request_handler_t request_handler_;
		
		typedef std::shared_ptr<ASIO_NS::ip::tcp::acceptor> tcp_acceptor_ptr;
		std::vector<tcp_acceptor_ptr> tcp_acceptor_;

		std::size_t max_req_size_ = 2 * 1024 * 1024;
		long keep_alive_timeout_ = 60;
	};

}