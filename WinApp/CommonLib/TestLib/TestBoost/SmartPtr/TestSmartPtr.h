#pragma once
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/checked_delete.hpp>
#include <boost/detail/atomic_count.hpp>
#include <iostream>

struct Shoe { ~Shoe() { std::cout << "Buckle my shoe\n"; } };

class MyClass {
	boost::scoped_ptr<int> ptr;
public:
	MyClass() : ptr(new int) { *ptr = 0; }
	int add_one() { return ++*ptr; }
};


template<class T>
class intrusive_ptr_base{
public:
	intrusive_ptr_base() : ref_count(0) {
		std::cout << "Default constructor" << std::endl;
	}

	intrusive_ptr_base(intrusive_ptr_base<T> const&) : ref_count(0) {
		std::cout << "Copy constructor..." << std::endl;
	}

	intrusive_ptr_base& operator=(intrusive_ptr_base const& rhs) {
		std::cout << "Assignment operator..." << std::endl;
		return *this;
	}

	friend void intrusive_ptr_add_ref(intrusive_ptr_base<T> const* s) {
		std::cout << "intrusive_ptr_add_ref..." << std::endl;
		assert(s->ref_count >= 0);
		assert(s != 0);
		++s->ref_count;
	}

	friend void intrusive_ptr_release(intrusive_ptr_base<T> const* s) {
		std::cout << "intrusive_ptr_release..." << std::endl;
		assert(s->ref_count > 0);
		assert(s != 0);
		if (--s->ref_count == 0)
			boost::checked_delete(static_cast<T const*>(s));
	}

	boost::intrusive_ptr<T> self() {
		return boost::intrusive_ptr<T>((T*)this);
	}

	boost::intrusive_ptr<const T> self() const {
		return boost::intrusive_ptr<const T>((T const*)this);
	}

	int refcount() const {
		return ref_count;
	}

private:
	///should be modifiable even from const intrusive_ptr objects
	mutable boost::detail::atomic_count ref_count;

};


struct Shoe2 : public intrusive_ptr_base<Shoe2> { ~Shoe2() { std::cout << "Buckle my shoe\n"; } };

inline void TestSmartPtr()
{
	{
		boost::scoped_ptr<Shoe> x(new Shoe);
		//boost::scoped_ptr<Shoe> y = x;
		boost::scoped_ptr<Shoe> z;
		z.swap(x);
		z.reset(x.get());

		MyClass my_instance;
		std::cout << my_instance.add_one() << '\n';
		std::cout << my_instance.add_one() << '\n';
	}

	{
		boost::scoped_array<Shoe> x(new Shoe[5]);
	}

	Shoe* r = new Shoe;
	{
		
		boost::shared_ptr<Shoe> x1(r);
		boost::shared_ptr<Shoe[]> y(new Shoe[5]);
	}
	{
	//	boost::shared_ptr<Shoe> x2(r);
	}

	{
		boost::shared_array<Shoe> x(new Shoe[5]);
	}

	boost::intrusive_ptr<Shoe2> r2 = new Shoe2;
	{
		boost::intrusive_ptr<Shoe2> x1(r2);
	}
	{
		boost::intrusive_ptr<Shoe2> x1(r2);
	}
}