//===- iterator_range.h - A range adaptor for iterators ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This provides a very simple, boring adaptor for a begin and end iterator
/// into a range type. This should be used to build range views that work well
/// with range based for loops and range based constructors.
///
/// Note that code here follows more standards-based coding conventions as it
/// is mirroring proposed interfaces for standardization.
///
//===----------------------------------------------------------------------===//

#ifndef SYSY_ITERATOR_RANGE_H
#define SYSY_ITERATOR_RANGE_H

namespace sysy
{

template <typename IteratorT>
class iterator_range
{
    IteratorT begin_iterator, end_iterator;

public:
    template <typename Container>
    iterator_range(Container &&c)
        : begin_iterator(c.begin()), end_iterator(c.end()) {}
    iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
        : begin_iterator(std::move(begin_iterator)),
            end_iterator(std::move(end_iterator)) {}

    IteratorT begin() const { return begin_iterator; }
    IteratorT end() const { return end_iterator; }
    bool empty() const { return begin_iterator == end_iterator; }
};

template <class T>
iterator_range<T> make_range(T x, T y)
{
    return iterator_range<T>(std::move(x), std::move(y));
}

} // namespace sysy

#endif