#include "Entry.h"
#include "File.h"
#include "Directory.h"

Entry::Entry(std::variant<std::unique_ptr<File>, std::unique_ptr<Directory>>&& entry)
    : mEntry(std::move(entry))
{
}

Entry::~Entry()
{
}
