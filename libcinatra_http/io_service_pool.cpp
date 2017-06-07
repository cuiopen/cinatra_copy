
#include <cinatra_http/http_server.h>

#include <boost/bind.hpp>

#include <stdexcept>


namespace cinatra
{
	io_service_work::~io_service_work()
	{

	}




	io_service_pool::~io_service_pool()
	{

	}

	io_service_pool::io_service_pool(std::size_t pool_size)
		: next_io_service_(0)
	{
		if (pool_size == 0)
			throw std::runtime_error("io_service_pool size is 0");

		for (std::size_t i = 0; i < pool_size; ++i)
		{
			io_service_ptr io_service(new ASIO_NS::io_service);
			work_ptr work(new ASIO_NS::io_service::work(*io_service));
			io_services_.push_back(io_service);
			work_.push_back(work);
		}
	}

	void io_service_pool::start()
	{
		threads_.clear();

		for (std::size_t i = 0; i < io_services_.size(); ++i)
		{
			threads_.emplace_back(std::make_shared<std::thread>(boost::bind(&ASIO_NS::io_service::run, io_services_[i])));
		}
	}

	bool io_service_pool::run()
	{
		for (std::size_t i = 0; i < threads_.size(); ++i)
			threads_[i]->join();
		return true;
	}

	intptr_t io_service_pool::run_one()
	{
		return -1;
	}

	intptr_t io_service_pool::poll()
	{
		return -1;
	}

	intptr_t io_service_pool::poll_one()
	{
		return -1;
	}

	void io_service_pool::stop()
	{
		work_.clear();

		for (std::size_t i = 0; i < io_services_.size(); ++i)
			io_services_[i]->stop();

		for (std::size_t i = 0; i < threads_.size(); ++i)
			threads_[i]->join();
		threads_.clear();
	}

	ASIO_NS::io_service& io_service_pool::get_io_service()
	{
		ASIO_NS::io_service& io_service = *io_services_[next_io_service_];
		++next_io_service_;
		if (next_io_service_ == io_services_.size())
			next_io_service_ = 0;
		return io_service;
	}





	io_service_inplace::io_service_inplace()
	{
		io_services_ = std::make_shared<ASIO_NS::io_service>();
		work_ = std::make_shared<ASIO_NS::io_service::work>(*io_services_);
	}

	io_service_inplace::~io_service_inplace()
	{

	}

	void io_service_inplace::start()
	{
	}

	bool io_service_inplace::run()
	{
		io_services_->run();
		return true;
	}

	intptr_t io_service_inplace::run_one()
	{
		return io_services_->run_one();
	}

	intptr_t io_service_inplace::poll()
	{
		return io_services_->poll();
	}

	intptr_t io_service_inplace::poll_one()
	{
		return io_services_->poll_one();
	}

	void io_service_inplace::stop()
	{
		work_ = nullptr;

		if (io_services_)
		{
			io_services_->run();
			io_services_->stop();

			io_services_ = nullptr;
		}
	}

	ASIO_NS::io_service& io_service_inplace::get_io_service()
	{
		return *io_services_;
	}
}
