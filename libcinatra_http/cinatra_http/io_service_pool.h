
#pragma once

#include "asio.inl"
#include <boost/noncopyable.hpp>

#include <vector>
#include <memory>
#include <thread>

namespace cinatra
{
	class io_service_work
		: private boost::noncopyable
	{
	public:
		virtual ~io_service_work();

		virtual void start() = 0;

		virtual bool run() = 0;
		virtual intptr_t run_one() = 0;
		virtual intptr_t poll() = 0;
		virtual intptr_t poll_one() = 0;

		virtual void stop() = 0;

		virtual ASIO_NS::io_service& get_io_service() = 0;
	};

	typedef std::shared_ptr<io_service_work> io_service_work_ptr;

	class io_service_pool
		: public io_service_work
	{
	public:
		explicit io_service_pool(std::size_t pool_size);
		~io_service_pool();

		void start();

		bool run();
		intptr_t run_one();
		intptr_t poll();
		intptr_t poll_one();

		void stop();

		ASIO_NS::io_service& get_io_service();

	private:
		using io_service_ptr = std::shared_ptr<ASIO_NS::io_service>;
		using work_ptr = std::shared_ptr<ASIO_NS::io_service::work>;

		std::vector<io_service_ptr> io_services_;
		std::vector<work_ptr> work_;
		std::size_t next_io_service_;
		std::vector<std::shared_ptr<std::thread> > threads_;
	};

	class io_service_inplace 
		: public io_service_work
	{
	public:
		explicit io_service_inplace();
		~io_service_inplace();

		void start();

		bool run();
		intptr_t run_one();
		intptr_t poll();
		intptr_t poll_one();

		void stop();

		ASIO_NS::io_service& get_io_service();

	private:
		using io_service_ptr = std::shared_ptr<ASIO_NS::io_service>;
		using work_ptr = std::shared_ptr<ASIO_NS::io_service::work>;

		io_service_ptr io_services_;
		work_ptr work_;
	};

}
