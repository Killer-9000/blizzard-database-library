#include "BlizzardDatabase.h"
#include <cassert>

namespace BlizzardDatabaseLib
{
    BlizzardDatabase::BlizzardDatabase(const std::string& databaseCollectionDirectory, const std::string& databaseDefinitionDirectory, const Structures::Build& build) :
        _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory), _build(build)
    {
        _loadedTables = std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>>();
        _blizzardTableReaderFactory = Reader::BlizzardTableReaderFactory();
    }

    const BlizzardDatabaseTable& BlizzardDatabase::LoadTable(const std::string& tableName)
    {
        if (_loadedTables.contains(tableName))
            std::cout << "Table Already Loaded" << std::endl;

        auto databaseFile = std::filesystem::directory_entry();
        for (const auto& entry : std::filesystem::directory_iterator(_databaseFilesLocation))
        {
          auto filename = entry.path().stem().string();
          if(Extension::String::IgnoreCaseCompare(filename, tableName))
          {
                std::cout << "Database <"<< tableName << "> Table Found!" << std::endl;
                databaseFile = entry;
          }
        }

        assert(databaseFile.exists() && "Database file was not found!");
        
        auto absoluteFilePathOfDatabaseTable = std::filesystem::path(_databaseFilesLocation) / databaseFile.path()
            .filename();
        auto absoluteFilePathOfDatabaseTableDefinition =  std::filesystem::path(_databaseDefinitionFilesLocation) /
            (tableName + ".dbd");

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition.generic_string());
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(_build, tableDefinition);

        if (!tableFound)
            std::cout << "Verion Not found" << std::endl;

        auto fileStream = std::make_shared<std::ifstream>();
        fileStream->open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

        auto streamReader = std::make_shared<Stream::StreamReader>(fileStream);
        auto fileFormatIdentifier = streamReader->ReadString(4);

        auto tableReader = _blizzardTableReaderFactory.For(streamReader, tableDefinition,fileFormatIdentifier);

        auto constructedTable = std::make_shared<BlizzardDatabaseTable>(tableReader);
        constructedTable->LoadTableStructure();

        _loadedTables.emplace(tableName, constructedTable);

        return *_loadedTables[tableName];
    }

    void BlizzardDatabase::UnloadTable(const std::string& tableName)
    {
        if (!_loadedTables.contains(tableName))
            std::cout << "Table Not Loaded" << std::endl;

        auto table = _loadedTables.at(tableName);

        table.reset();

        _loadedTables.erase(tableName);
    }
}
