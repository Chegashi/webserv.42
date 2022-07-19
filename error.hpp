#ifndef __ERROR_HPP__
# define __ERROR_HPP__

#include <iostream>

template <class A>
void error(const A &a) {
	std::cerr << a;
}

template <class A, class B>
void error(const A &a, const B &b) {
	std::cerr << a << b;
}

template <class A, class B, class C>
void error(const A &a, const B &b, const C &c) {
	std::cerr << a << b << c;
}

template <class A, class B, class C, class D>
void error(const A &a, const B &b, const C &c, const D &d) {
	std::cerr << a << b << c << d;
}

template <class A, class B, class C, class D, class E>
void error(const A &a, const B &b, const C &c, const D &d, const E &e) {
	std::cerr << a << b << c << d << e;
}

template <class A, class B, class C, class D, class E, class F>
void error(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
	std::cerr << a << b << c << d << e << f;
}

template <class A, class B, class C, class D, class E, class F, class G>
void error(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g) {
	std::cerr << a << b << c << d << e << f << g;
}

template <class A, class B, class C, class D, class E, class F, class G, class H>
void error(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h) {
	std::cerr << a << b << c << d << e << f << g << h;
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
void error(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h, const I &i) {
	std::cerr << a << b << c << d << e << f << g << h << i;
}

template <class A>
void errorln(const A &a) {
	std::cerr << a << std::endl;
}

template <class A, class B>
void errorln(const A &a, const B &b) {
	std::cerr << a << b << std::endl;
}

template <class A, class B, class C>
void errorln(const A &a, const B &b, const C &c) {
	std::cerr << a << b << c << std::endl;
}

template <class A, class B, class C, class D>
void errorln(const A &a, const B &b, const C &c, const D &d) {
	std::cerr << a << b << c << d << std::endl;
}

template <class A, class B, class C, class D, class E>
void errorln(const A &a, const B &b, const C &c, const D &d, const E &e) {
	std::cerr << a << b << c << d << e << std::endl;
}

template <class A, class B, class C, class D, class E, class F>
void errorln(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
	std::cerr << a << b << c << d << e << f << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G>
void errorln(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g) {
	std::cerr << a << b << c << d << e << f << g << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G, class H>
void errorln(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h) {
	std::cerr << a << b << c << d << e << f << g << h << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
void errorln(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h, const I &i) {
	std::cerr << a << b << c << d << e << f << g << h << i << std::endl;
}

#endif
