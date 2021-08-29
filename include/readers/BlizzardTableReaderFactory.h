#pragma once
#include <iostream>
#include <memory>
#include <readers/IBlizzardTableReader.h>
#include <readers/WDC3TableReader.h>
#include <stream/StreamReader.h>

namespace BlizzardDatabaseLib {
	namespace Reader {
		class BlizzardTableReaderFactory
		{
		public:
			std::shared_ptr<IBlizzardTableReader> For(std::shared_ptr<Stream::StreamReader> streamReader, Structures::VersionDefinition& versionDefinition, const std::string& formatSignature);
		};
	}
}