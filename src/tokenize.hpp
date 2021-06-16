#pragma once

#include <string>
#include <vector>

using Token = std::string;
using Tokens = std::vector<Token>;

Tokens tokenize(std::string const &line);