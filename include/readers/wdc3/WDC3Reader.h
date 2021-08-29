#pragma once
#include <type_traits>
#include <fstream>
#include <istream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <readers/IBlizzardTableReader.h>
#include <stream/StreamReader.h>
#include <stream/BitReader.h>
#include <structures/Types.h>
#include <structures/FileStructures.h>
#include <extensions/StringExtensions.h>
#include <extensions/FlagExtensions.h>
#include <extensions/MemoryExtensions.h>

namespace BlizzardDatabaseLib {

  
    class WDC3Reader
    {
    private:
        const int _headerSize = 72;
        const unsigned int WDC3FmtSig = 0x33434457;
        Stream::StreamReader _streamReader;

    public:
        Structures::WDC3Header Header;

        std::vector<Structures::WDC3Section> Sections;
        std::vector<Structures::FieldMeta> Meta;
        std::vector<Structures::ColumnMetaData> ColumnMeta;
        std::map<int, std::vector<Structures::Int32>> PalletData;
        std::map<int, std::map<int, Structures::Int32>> CommonData;
        std::map<int, int> CopyData;
        std::map<long, std::string> StringTable;

        WDC3Reader(Stream::StreamReader& streamReader);
        std::vector<Structures::BlizzardDatabaseRow> ReadRows(Structures::VersionDefinition& versionDefinition);
    private:
        template<typename T>
        T GetFieldValue(int Id, Stream::BitReader& reader, std::map<long, std::string>& stringLookup, Structures::FieldMeta& fieldMeta,
            Structures::ColumnMetaData& columnMeta, std::vector<Structures::Int32>& palletData, std::map<int, Structures::Int32>& commonData)
        {
            switch (columnMeta.Compression)
            {
                case Structures::CompressionType::None:
                {
                    auto bitSize = 32 - fieldMeta.Bits;
                    if (bitSize <= 0)
                        bitSize = columnMeta.compressionData.Immediate.BitWidth;

                    return reader.ReadValue64(bitSize).As<T>();
                }
                case  Structures::CompressionType::SignedImmediate:
                {
                    return reader.ReadSignedValue64(columnMeta.compressionData.Immediate.BitWidth).As<T>();
                }
                case  Structures::CompressionType::Immediate:
                {
                    return reader.ReadValue64(columnMeta.compressionData.Immediate.BitWidth).As<T>();
                }
                case  Structures::CompressionType::Common:
                {
                    if (commonData.contains(Id))
                        return commonData.at(Id).As<T>();

                    return columnMeta.compressionData.Common.DefaultValue.As<T>();
                }
                case  Structures::CompressionType::Pallet:
                {
                    auto value = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);
                    return palletData[value].As<T>();
                }
                case  Structures::CompressionType::PalletArray:
                {
                    if (columnMeta.compressionData.Pallet.Cardinality != 1)
                        break;

                    auto palletArrayIndex = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);
                    return palletData[palletArrayIndex].As<T>();
                }
            }

            return static_cast<T>(0);
        }

        template<typename T>
        std::vector<T> GetFieldArrayValue(int Id, Stream::BitReader& reader, std::map<long, std::string>& stringLookup, Structures::FieldMeta& fieldMeta,
            Structures::ColumnMetaData& columnMeta, std::vector<Structures::Int32>& palletData, std::map<int, Structures::Int32>& commonData)
        {
            auto vector = std::vector<T>();
            switch (columnMeta.Compression)
            {
                case  Structures::CompressionType::None:
                {
                    auto bitSize = 32 - fieldMeta.Bits;
                    if (bitSize <= 0)
                        bitSize = columnMeta.compressionData.Immediate.BitWidth;

                    auto entires = columnMeta.Size / bitSize;
                    for (auto i = 0; i < entires; i++)
                    {
                        auto entry = reader.ReadValue64(bitSize).As<T>();
                        vector.push_back(entry);
                    }
                    break;
                }
                case  Structures::CompressionType::PalletArray:
                {
                    auto cardinality = columnMeta.compressionData.Pallet.Cardinality;
                    auto index = reader.ReadUint32(columnMeta.compressionData.Pallet.BitWidth);

                    for (auto i = 0; i < cardinality; i++)
                    {
                        auto data = palletData[i + cardinality * (index)].As<T>();
                        vector.push_back(data);
                    }
                }
            }
            return vector;
        }

        std::vector<std::string> GetFieldStringArrayValue(int offset, Stream::BitReader& reader, std::map<long, std::string>& stringLookup, Structures::FieldMeta& fieldMeta,
            Structures::ColumnMetaData& columnMeta, std::vector<Structures::Int32>& palletData, std::map<int, Structures::Int32>& commonData)
        {
            auto vector = std::vector<std::string>();
            switch (columnMeta.Compression)
            {
                case Structures::CompressionType::None:
                {
                    auto bitSize = 32 - fieldMeta.Bits;
                    if (bitSize <= 0)
                        bitSize = columnMeta.compressionData.Immediate.BitWidth;

                    auto entires = columnMeta.Size / bitSize;
                    for (auto i = 0; i < entires; i++)
                    {
                        auto entryIndex = (reader.Position >> 3) + offset + reader.ReadValue64(bitSize).As<int>();
                        auto string = stringLookup.at(entryIndex);
                        vector.push_back(string);
                    }
                }
            }

            return vector;
        }
    };
}