#pragma once
#include<structures/Build.h>

namespace BlizzardDatabaseLib {
    namespace Structures {

        class BuildRange
        {
        private:
            Build _minBuild;
            Build _maxBuild;
        public:
            BuildRange(Build minBuild, Build maxBuild) : _minBuild(minBuild), _maxBuild(maxBuild) {}

            bool Contains(Build& build)
            {
                if (build > _minBuild && build < _maxBuild)
                    return true;
                return false;
            }
        };
    }
}
