#pragma once

#include <iostream>
#include <fstream>
#include <streambuf>

#include <array>
#include <sstream>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <unordered_map>

#include <chrono>

#include <functional>

class ScopeGuard
{
	std::function<void()> exit;
public:
	ScopeGuard(std::function<void()> exit, std::function<void()> enter = [] {}) : exit(exit) { enter(); }
	~ScopeGuard() { exit(); }
};