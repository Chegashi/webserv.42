#ifndef __PRINT_HPP__
# define __PRINT_HPP__

#include <iostream>
#include <string>

template <class A>
void print(const A &a) {
	std::cout << a;
}

template <class A, class B>
void print(const A &a, const B &b) {
	std::cout << a << b;
}

template <class A, class B, class C>
void print(const A &a, const B &b, const C &c) {
	std::cout << a << b << c;
}

template <class A, class B, class C, class D>
void print(const A &a, const B &b, const C &c, const D &d) {
	std::cout << a << b << c << d;
}

template <class A, class B, class C, class D, class E>
void print(const A &a, const B &b, const C &c, const D &d, const E &e) {
	std::cout << a << b << c << d << e;
}

template <class A, class B, class C, class D, class E, class F>
void print(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
	std::cout << a << b << c << d << e << f;
}

template <class A, class B, class C, class D, class E, class F, class G>
void print(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g) {
	std::cout << a << b << c << d << e << f << g;
}

template <class A, class B, class C, class D, class E, class F, class G, class H>
void print(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h) {
	std::cout << a << b << c << d << e << f << g << h;
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
void print(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h, const I &i) {
	std::cout << a << b << c << d << e << f << g << h << i;
}

template <class A>
void println(const A &a) {
	std::cout << a << std::endl;
}

template <class A, class B>
void println(const A &a, const B &b) {
	std::cout << a << b << std::endl;
}

template <class A, class B, class C>
void println(const A &a, const B &b, const C &c) {
	std::cout << a << b << c << std::endl;
}

template <class A, class B, class C, class D>
void println(const A &a, const B &b, const C &c, const D &d) {
	std::cout << a << b << c << d << std::endl;
}

template <class A, class B, class C, class D, class E>
void println(const A &a, const B &b, const C &c, const D &d, const E &e) {
	std::cout << a << b << c << d << e << std::endl;
}

template <class A, class B, class C, class D, class E, class F>
void println(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f) {
	std::cout << a << b << c << d << e << f << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G>
void println(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g) {
	std::cout << a << b << c << d << e << f << g << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G, class H>
void println(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h) {
	std::cout << a << b << c << d << e << f << g << h << std::endl;
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
void println(const A &a, const B &b, const C &c, const D &d, const E &e, const F &f, const G &g, const H &h, const I &i) {
	std::cout << a << b << c << d << e << f << g << h << i << std::endl;
}

#endif
