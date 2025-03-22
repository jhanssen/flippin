#include "Entry.h"
#include "File.h"
#include "Directory.h"

namespace flippy {

Entry::Entry(std::variant<std::unique_ptr<File>, std::unique_ptr<Directory>>&& entry)
    : mEntry(std::move(entry))
{
}

Entry::~Entry()
{
}

} // namespace flippy
