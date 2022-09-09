#ifndef __THROWED_HPP__
# define __THROWED_HPP__

template <class Return>
bool throwed(Return (*f)()) {
        try { f(); } catch(...) { return true; } return false;
}

template <class Return, class Arg0>
bool throwed(Return (*f)(Arg0), const Arg0 &arg0) {
        try { f(arg0); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1>
bool throwed(Return (*f)(Arg0, Arg1), const Arg0 &arg0, const Arg1 &arg1) {
        try { f(arg0, arg1); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2>
bool throwed(Return (*f)(Arg0, Arg1, Arg2), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2) {
        try { f(arg0, arg1, arg2); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3) {
        try { f(arg0, arg1, arg2, arg3); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3, Arg4), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4) {
        try { f(arg0, arg1, arg2, arg3, arg4); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5) {
        try { f(arg0, arg1, arg2, arg3, arg4, arg5); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const Arg6 &arg6) {
        try { f(arg0, arg1, arg2, arg3, arg4, arg5, arg6); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const Arg6 &arg6, const Arg7 &arg7) {
        try { f(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); } catch(...) { return true; } return false;
}

template <class Return, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
bool throwed(Return (*f)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8), const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const Arg6 &arg6, const Arg7 &arg7, const Arg8 &arg8) {
        try { f(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); } catch(...) { return true; } return false;
}

#endif
