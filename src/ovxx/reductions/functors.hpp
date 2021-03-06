//
// Copyright (c) 2006 by CodeSourcery
// Copyright (c) 2013 Stefan Seefeld
// All rights reserved.
//
// This file is part of OpenVSIP. It is made available under the
// license contained in the accompanying LICENSE.BSD file.

#ifndef ovxx_reductions_functors_hpp_
#define ovxx_reductions_functors_hpp_

#include <ovxx/support.hpp>
#include <ovxx/reductions/types.hpp>

namespace ovxx
{
namespace dispatcher
{
namespace op
{
// Evaluator OpTag for value reductions.
template <template <typename> class R> struct reduce;
// Evaluator OpTag for reductions returning index.
template <template <typename> class R> struct reduce_idx;
} // namespace ovxx::dispatcher::op
} // namespace ovxx::dispatcher

template <typename T>
struct All_true
{
  static reduction_type const rtype = reduce_all_true;

  typedef T result_type;
  typedef T accum_type;

  static accum_type initial() { return ~(accum_type());}

  static accum_type update(accum_type state, T new_value)
  { return band(state, new_value);}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type state) { return state == T();}
};

template <>
struct All_true<bool>
{
  static reduction_type const rtype = reduce_all_true_bool;

  typedef bool result_type;
  typedef bool accum_type;

  static bool initial() { return true;}

  static bool update(bool state, bool new_value)
  { return land(state, new_value);}

  static bool value(bool state, length_type)
  { return state;}

  static bool done(bool state) { return state == false;}
};

template <typename T>
struct Any_true
{
  static reduction_type const rtype = reduce_any_true;

  typedef T result_type;
  typedef T accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return bor(state, new_value);}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type state) { return state == ~(T());}
};

template <>
struct Any_true<bool>
{
  static reduction_type const rtype = reduce_any_true_bool;

  typedef bool result_type;
  typedef bool accum_type;

  static bool initial() { return false;}

  static bool update(bool state, bool new_value)
  { return lor(state, new_value);}

  static bool value(bool state, length_type)
  { return state;}

  static bool done(bool state) { return state == true;}
};

template <typename T, typename R = T>
struct Mean_value_base
{
  static reduction_type const rtype = reduce_sum;

  typedef R result_type;
  typedef R accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return state + new_value;}

  static accum_type value(accum_type state, length_type size)
  { return state / static_cast<accum_type>(size);}

  static bool done(accum_type) { return false;}
};

template <typename T>
struct Mean_value : public Mean_value_base<T> {};

template <typename R>
struct Mean_value_helper
{
  template <typename T>
  struct Mean_value : public Mean_value_base<T,R> {};
};


template <typename T, typename R = typename scalar_of<T>::type>
struct Mean_magsq_value_base
{
  static reduction_type const rtype = reduce_sum;

  typedef R result_type;
  typedef R accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return state + math::magsq(new_value, R());}

  static accum_type value(accum_type state, length_type size)
  { return state / size;}

  static bool done(accum_type) { return false;}
};

template <typename T>
struct Mean_magsq_value : public Mean_magsq_value_base<T> {};

template <typename R>
struct Mean_magsq_value_helper
{
  template <typename T>
  struct Mean_magsq_value : public Mean_magsq_value_base<T,R> {};
};

// Generic base class for summing items of type T into
// an accumulator of type R.
template <typename T, typename R = T>
struct Sum_value_base
{
  static reduction_type const rtype = reduce_sum;

  typedef R result_type;
  typedef R accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return state + new_value;}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type) { return false;}
};

// Use this with reduce<> when T and R are identical.
// eg:
//   reduce<impl::Sum_value>
template <typename T>
struct Sum_value : public Sum_value_base<T> {};

// Use this with reduce<> when T and R are different.
// eg:
//   reduce<impl::Sum_value_helper<R>::template Sum_value>

template <typename R>
struct Sum_value_helper
{
  template <typename T>
  struct Sum_value : public Sum_value_base<T,R> {};
};

template <typename T>
struct Sum_magsq_value
{
  static reduction_type const rtype = reduce_sum;

  typedef typename scalar_of<T>::type result_type;
  typedef typename scalar_of<T>::type accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return state + magsq(new_value);}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type) { return false; }
};

/// Specialization for 'bool': return the number of true values.
template <>
struct Sum_value<bool>
{
  static reduction_type const rtype = reduce_sum;

  typedef length_type result_type;
  typedef length_type accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, bool new_value)
  { return state + (new_value ? 1 : 0);}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type) { return false;}
};

template <typename T, typename R = T>
struct Sum_sq_value_base
{
  static reduction_type const rtype = reduce_sum;

  typedef R result_type;
  typedef R accum_type;

  static accum_type initial() { return accum_type();}

  static accum_type update(accum_type state, T new_value)
  { return state + new_value * new_value;}

  static accum_type value(accum_type state, length_type)
  { return state;}

  static bool done(accum_type) { return false;}
};

template <typename T>
struct Sum_sq_value : public Sum_sq_value_base<T> {};

// Use this with reduce<> when T and R are different.
// eg:
//   reduce<impl::Sum_value_helper<R>::template Sum_value>
template <typename R>
struct Sum_sq_value_helper
{
  template <typename T>
  struct Sum_sq_value : public Sum_sq_value_base<T,R> {};
};

template <typename T>
class Max_value
{
public:
  typedef T result_type;

  Max_value(T init) : value_(init) {}

  bool next_value(T value)
  {
    if (value > value_)
    {
      value_ = value;
      return true;
    }
    else return false;
  }

  T value() { return value_;}

private:
  T value_;
};

template <typename T>
class Min_value
{
public:
  typedef T result_type;

  Min_value(T init) : value_(init) {}

  bool next_value(T value)
  {
    if (value < value_)
    {
      value_ = value;
      return true;
    }
    else return false;
  }

  T value() { return value_;}

private:
  T value_;
};

template <typename T>
class Max_mag_value
{
public:
  typedef typename scalar_of<T>::type result_type;

  Max_mag_value(T init) : value_(mag(init)) {}

  bool next_value(T value)
  {
    result_type tmp = mag(value);
    if (tmp > value_)
    {
      value_ = tmp;
      return true;
    }
    else return false;
  }

  result_type value() { return value_;}

private:
  result_type value_;
};



template <typename T>
class Min_mag_value
{
public:
  typedef typename scalar_of<T>::type result_type;

  Min_mag_value(T init) : value_(mag(init)) {}

  bool next_value(T value)
  {
    result_type tmp = mag(value);
    if (tmp < value_)
    {
      value_ = tmp;
      return true;
    }
    else return false;
  }

  result_type value() { return value_;}

private:
  result_type value_;
};



template <typename T>
class Max_magsq_value
{
public:
  typedef typename scalar_of<T>::type result_type;

  Max_magsq_value(T init) : value_(magsq(init)) {}

  bool next_value(T value)
  {
    result_type tmp = magsq(value);
    if (tmp > value_)
    {
      value_ = tmp;
      return true;
    }
    else return false;
  }

  result_type value() { return value_;}

private:
  result_type value_;
};

template <typename T>
class Min_magsq_value
{
public:
  typedef typename scalar_of<T>::type result_type;

  Min_magsq_value(T init) : value_(magsq(init)) {}

  bool next_value(T value)
  {
    result_type tmp = magsq(value);
    if (tmp < value_)
    {
      value_ = tmp;
      return true;
    }
    else return false;
  }

  result_type value() { return value_;}

private:
  result_type value_;
};

} // namespace ovxx

#endif
