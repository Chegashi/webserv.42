#ifndef __LIST_HPP__
# define __LIST_HPP__

# include <vector>

template <class A>
std::vector<A> list() {
	std::vector<A> ret;
	return ret;
}

template <class A>
std::vector<A> list(const A &a) {
	std::vector<A> ret;
	ret.push_back(a);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d, const A &e) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	ret.push_back(e);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d, const A &e, const A &f) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	ret.push_back(e);
	ret.push_back(f);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d, const A &e, const A &f, const A &g) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	ret.push_back(e);
	ret.push_back(f);
	ret.push_back(g);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d, const A &e, const A &f, const A &g, const A &h) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	ret.push_back(e);
	ret.push_back(f);
	ret.push_back(g);
	ret.push_back(h);
	return ret;
}

template <class A>
std::vector<A> list(const A &a, const A &b, const A &c, const A &d, const A &e, const A &f, const A &g, const A &h, const A &i) {
	std::vector<A> ret;
	ret.push_back(a);
	ret.push_back(b);
	ret.push_back(c);
	ret.push_back(d);
	ret.push_back(e);
	ret.push_back(f);
	ret.push_back(g);
	ret.push_back(h);
	ret.push_back(i);
	return ret;
}

#endif
