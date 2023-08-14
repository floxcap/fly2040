/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/util.hpp>
#include <fstream>

namespace brls
{

[[noreturn]] void fatal(std::string message)
{
    brls::Logger::error("Fatal error: {}", message);
// allow disabling exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
    throw std::logic_error(message);
#else
    std::abort();
#endif
}

std::string loadFileContents(const std::string &path) {
    std::ifstream fin;
    fin.open(path, std::ios::in);
    if (!fin.is_open()) {
      brls::Logger::error("cannot open file: {}", path);
      return "";
    }

    std::string res, buff;
    while (getline(fin, buff)) {
        res += buff + "\n";
    }
    fin.close();

    return res;
}

} // namespace brls
