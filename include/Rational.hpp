/* -*- c++ -*- */
#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <iostream>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <limits>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

template <typename T> class Rational;

namespace std {
template <typename T>
inline T floor(const T x)
{ return x; }

template <typename T>
inline T abs(const T &x)
{ return x < 0 ? -x : x; }
#define __UNSIGNED_ABS(T) inline unsigned T abs(unsigned T x) {return x;}
__UNSIGNED_ABS(char)
__UNSIGNED_ABS(short)
__UNSIGNED_ABS(int)
__UNSIGNED_ABS(long)
__UNSIGNED_ABS(long long)
#undef __UNSIGNED_ABS
template <typename T>
inline Rational<T> abs(const Rational<T> &value) {
	Rational<T> r(abs(value.num()), abs(value.denom()));
	return r;
}

template<typename T>
inline T fmod(T a, T b) { return a % b; }
template<>
inline long double fmod(long double a, long double b) { return fmodl(a, b); }
template<>
inline float fmod(float a, float b) { return fmod((double)a, (double)b); }

template <typename T>
inline T gcd(T a, T b)
{
	a = abs(a);
        b = abs(b);
	while (b > 0) {
		T t = b;
		b = fmod(a, b);
		a = t;
	}
	return a;
}

template <typename T>
void simplify_bounded(T &anum, T &adenom, const T max_val)
{
        using namespace std;
        if (anum == 0) {
                adenom = 1;
                return;
        }
        bool neg = (anum < 0) != (adenom < 0);
        T num = abs(anum), denom = abs(adenom);
        if (num / denom >= max_val) {
                anum = neg ? -max_val : max_val;
                adenom = 1;
                return;
        }
        if (denom / num > max_val) {
                if (denom / num > 2 * max_val) {
                        anum = 0;
                        adenom = 1;
                } else {
                        anum = neg ? -1 : 1;
                        adenom = max_val;
                }
                return;
        }
        T p0, q0, p1, q1, p2, q2;
        p0 = 1;
        q0 = 0;
        T rem = fmod(num, denom);
        p1 = floor(num / denom);
        q1 = 1;
        num = denom;
        denom = rem;
        while (denom > 0) {
                rem = fmod(num, denom);
                T quot = floor(num / denom);
                p2 = quot * p1 + p0;
                if (p2 > max_val)
                        break;
                q2 = quot * q1 + q0;
                if (q2 > max_val)
                        break;
                q0 = q1;
                q1 = q2;
                p0 = p1;
                p1 = p2;
                num = denom;
                denom = rem;
        }
        anum = neg ? -p1 : p1;
        adenom = q1;
}

} //namespace std

#define __RAT_ALL_INTS(macro)			\
	macro(int)				\
	macro(short)				\
	macro(long)				\
	macro(long long)			\
	macro(unsigned int)			\
	macro(unsigned short)			\
	macro(unsigned long)			\
	macro(unsigned long long)

#define __RAT_ALL_FLOATS(macro)			\
	macro(float)				\
	macro(double)				\
	macro(long double)


#define __RAT_PRINT_INT(T)                              \
inline void print_num(std::FILE *f, const T num)        \
{ std::fprintf(f, "%Ld", (long long)num); }

#define __RAT_ALL_INTS_EX_LL(macro)		\
	macro(int)				\
	macro(short)				\
	macro(long)				\
	macro(long long)			\
	macro(unsigned int)			\
	macro(unsigned short)			\
	macro(unsigned long)

#define __RAT_PRINT_FLOAT(T)                            \
inline void print_num(std::FILE *f, const T num)        \
{ std::fprintf(f, "%.0Lf", (long double)std::floor(num));  }

__RAT_ALL_INTS_EX_LL(__RAT_PRINT_INT)

inline void print_num(std::FILE *f, const unsigned long long num)
{ std::fprintf(f, "%Lu", num); }

__RAT_ALL_FLOATS(__RAT_PRINT_FLOAT)

