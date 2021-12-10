#include <readers/wdbc/WDBCTableReader.h>
#include <extensions/MemoryExtensions.h>

namespace BlizzardDatabaseLib {
    namespace Reader {
  
        WDBCTableReader::WDBCTableReader(std::shared_ptr<Stream::StreamReader> streamReader, Structures::VersionDefinition versionDefinition) : _streamReader(streamReader), _versionDefinition(versionDefinition)
        {
            _stringTable = std::map<long, std::string>();
        }

        WDBCTableReader::~WDBCTableReader()
        {
            _streamReader.reset();
        }


        void WDBCTableReader::LoadTableStructure()
        {
            auto length = _streamReader->Length();
            auto headerSize = sizeof(Structures::WDBCHeader);

            if (length < headerSize)
            {
                std::cout << "Error Occured While Parsing WDC3 Header, Header to short." << std::endl;
                return;
            }

            _streamReader->Jump(0);

            auto magicNumber = _streamReader->Read<unsigned int>();
            if (magicNumber != Flag::TableFormatSignatures::WDBC_FMT_SIGNATURE)
            {
                std::cout << "Error Occured While Parsing WDC3 Header, Format Signature doesnt Match." << std::endl;
                return;
            }

            Header = _streamReader->Read<Structures::WDBCHeader>();
            _recordData = _streamReader->ReadBlock(Header.RecordsCount * Header.RecordSize);
         
            for(int i = 0; i < Header.StringTableSize;)
            {
                auto lastPosition = _streamReader->Position();
                auto stringValue = _streamReader->ReadString();
                _stringTable[i] = stringValue;
                i += _streamReader->Position() - lastPosition;
            }
        }

        Structures::BlizzardDatabaseRow WDBCTableReader::RecordById(unsigned int Id)
        {
            auto offset = 0;
            for(int i = 0; i < Header.RecordsCount; i++)
            {
                auto recordDataBlock = std::make_unique<char[]>(32);
                memcpy(recordDataBlock.get(), _recordData.get() + offset, 32);
                auto bitReader = Stream::BitReader(recordDataBlock, Header.RecordSize);
                auto rowId = bitReader.ReadUint32(32);
                if(rowId == Id)
                {
                    return Record(i);
                }

                offset += Header.RecordSize;
            }

            return Structures::BlizzardDatabaseRow();
        }

        Structures::BlizzardDatabaseRow WDBCTableReader::Record(unsigned int index)
        {
            auto recordDataOffset = Header.RecordSize * index;
            auto recordDataBlock = std::make_unique<char[]>(Header.RecordSize);
            memcpy(recordDataBlock.get(), _recordData.get() + recordDataOffset, Header.RecordSize);

            auto bitReader = Stream::BitReader(recordDataBlock, Header.RecordSize);

            auto rowId = bitReader.ReadUint32(32);
            auto row = Structures::BlizzardDatabaseRow(rowId);
            for(auto i = 1; i < _versionDefinition.versionDefinitions.definitions.size(); i++)
            {
                auto definition = _versionDefinition.versionDefinitions.definitions[i];
                auto columnDefinition = _versionDefinition.columnDefinitions.at(definition.name);
                auto column = Structures::BlizzardDatabaseColumn();

                auto value = std::string();
                if (Extension::String::Compare(columnDefinition.type, "int"))
                {
                    auto intValue = bitReader.ReadSignedValue64(32).As<int>();
                    value = std::to_string(intValue);
                }

                else if (Extension::String::Compare(columnDefinition.type, "string"))
                {
                    auto intValue = bitReader.ReadUint32(32);
                    value = _stringTable.at(intValue);
                }

                else if (Extension::String::Compare(columnDefinition.type, "locstring"))
                {
                    auto intValue = bitReader.ReadUint32(32);
                    column.Value = _stringTable.at(intValue);
                    row.Columns[definition.name] = column;

                    for(int i = 0 ; i < 15; i++)
                    {
                        column = Structures::BlizzardDatabaseColumn();

                        intValue = bitReader.ReadUint32(32);
                        column.Value = _stringTable.at(intValue);
                        row.Columns[definition.name+"_loc"+std::to_string(i)] = column;
                    }

                    column = Structures::BlizzardDatabaseColumn();
                    intValue = bitReader.ReadUint32(32);
                    column.Value = std::to_string(intValue);
                    row.Columns[definition.name + "_flags"] = column;
                    continue;
                }

                else if (Extension::String::Compare(columnDefinition.type, "float"))
                {
                    if (definition.arrLength > 1)
                    {
                        for(int i = 0; i < definition.arrLength; i++)
                        {
                            auto intValue = bitReader.ReadSignedValue64(32).As<float>();
                            column.Values.push_back(std::to_string(intValue));
                        }
                    }
                	else
                    {
                        auto intValue = bitReader.ReadSignedValue64(32).As<float>();
                        value = std::to_string(intValue);
                    }
                }
                else
                {
                    std::cout << "Unknown Type" << std::endl;
                }


                column.Value = value;
                row.Columns[definition.name] = column;
            }

            return row;
        }

        void WDBCTableReader::CloseAllSections()
        {   
            _recordData.reset();
            _stringTable.clear();
        }

        std::size_t WDBCTableReader::RecordCount()
        {   
            return Header.RecordsCount;
        }

        Structures::BlizzardDatabaseRow WDBCTableReader::RecordDefinition()
        {
            auto recordDefinition = Structures::BlizzardDatabaseRow();
            for (auto& columnInformation : _versionDefinition.versionDefinitions.definitions)
            {
                if (columnInformation.isID)
                    continue;

                recordDefinition.Columns[columnInformation.name] = {Structures::BlizzardDatabaseColumn{std::string("")}};
            }

            return recordDefinition;
        }
    }
}