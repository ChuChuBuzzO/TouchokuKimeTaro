#pragma once
#include <iostream>
#include <vector>

// vector•\Ž¦—p
template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& vec) {
	if (vec.empty()) {
		os << "{ }";
		return os;
	}
	os << "{" << vec[0];
	for (auto iter = ++vec.begin(); iter != vec.end(); iter++) {
		os << ", " << *iter;
	}
	os << "}";
	return os;
}