template<typename T>
inline void check_nd_bounds_int(T &num, T &denom)
{
	const T MAXVAL = T(1) << T((sizeof(T) * 4 - 2));
        while (denom > T(1) &&
	       (std::abs(num) > MAXVAL ||
		std::abs(denom) > MAXVAL)) {
	        num /= 2;
		denom /= 2;
        }
}
template<typename T>
inline void check_nd_bounds_float(T &num, T &denom)
{
        const double MAXVAL = 1e153;
        using std::abs;
        using std::floor;
        T t = std::max(abs(num), denom);
        if (t > MAXVAL) {
                t /= MAXVAL;
                if (floor(denom / t) == 0.0) {
                        num = floor(num / denom);
                        denom = 1;
                } else {
                        num = floor(num / t);
                        if (num == 0)
                                denom = 1;
                        else
                                denom = floor(denom / t);
                }
        }
}

template <typename T>
inline void check_nd_bounds(T &num, T &denom);

#define __RAT_INT_ND_BOUNDS(T)					\
template<> inline void check_nd_bounds(T &num, T &denom)	\
{ check_nd_bounds_int(num, denom); }

#define __RAT_FLOAT_ND_BOUNDS(T)				\
template<> inline void check_nd_bounds(T &num, T &denom)	\
{ check_nd_bounds_float(num, denom); }

__RAT_ALL_INTS(__RAT_INT_ND_BOUNDS)
__RAT_ALL_FLOATS(__RAT_FLOAT_ND_BOUNDS)

template <typename T = long long> class Rational {
        T num_, denom_;
        typedef std::numeric_limits<T> limit;
        static const T max_stored() {
                return T(limit::radix) << T(limit::digits / 2 - 1);
        }
public:
        T &num() { return num_; }
        const T &num() const { return num_; }
        void set_denom(const T &value) { denom_ = value; simplify(); }
        const T &denom() const { return denom_; }
        void assign(T num, T denom = T(1)) { denom_ = denom; num_ = num; simplify(); }

	template<typename U>
	void assign_float(U value) {
                using std::floor;
                using std::pow;
                if (limit::max() / max_stored() < abs(value)) {
                        num_ = T(floor(limit::max() / 4));
                        denom_ = T(floor(num_ / value));
                        if (denom_ < 0) {
                                denom_ *= -1;
                                num_ *= -1;
                        }
                        if (denom_ < 1)
                                denom_ = 1;
                } else {
                        denom_ = max_stored();
        		num_ = T(floor(value * denom_));
                }
                simplify();
	}

	Rational() { num_ = T(0); denom_ = T(1); }
	Rational(float value) { assign_float(value); }
	Rational(double value) { assign_float(value); }
	Rational(long double value) { assign_float(value); }
	Rational(T n, T d) { assign(n, d); }

	template<typename U>
	Rational(const U &value) {
		num_ = value;
		denom_ = 1;
	}
	
 	template <typename T1> Rational(const Rational<T1> &value) {
		num_ = value.num_;
		denom_ = value.denom_;
	}

	template <typename T1> Rational &operator=(const Rational<T1> &r) {
		num_ = r.num_;
		denom_ = r.denom_;
		return *this;
	}
        template<typename U>
	Rational &operator=(const U &r) {
		*this = Rational<T>(r);
		return *this;
	}
        /*
        operator double() const {
		return (double)num_ / denom_;
	}  */
        template<typename U>
        T convert() const {
                return (T)num_ / (T)denom_;
        }

	void printf(std::FILE *f, bool braces = true) const {
                braces &= *this < Rational<T>(0);
		if (braces)
			fprintf(f, "(");
		if (*this < Rational<T>(0))
			fprintf(f, "-");
		if (denom_ != 1)
			fprintf(f, "<sup>");
                print_num(f, std::abs(num_));
                if (denom_ != 1) {
        	        fprintf(f, "</sup>/<sub>");
                        print_num(f, denom_);
                        fprintf(f, "</sub>");
                }
		if (braces)
			fprintf(f, ")");
	}
	void printf_cool(std::FILE *f) const {
                T anum = std::abs(num_), adenom = std::abs(denom_);
                if (anum == 0) {
                        fputc('0', f);
                        return;
                }
                if (*this < 0)
                        fprintf(f, "-");
                if (anum >= adenom)
                        print_num(f, anum / adenom);
                if (fmod(anum, adenom)) {
                        if (anum >= adenom)
                                fputs("&nbsp", f);
        		fprintf(f, "<sup>");
                        print_num(f, fmod(anum, adenom));
        	        fprintf(f, "</sup>/<sub>");
                        print_num(f, denom_);
                        fprintf(f, "</sub>");
                }
	}
#undef FORMAT_SPEC
#undef TYPE_CONV 
	void simplify() {
                using std::abs;
                if (denom_ <= T(0)) {
                        num_ = -num_;
                        denom_ = -denom_;
                }
                if (num_ == 0)
                        denom_ = 1;
                else
                        if (abs(num_) > max_stored() || abs(denom_) > max_stored())
                                simplify_bounded(num_, denom_, max_stored());
	}

	Rational<T> operator+() const {
		return *this;
	}
	Rational<T> operator-() const {
		Rational<T> tmp = *this;
		tmp.num_ *= -1;
		return tmp;
	}
	template <typename T1> Rational<T> &operator+=(const Rational<T1> &r) {
		num_ = num_ * r.denom_ + r.num_ * denom_;
		denom_ *= r.denom_;
		simplify();
		return *this;
	}

	template <typename T1> Rational<T> &operator-=(const Rational<T1> &r) {
		num_ = num_ * r.denom_ - r.num_ * denom_;
		denom_ *= r.denom_;
		simplify();
		return *this;
	}

	template <typename T1> Rational<T> &operator*=(const Rational<T1> &r) {
		num_ *= r.num_;
		denom_ *= r.denom_;
		simplify();
		return *this;
	}
	
	template <typename T1> Rational<T> &operator/=(const Rational<T1> &r) {
                assert(r.num() != 0);
		num_ *= r.denom_;
		denom_ *= r.num_;
		simplify();
		return *this;
	}
};

