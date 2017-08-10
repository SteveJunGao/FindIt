#include <thread>
#include <mutex>
#include <unordered_map>
#pragma once

using namespace std;

#pragma deprecated(safe_unordered_map)
template<class _Kty, class _Ty, class _Hasher = hash<_Kty>, class _Keyeq = equal_to<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty>>>
class safe_unordered_map : protected unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc> {
// use protected inheritance to prevent direct access to unsafe interface
public:
	typedef unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc> Unsafe;
	mutex mtx;

	mapped_type& operator[](const key_type& _Keyval)
	{
		lock_guard<mutex> guard(mtx);
		return Unsafe::operator[](_Keyval);
	}

	size_type erase(const key_type& _Keyval)
	{
		lock_guard<mutex> guard(mtx);
		return Unsafe::erase(_Keyval);
	}

	iterator find(const key_type& _Keyval)
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::find(_Keyval);
	}

	iterator end() _NOEXCEPT
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::end();
	}
};

#pragma deprecated(safe_vector)
template<class _Ty, class _Alloc = allocator<_Ty>>
class safe_vector : protected vector<_Ty, _Alloc> {
// use protected inheritance to prevent direct access to unsafe interface
public:
	typedef vector<_Ty, _Alloc> Unsafe;
	mutex mtx;

	void push_back(value_type& _Val)
	{
		lock_guard<mutex> guard(mtx);
		Unsafe::push_back(_Val);
	}

	reference operator[](size_type _Pos)
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::operator[](_Pos);
	}

	void resize(size_type _Newsize)
	{
		lock_guard<mutex> guard(mtx);
		Unsafe::resize(_Newsize);
	}

	iterator begin() _NOEXCEPT
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::begin();
	}

	iterator end() _NOEXCEPT
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::end();
	}

	size_type size() const _NOEXCEPT
	{
		// lock_guard<mutex> guard(mtx);
		return Unsafe::size();
	}

	void clear() _NOEXCEPT
	{
		lock_guard<mutex> guard(mtx);
		return Unsafe::clear();
	}
};