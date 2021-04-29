#pragma once
#include "IJournalLogger.h"
//------------------------------------------------------------------------------------
class DummyJournalLogger : public IJournalLogger
{
public:
    // Inherited via IJournalLogger
    virtual void Write(unsigned short type, unsigned short category, unsigned long eventId, StringVector& params) const override
    {

    }

    virtual ILogReaderPtr MakeReader() const override;
};