/*
  common defines
 */
#define __RAT_ALL_ARITH_OPERS(macro, U)		\
	macro(U, +)				\
	macro(U, -)				\
	macro(U, *)				\
	macro(U, /)
/*
  unary operators: op=
 */
#define __RAT_INT_ARITH_OPERS(U)				\
template<typename T>					\
inline Rational<T> &operator+=(Rational<T> &l, U r)	\
{							\
	l.num() += r * l.denom();			\
	return l;					\
}							\
template<typename T>					\
inline Rational<T> &operator-=(Rational<T> &l, U r)	\
{							\
	l.num() -= r * l.denom();			\
	return l;					\
}							\
template<typename T>					\
inline Rational<T> &operator*=(Rational<T> &l, U r)	\
{							\
	l.num() *= r;					\
	l.simplify();					\
	return l;					\
}							\
template<typename T>					\
inline Rational<T> &operator/=(Rational<T> &l, U r)	\
{   assert(r != 0);							\
	l.set_denom(l.denom() * r);			\
	return l;					\
}

#define __RAT_FLOAT_ONE(U, op)				\
template<typename T>					\
inline Rational<T> &operator op##=(Rational<T> &l, U r)	\
{							\
	return l op##= Rational<T>(r);			\
}							

#define __RAT_FLOAT_ARITH_OPERS(U) __RAT_ALL_ARITH_OPERS(__RAT_FLOAT_ONE, U)
	
__RAT_ALL_INTS(__RAT_INT_ARITH_OPERS)
__RAT_ALL_FLOATS(__RAT_FLOAT_ARITH_OPERS)

/*
  binary operators: Rational and Rational
*/
#define __RAT_BINARY_ARITH_OPER_RR(Unused, oper)			\
template <typename T, typename U>				\
inline Rational<T>						\
operator oper(const Rational<T> &l, const Rational<U> &r)	\
{								\
	Rational<T> t = l;					\
	return t oper##= r;					\
}

__RAT_ALL_ARITH_OPERS(__RAT_BINARY_ARITH_OPER_RR, Unused)

/*
  binary operators: Rational and primitive
*/
#define __RAT_BINARY_ARITH_OPER_RP(U, oper)				\
template <typename T>						\
inline Rational<T> operator oper(const Rational<T> &l, U r)	\
{								\
	Rational<T> t = l;					\
	return t oper##= r;					\
}

#define __RAT_BINARY_ARITH_OPERS_RP(U)		\
	__RAT_BINARY_ARITH_OPER_RP(U, +)		\
	__RAT_BINARY_ARITH_OPER_RP(U, -)		\
	__RAT_BINARY_ARITH_OPER_RP(U, *)		\
	__RAT_BINARY_ARITH_OPER_RP(U, /)
	
__RAT_ALL_INTS(__RAT_BINARY_ARITH_OPERS_RP)
__RAT_ALL_FLOATS(__RAT_BINARY_ARITH_OPERS_RP)

