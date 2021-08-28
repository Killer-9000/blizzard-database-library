#pragma once
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <structures/Types.h>

namespace std {
    template <typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>&
        tab(basic_ostream<_CharT, _Traits>& __os) {
        return __os.put(__os.widen('\t'));
    }
}

namespace BlizzardDatabaseLib {
    namespace Extension {
    
        class String
        {
        public:
            static std::vector<std::string> Split(const std::string& s, std::string delimiter)
            {
                size_t pos_start = 0, pos_end, delim_len = delimiter.length();
                std::string token;
                std::vector<std::string> res;

                while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
                    token = s.substr(pos_start, pos_end - pos_start);
                    pos_start = pos_end + delim_len;

                    if (token.empty())
                        continue;

                    res.push_back(token);
                }

                res.push_back(s.substr(pos_start));
                return res;
            }

            static bool Compare(const std::string& lhs, const std::string& rhs)
            {
                auto result = strcmp(lhs.c_str(), rhs.c_str());

                if (result == 0)
                    return true;
                return false;
            }

            static void Replace(std::string& data, std::string toSearch, std::string replaceStr)
            {
                size_t pos = data.find(toSearch);
                while (pos != std::string::npos)
                {
                    data.replace(pos, toSearch.size(), replaceStr);
                    pos = data.find(toSearch, pos + replaceStr.size());
                }
            }
        };

        class Flag
        {
        public:
            inline static bool HasFlag(const BlizzardDatabaseLib::Flag::DatabaseVersion2Flag& lhs, const BlizzardDatabaseLib::Flag::DatabaseVersion2Flag& rhs) noexcept
            {
                return (lhs & rhs) == rhs;
            }
        };
    }
}