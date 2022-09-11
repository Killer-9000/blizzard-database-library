#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <readers/BlizzardTableReaderFactory.h>
#include <DatabaseDefinition.h>
#include <extensions/StringExtensions.h>
#include <stream/StreamReader.h>
#include <sstream>
#include <functional>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
		friend class BlizzardDatabaseTable;
	private:
		const std::string _databaseDefinitionFilesLocation;
		const Structures::Build _build;
		Reader::BlizzardTableReaderFactory _blizzardTableReaderFactory;

		std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>> _loadedTables;
	public:
		BlizzardDatabase(const std::string& databaseDefinitionDirectory, const Structures::Build& build);

		const BlizzardDatabaseTable& LoadTable(const std::string& tableName, std::function<std::shared_ptr<BlizzardDatabaseLib::Stream::IMemStream>(std::string const&)> file_callback);
		std::shared_ptr<BlizzardDatabaseTable>& GetTable(const std::string& tableName, std::function<std::shared_ptr<BlizzardDatabaseLib::Stream::IMemStream>(std::string const&)> file_callback)
		{
			if (!_loadedTables.contains(tableName))
				LoadTable(tableName, file_callback);
			return _loadedTables[tableName];
		}
		void SaveTable(const BlizzardDatabaseTable& table);
		void UnloadTable(const std::string& tableName);
	};
}