/*
 * async_handler.hpp
 *
 *  Created on: 12.06.2016
 *      Author: Klemens
 */

#ifndef BOOST_PROCESS_DETAIL_ASYNC_HANDLER_HPP_
#define BOOST_PROCESS_DETAIL_ASYNC_HANDLER_HPP_

#include <type_traits>

#if defined(BOOST_POSIX_API)
#include <boost/process/posix.hpp>
#include <boost/process/detail/posix/async_handler.hpp>
#include <boost/process/detail/posix/asio_fwd.hpp>
#else
#include <boost/process/detail/windows/async_handler.hpp>
#include <boost/process/detail/windows/asio_fwd.hpp>
#endif

namespace boost {

namespace process {

namespace detail {

#if defined(BOOST_POSIX_API)
using ::boost::process::detail::posix::is_async_handler;
#else
using ::boost::process::detail::windows::is_async_handler;
#endif

template<typename ...Args>
struct has_io_service;

template<typename T, typename ...Args>
struct has_io_service<T, Args...>
{
    typedef typename has_io_service<Args...>::type next;
    typedef typename std::is_same<
                typename std::remove_reference<T>::type,
                boost::asio::io_service>::type is_ios;
    typedef typename std::conditional<is_ios::value,
            std::true_type,
            next>::type type;
};

template<typename T>
struct has_io_service<T>
{
    typedef typename std::is_same<
            typename std::remove_reference<T>::type,
            boost::asio::io_service>::type type;
};

template<typename ...Args>
using has_io_service_t = typename has_io_service<Args...>::type;

template<typename ...Args>
struct has_async_handler;

template<typename T, typename ...Args>
struct has_async_handler<T, Args...>
{
    typedef typename has_async_handler<Args...>::type next;
    typedef typename is_async_handler<T>::type is_ios;
    typedef typename std::conditional<is_ios::value,
            std::true_type,
            next>::type type;
};

template<typename T>
struct has_async_handler<T>
{
    typedef typename is_async_handler<T>::type type;
};


template<typename T>
struct is_yield_context
{
    typedef std::false_type type;
};

template<typename T>
struct is_yield_context<::boost::asio::basic_yield_context<T>>
{
    typedef std::true_type type;
};

template<typename ...Args>
struct has_yield_context;

template<typename T, typename ...Args>
struct has_yield_context<T, Args...>
{
    typedef typename has_yield_context<Args...>::type next;
    typedef typename is_yield_context<
            typename std::remove_reference<T>::type>::type is_ios;
    typedef typename std::conditional<is_ios::value,
            std::true_type,
            next>::type type;
};

template<typename T>
struct has_yield_context<T>
{
    typedef typename is_yield_context<
            typename std::remove_reference<T>::type>::type type;
};


template<typename ...Args>
boost::asio::io_service &get_io_service_var(boost::asio::io_service & f, Args&...args)
{
    return f;
}

template<typename First, typename ...Args>
boost::asio::io_service &get_io_service_var(First & f, Args&...args)
{
    return get_io_service_var(args...);
}

}
}
}


#endif /* BOOST_PROCESS_DETAIL_ASYNC_HANDLER_HPP_ */
