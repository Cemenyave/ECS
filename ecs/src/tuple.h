#pragma once
#include <iostream>

template<typename...>
class Tuple;

template<>
class Tuple<>
{
};

template<typename T, typename... Ts>
class Tuple<T, Ts...> : public Tuple<Ts...>
{
public:
  Tuple(const T& t, const Ts&... ts)
    : value(t)
    , Tuple<Ts...>(ts...)
  {}

  Tuple(T&& t, Ts&&... ts)
    : value(std::move(t))
    , Tuple<Ts...>(std::move(ts)...)
  {}
private:
  T value;
};


template<template<typename... Ts>typename TupleType int N>
struct data_type
{

}

/*
template<int N>
void get(Tuple tuple);
*/
