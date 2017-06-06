
#include <cinatra_http/http_server.h>

#include <boost/bind.hpp>
#include <iostream>
#include <memory>

namespace cinatra
{
	http_server::http_server(const io_service_work_ptr & work)
		: io_service_pool_(work)
	{
	}

	cinatra::http_server& http_server::listen(const std::string& address, const std::string& port)
	{
		auto acceprot = std::make_shared<ASIO_NS::ip::tcp::acceptor>(io_service_pool_->get_io_service());
		do_listen(acceprot, address, port);
		start_accept(acceprot);
		return *this;
	}

#if !DISABLE_ASIO_OPENSSL
	cinatra::http_server& http_server::listen(const std::string& address, const std::string& port, ssl_method_t ssl_method,
		const std::string& private_key, const std::string& certificate_chain, bool is_file /*= true*/)
	{
		return listen(address, port, ssl_config_t{ ssl_method, private_key, certificate_chain, is_file });
	}

	cinatra::http_server& http_server::listen(const std::string& address, const std::string& port, const ssl_config_t& ssl_cfg)
	{
		auto ssl_ctx = std::make_shared<ASIO_NS::ssl::context>(static_cast<ASIO_NS::ssl::context::method>(ssl_cfg.ssl_method));
		if (ssl_cfg.is_file)
		{
			ssl_ctx->use_private_key_file(ssl_cfg.private_key, ASIO_NS::ssl::context::pem);
			ssl_ctx->use_certificate_chain_file(ssl_cfg.certificate_chain);
		}
		else
		{
			ssl_ctx->use_private_key(ASIO_NS::buffer(ssl_cfg.private_key), ASIO_NS::ssl::context::pem);
			ssl_ctx->use_certificate_chain(ASIO_NS::buffer(ssl_cfg.certificate_chain));
		}

		//HTTP2???
		//configure_tls_context_easy(ec, tls);
		auto acceprot = std::make_shared<ASIO_NS::ip::tcp::acceptor>(io_service_pool_->get_io_service());
		do_listen(acceprot, address, port);
		start_accept(acceprot, ssl_ctx);
		return *this;
	}
#endif

	void http_server::start()
	{
		io_service_pool_->start();
	}

	void http_server::run()
	{
		io_service_pool_->run();
	}

	size_t http_server::run_one()
	{
		return io_service_pool_->run_one();
	}

	size_t http_server::poll()
	{
		return io_service_pool_->poll();
	}

	size_t http_server::poll_one()
	{
		return io_service_pool_->poll_one();
	}

	void http_server::start_accept(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor)
	{
		auto new_conn = std::make_shared<connection<ASIO_NS::ip::tcp::socket>>(
			io_service_pool_->get_io_service(), request_handler_, max_req_size_, keep_alive_timeout_);
		acceptor->async_accept(new_conn->socket(), [this, new_conn, acceptor](const error_code& e)
		{
			if (!e)
			{
				new_conn->socket().set_option(ASIO_NS::ip::tcp::no_delay(true));
				new_conn->start();
			}
			else
			{
				std::cout << "server::handle_accept: " << e.message() << std::endl;
			}

			start_accept(acceptor);
		});
	}

#if !DISABLE_ASIO_OPENSSL
	void http_server::start_accept(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor,
		std::shared_ptr<ASIO_NS::ssl::context> const& ssl_ctx)
	{
		auto new_conn = std::make_shared<connection<ASIO_NS::ssl::stream<ASIO_NS::ip::tcp::socket>>>(
			io_service_pool_->get_io_service(), request_handler_, *ssl_ctx, max_req_size_, keep_alive_timeout_);
		acceptor->async_accept(new_conn->socket().lowest_layer(), [this, new_conn, acceptor, ssl_ctx](const error_code& e)
		{
			if (!e)
			{
				new_conn->socket().lowest_layer().set_option(ASIO_NS::ip::tcp::no_delay(true));
				new_conn->reset_timer();
				new_conn->socket().async_handshake(ASIO_NS::ssl::stream_base::server,
					[new_conn](const error_code &e)
				{
					if (e)
					{
						return;
					}
					//HTTP2???
// 					if (!tls_h2_negotiated(new_conn->socket()))
// 					{
// 						return;
// 					}
					new_conn->start();
				});
			}
			else
			{
				std::cout << "server::handle_accept: " << e.message() << std::endl;
			}

			start_accept(acceptor, ssl_ctx);
		});
	}
#endif

	void http_server::do_listen(std::shared_ptr<ASIO_NS::ip::tcp::acceptor> const& acceptor,
		const std::string& address, const std::string& port)
	{
		ASIO_NS::ip::tcp::resolver resolver(acceptor->get_io_service());
		ASIO_NS::ip::tcp::resolver::query query(address, port);
		ASIO_NS::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		acceptor->open(endpoint.protocol());
		acceptor->set_option(ASIO_NS::ip::tcp::acceptor::reuse_address(true));
		acceptor->bind(endpoint);
		acceptor->listen();
	}

	void http_server::stop()
	{
		io_service_pool_->stop();
	}

}
