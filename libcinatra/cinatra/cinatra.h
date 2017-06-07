#pragma once


#include <cinatra/aop.hpp>
#include <cinatra/router.h>

#include <cinatra_http/http_server.h>
#include <cinatra_http/utils.h>

#include <boost/thread.hpp>

namespace cinatra
{
	template<typename... MiddlewaresT>
	class cinatra
	{
	public:
		
		void listen(const std::string& address, const std::string& port)
		{
			listen_infos_.emplace_back(listen_info_t{ address, port });
		}
#if !DISABLE_ASIO_OPENSSL
		void listen(const std::string& address, const std::string& port, const http_server::ssl_config_t& ssl_cfg)
		{
			listen_infos_.emplace_back(ssl_listen_info_t{ listen_info_t{ address, port }, ssl_cfg });
		}
		void listen(const std::string& address, const std::string& port, http_server::ssl_method_t ssl_method,
			const std::string& private_key, const std::string& certificate_chain, bool is_file = true)
		{
			listen_infos_.emplace_back(ssl_listen_info_t
			{
				listen_info_t{ address, port },
				http_server::ssl_config_t{ssl_method, private_key, certificate_chain, is_file}
			});
		}
#endif

		template<class Work, class... Args>
		void start(Args&&... args)
		{
			start_(std::make_shared<Work>(std::forward<Args>(args)...));
		}
	private:
		void start_(const io_service_work_ptr & work)
		{
			server_ = std::make_shared<http_server>(work);

			server_->request_handler([this](request const& req, response& res)
			{
				context_container ctx;
				if (!aop_.invoke(req, res, ctx))
				{
					res = reply_static_file(static_path_, req);
				}
			});

			for (auto const& info : listen_infos_)
			{
				server_->listen(info.address, info.port);
			}

#if !DISABLE_ASIO_OPENSSL
			for (auto const& info : ssl_listen_infos_)
			{
				server_->listen(info.info.address, info.info.port, info.config);
			}
#endif

			aop_.set_func([this](request const& req, response & res, context_container& ctx)
			{
				return router_.handle(req, res, ctx);
			});

			server_->set_max_req_size(max_req_size_);
			server_->set_keep_alive_timeout(keep_alive_timeout_);

			server_->start();
		}

	public:
		void run()
		{
			server_->run();
		}

		size_t run_one()
		{
			return server_->run_one();
		}

		size_t poll()
		{
			return server_->poll();
		}

		size_t poll_one()
		{
			return server_->poll_one();
		}

		void stop()
		{
			if (server_)
			{
				server_->stop();
				server_ = nullptr;
			}
		}

		template<typename FunctionT>
		void route(const std::string& path, const FunctionT& f)
		{
			router_.route(path, f);
		}

		template<typename T>
		T& get_middleware()
		{
			return aop_.template get_aspect<T>();
		}

		void set_static_path(std::string path)
		{
			static_path_ = std::move(path);
		}

		void set_max_req_size(std::size_t sz)
		{
			max_req_size_ = sz;
		}

		void set_keep_alive_timeout(long seconds)
		{
			keep_alive_timeout_ = seconds;
		}

        void set_thread_num(std::size_t num)
        {
            io_service_pool_size_ = num;
        }
	private:
		aop<MiddlewaresT...> aop_;
		std::shared_ptr<http_server> server_;


		struct listen_info_t 
		{
			std::string address;
			std::string port;
		};
		std::vector<listen_info_t> listen_infos_;

#if !DISABLE_ASIO_OPENSSL
		struct ssl_listen_info_t
		{
			listen_info_t info;
			http_server::ssl_config_t config;
		};
		std::vector<ssl_listen_info_t> ssl_listen_infos_;
#endif
		router router_;

		std::string static_path_;

		std::size_t max_req_size_ = 2 * 1024 * 1024;
		long keep_alive_timeout_ = 60;
        std::size_t io_service_pool_size_ = boost::thread::hardware_concurrency();
	};

}