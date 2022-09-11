#pragma once
#include <type_traits>
#include <fstream>
#include <istream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <flags/TableFormatSignatures.h>
#include <readers/IBlizzardTableReader.h>
#include <readers/wdbc/WDBCRecordReader.h>
#include <stream/StreamReader.h>
#include <stream/BitReader.h>
#include <structures/Types.h>
#include <structures/FileStructures.h>
#include <extensions/StringExtensions.h>
#include <extensions/FlagExtensions.h>
#include <extensions/MemoryExtensions.h>
#include <extensions/VectorExtensions.h>


namespace BlizzardDatabaseLib {
    namespace Reader {

        class WDBCTableReader : public IBlizzardTableReader
        {       
            std::shared_ptr<Stream::StreamReader> _streamReader;

            Structures::WDBCHeader Header;
            std::shared_ptr<char[]> _recordData;
            std::map<long, std::string> _stringTable;
            Structures::VersionDefinition _versionDefinition;
        public:
            WDBCTableReader(std::shared_ptr<Stream::StreamReader> streamReader, Structures::VersionDefinition versionDefinition);
            ~WDBCTableReader();
            void LoadTableStructure() override;
            void CloseAllSections() override;
            Structures::BlizzardDatabaseRow RecordById(unsigned int Id) override;
            Structures::BlizzardDatabaseRow Record(unsigned int index) override;
            std::vector<Structures::BlizzardDatabaseRowDefiniton> RecordDefinition() override;
            uint32_t RecordCount() override;
        };
    }
}