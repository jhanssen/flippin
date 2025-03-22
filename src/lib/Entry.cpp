#include "Entry.h"
#include "File.h"
#include "Directory.h"

namespace flippy {

Entry::Entry(std::variant<std::shared_ptr<File>, std::shared_ptr<Directory>>&& entry)
    : mEntry(std::move(entry))
{
}

Entry::~Entry()
{
}

} // namespace flippy