/*
  binary operators: primitive and Rational
*/
/*
  commutative: + and *
*/
#define __RAT_BINARY_ARITH_OPER_COMMUT_PR(U, oper)			\
template <typename T>						\
inline Rational<T> operator oper(U l, const Rational<T> &r)	\
{								\
	Rational<T> t = r;					\
	return t oper##= l;					\
}
#define __RAT_BINARY_ARITH_OPERS_COMMUT_PR(U)		\
	__RAT_BINARY_ARITH_OPER_COMMUT_PR(U, +)	\
	__RAT_BINARY_ARITH_OPER_COMMUT_PR(U, *)			

__RAT_ALL_INTS(__RAT_BINARY_ARITH_OPERS_COMMUT_PR)
__RAT_ALL_FLOATS(__RAT_BINARY_ARITH_OPERS_COMMUT_PR)

/*
  noncommutative: - and /
*/
#define __RAT_BINARY_OPERS_INT_PR(U)				\
template <typename T>						\
inline Rational<T> operator-(U l, const Rational<T> &r)		\
{								\
	return Rational<T>(l * r.denom() - r.num(), r.denom()); \
}								\
template <typename T>						\
inline Rational<T> operator/(U l, const Rational<T> &r)		\
{								\
	return Rational<T>(l * r.denom(), r.num());		\
}

#define __RAT_BINARY_OPERS_FLOAT_PR(U)			\
template <typename T>					\
inline Rational<T> operator-(U l, const Rational<T> &r)	\
{							\
	return Rational<T>(l) - r;			\
}							\
template <typename T>					\
inline Rational<T> operator/(U l, const Rational<T> &r)	\
{							\
	return Rational<T>(l) / r;			\
}
__RAT_ALL_INTS(__RAT_BINARY_OPERS_INT_PR)
__RAT_ALL_FLOATS(__RAT_BINARY_OPERS_FLOAT_PR)

/*
  Comparing
 */
#define __RAT_ALL_CMP_OPERS(macro, U)		\
	macro(U, ==)				\
	macro(U, !=)				\
	macro(U, <)				\
	macro(U, >)				\
	macro(U, <=)				\
	macro(U, >=)

/*
  binary operators: Rational and Rational
*/
#define __RAT_BINARY_CMP_OPER_RR(Unused, oper)				\
template <typename T, typename U>					\
inline bool operator oper(const Rational<T> &r0, const Rational<U> &r1)	\
{									\
	return r0.num() * r1.denom() oper r1.num() * r0.denom();	\
}

__RAT_ALL_CMP_OPERS(__RAT_BINARY_CMP_OPER_RR, Unused)

/*
  binary operators: (Rational and primitive) or (primitive and Rational)
*/
#define __RAT_BINARY_CMP_OPER_INT(U, oper)		\
template <typename T>					\
inline bool operator oper(const Rational<T> &l, U r)	\
{							\
	return l.num() oper r * l.denom();		\
}							\
template <typename T>					\
inline bool operator oper(U l, const Rational<T> &r)	\
{							\
	return l * r.denom() oper r.num();		\
}

#define __RAT_BINARY_CMP_OPERS_INT(U)				\
	__RAT_ALL_CMP_OPERS(__RAT_BINARY_CMP_OPER_INT, U)

#define __RAT_BINARY_CMP_OPER_FLOAT(U, oper)		\
template <typename T>					\
inline bool operator oper(const Rational<T> &l, U r)	\
{							\
	return l oper Rational<U>(r);			\
}							\
template <typename T>					\
inline bool operator oper(U l, const Rational<T> &r)	\
{							\
	return Rational<U>(l) oper r;			\
}

#define __RAT_BINARY_CMP_OPERS_FLOAT(U)				\
	__RAT_ALL_CMP_OPERS(__RAT_BINARY_CMP_OPER_FLOAT, U)

__RAT_ALL_INTS(__RAT_BINARY_CMP_OPERS_INT)
__RAT_ALL_FLOATS(__RAT_BINARY_CMP_OPERS_FLOAT)

template<typename T, typename _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os, const Rational<T> &r)
{
        os << r.num();
        if (r.denom() != 1)
                os << '/' << r.denom();
        return os;
}
#endif /*RATIONAL_HPP*/


