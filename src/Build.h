#pragma once
#include <string>
#include <Extensions.h>

class Build
{
private:
    short _expansion;
    short _major;
    short _minor;
    unsigned int build;
public:
    Build(std::string buildString)
    {
        auto buildVersionElements = StringExtenstions::Split(buildString, '.');

        _expansion = std::atoi(buildVersionElements[0].c_str());
        _major = std::atoi(buildVersionElements[1].c_str());
        _minor = std::atoi(buildVersionElements[2].c_str());
        build = std::atoi(buildVersionElements[3].c_str());
    }

    bool operator==(const Build& rhs) {
        return std::tie(_expansion, _major, _minor, build) == std::tie(rhs._expansion, rhs._major, rhs._minor, rhs.build);
    }

    bool operator<(const Build& rhs) {
        return std::tie(_expansion, _major, _minor, build) < std::tie(rhs._expansion, rhs._major, rhs._minor, rhs.build);
    }

    bool operator>(const Build& rhs) {
        return std::tie(_expansion, _major, _minor, build) > std::tie(rhs._expansion, rhs._major, rhs._minor, rhs.build);
    }
};