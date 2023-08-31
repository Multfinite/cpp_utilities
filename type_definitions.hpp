#ifndef TYPE_DEFINITIONS_HPP
#define TYPE_DEFINITIONS_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#pragma warning (disable : 4996)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string_view>
#include <string>

#include <memory>
#include <atomic>
#include <any>

#include <list>
#include <map>
#include <functional>
#include <optional>

#include <chrono>

#include <date/date.h>
#include <nlohmann/json.hpp>
#include <stduuid/uuid.h>

using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;
using std::ifstream;
using std::ostream;
using std::function;
using std::optional;

using std::list;
using std::map;
using std::vector;

using std::string_view;
using std::string;
using std::stringstream;

using std::cout;
using std::cin;
using std::endl;

using nlohmann::json;

using byte		= unsigned char;
using sbyte	= char;

#endif // TYPE_DEFINITIONS_HPP