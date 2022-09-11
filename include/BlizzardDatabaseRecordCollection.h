#pragma once
#include <memory>
#include <readers/IBlizzardTableReader.h>
#include <structures/FileStructures.h>

namespace BlizzardDatabaseLib {

    class BlizzardDatabaseRecordCollection
    {
    private:
        uint32_t _minIndex = 0;
        uint32_t _maxIndex = 0;
        uint32_t _currentIndex = 0;
        std::shared_ptr<Reader::IBlizzardTableReader> _tableReader;
        Structures::BlizzardDatabaseRow _currentRecord;
    public:
        BlizzardDatabaseRecordCollection(std::shared_ptr<Reader::IBlizzardTableReader> tableReader)
        {
            _maxIndex = tableReader->RecordCount();
            _tableReader = tableReader;
        }

        bool HasRecords()
        {
            return _currentIndex < _maxIndex - 1;
        }

        Structures::BlizzardDatabaseRow& Next()
        {
            _currentRecord = _tableReader->Record(_currentIndex);
            _currentIndex++;
            return _currentRecord;
        }

        Structures::BlizzardDatabaseRow& First()
        {
            _currentRecord = _tableReader->Record(_minIndex);
            return _currentRecord;
        }

        Structures::BlizzardDatabaseRow& Last()
        {
            _currentRecord = _tableReader->Record(_maxIndex);
            return _currentRecord;
        }
    };
}