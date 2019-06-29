// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite_ex.hpp>

#define DEFAULT_EXECUTION_MONITOR_TYPE execution_monitor::use_sleep_only
#include "util.inl"

template <typename M>
struct test_lock
{
    typedef M mutex_type;
    typedef typename M::scoped_lock lock_type;

    void operator()()
    {
        mutex_type mutex;
        boost::condition condition;

        // Test the lock's constructors.
        {
            lock_type lock(mutex, false);
            BOOST_CHECK(!lock);
        }
        lock_type lock(mutex);
        BOOST_CHECK(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::xtime xt = delay(0, 100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_CHECK(!condition.timed_wait(lock, xt));
        BOOST_CHECK(lock ? true : false);

        // Test the lock and unlock methods.
        lock.unlock();
        BOOST_CHECK(!lock);
        lock.lock();
        BOOST_CHECK(lock ? true : false);
    }
};

template <typename M>
struct test_trylock
{
    typedef M mutex_type;
    typedef typename M::scoped_try_lock try_lock_type;

    void operator()()
    {
        mutex_type mutex;
        boost::condition condition;

        // Test the lock's constructors.
        {
            try_lock_type lock(mutex);
            BOOST_CHECK(lock ? true : false);
        }
        {
            try_lock_type lock(mutex, false);
            BOOST_CHECK(!lock);
        }
        try_lock_type lock(mutex, true);
        BOOST_CHECK(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::xtime xt = delay(0, 100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_CHECK(!condition.timed_wait(lock, xt));
        BOOST_CHECK(lock ? true : false);

        // Test the lock, unlock and trylock methods.
        lock.unlock();
        BOOST_CHECK(!lock);
        lock.lock();
        BOOST_CHECK(lock ? true : false);
        lock.unlock();
        BOOST_CHECK(!lock);
        BOOST_CHECK(lock.try_lock());
        BOOST_CHECK(lock ? true : false);
    }
};

template <typename M>
struct test_timedlock
{
    typedef M mutex_type;
    typedef typename M::scoped_timed_lock timed_lock_type;

    void operator()()
    {
        mutex_type mutex;
        boost::condition condition;

        // Test the lock's constructors.
        {
            // Construct and initialize an xtime for a fast time out.
            boost::xtime xt = delay(0, 100);

            timed_lock_type lock(mutex, xt);
            BOOST_CHECK(lock ? true : false);
        }
        {
            timed_lock_type lock(mutex, false);
            BOOST_CHECK(!lock);
        }
        timed_lock_type lock(mutex, true);
        BOOST_CHECK(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::xtime xt = delay(0, 100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_CHECK(!condition.timed_wait(lock, xt));
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK(in_range(xt));

        // Test the lock, unlock and timedlock methods.
        lock.unlock();
        BOOST_CHECK(!lock);
        lock.lock();
        BOOST_CHECK(lock ? true : false);
        lock.unlock();
        BOOST_CHECK(!lock);
        xt = delay(0, 100);
        BOOST_CHECK(lock.timed_lock(xt));
        BOOST_CHECK(lock ? true : false);
    }
};

template <typename M>
struct test_recursive_lock
{
    typedef M mutex_type;
    typedef typename M::scoped_lock lock_type;

    void operator()()
    {
        mutex_type mx;
        lock_type lock1(mx);
        lock_type lock2(mx);
    }
};

void do_test_mutex()
{
    test_lock<boost::mutex>()();
}

void test_mutex()
{
    timed_test(&do_test_mutex, 3);
}

void do_test_try_mutex()
{
    test_lock<boost::try_mutex>()();
    test_trylock<boost::try_mutex>()();
}

void test_try_mutex()
{
    timed_test(&do_test_try_mutex, 3);
}

void do_test_timed_mutex()
{
    test_lock<boost::timed_mutex>()();
    test_trylock<boost::timed_mutex>()();
    test_timedlock<boost::timed_mutex>()();
}

void test_timed_mutex()
{
    timed_test(&do_test_timed_mutex, 3);
}

void do_test_recursive_mutex()
{
    test_lock<boost::recursive_mutex>()();
    test_recursive_lock<boost::recursive_mutex>()();
}

void test_recursive_mutex()
{
    timed_test(&do_test_recursive_mutex, 3);
}

void do_test_recursive_try_mutex()
{
    test_lock<boost::recursive_try_mutex>()();
    test_trylock<boost::recursive_try_mutex>()();
    test_recursive_lock<boost::recursive_try_mutex>()();
}

void test_recursive_try_mutex()
{
    timed_test(&do_test_recursive_try_mutex, 3);
}

void do_test_recursive_timed_mutex()
{
    test_lock<boost::recursive_timed_mutex>()();
    test_trylock<boost::recursive_timed_mutex>()();
    test_timedlock<boost::recursive_timed_mutex>()();
    test_recursive_lock<boost::recursive_timed_mutex>()();
}

void test_recursive_timed_mutex()
{
    timed_test(&do_test_recursive_timed_mutex, 3);
}

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[])
{
    boost::unit_test_framework::test_suite* test =
        BOOST_TEST_SUITE("Boost.Threads: mutex test suite");

    test->add(BOOST_TEST_CASE(&test_mutex));
    test->add(BOOST_TEST_CASE(&test_try_mutex));
    test->add(BOOST_TEST_CASE(&test_timed_mutex));
    test->add(BOOST_TEST_CASE(&test_recursive_mutex));
    test->add(BOOST_TEST_CASE(&test_recursive_try_mutex));
    test->add(BOOST_TEST_CASE(&test_recursive_timed_mutex));

    return test;
}
