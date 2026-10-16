#include "OpeningBook.h"

OpeningBook::OpeningBook()
{
    srand(time(0));
}

OpeningBook::~OpeningBook()
{
    freeBook();
}

bool OpeningBook::initializeBook(const std::string &book_path)
{
    freeBook();
    std::ifstream file(book_path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        log(OPENING_BOOK, "Failed to open opening book at: " + book_path);
        return false;
    }

    std::streamsize file_size = file.tellg();
    if (file_size <= 0 || (file_size % sizeof(PolyglotBookEntry)) != 0)
    {
        log(OPENING_BOOK, "Invalid book file: " + book_path);
        return false;
    }

    num_entries = static_cast<uint64>(file_size / sizeof(PolyglotBookEntry));
    PolyglotBookEntry polyglot_book[num_entries];
    // todo would mmap be faster than heap allocation?
    book = new BookEntry[num_entries];
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char *>(polyglot_book), file_size))
    {
        log(OPENING_BOOK, "Error reading book file data.");
        freeBook();
        return false;
    }

    // convert polyglot book format (big-endian) to internal representation
    for (size_t i = 0; i < num_entries; i++)
    {
        book[i].hash = __builtin_bswap64(polyglot_book[i].hash);
        book[i].move = Move(__builtin_bswap16(polyglot_book[i].move));
        book[i].weight = __builtin_bswap16(polyglot_book[i].weight);
    }
    log(OPENING_BOOK, "Loaded opening book with " + std::to_string(num_entries) + " entries from: " + book_path);
    return true;
}

Move OpeningBook::getBookMoveIfAvailable(const Hash &hash)
{
    uint64 index = getBookIndex(hash);
    if (index >= num_entries)
        return NULL_MOVE;

    // find first and last entries for the given position
    uint64 first_entry_index = index;
    while (first_entry_index > 0 && book[first_entry_index - 1].hash == hash)
        first_entry_index--;
    uint64 last_entry_index = index;
    while (last_entry_index < num_entries - 1 && book[last_entry_index + 1].hash == hash)
        last_entry_index++;

    // compute total weight
    uint32 total_weight = 0;
    log(OPENING_BOOK_MOVE, "Found " + std::to_string(last_entry_index - first_entry_index + 1) + " book entries for hash: " + std::to_string(hash));
    for (uint64 i = first_entry_index; i <= last_entry_index; i++)
    {
        log(OPENING_BOOK_MOVE, "\tBook entry: " + book[i].toString());
        total_weight += book[i].weight;
    }

    // sample move based on relative weights
    uint32 random_weight = rand() % total_weight;
    uint32 cumulative_weight = 0;
    for (uint64 i = first_entry_index; i <= last_entry_index; i++)
    {
        cumulative_weight += book[i].weight;
        if (cumulative_weight > random_weight)
        {
            log(OPENING_BOOK_MOVE, "Selected book move: " + book[i].move.toString());
            return book[i].move;
        }
    }
    throw std::runtime_error("Failed to select a book move for hash: " + std::to_string(hash));
    return NULL_MOVE;
}

uint64 OpeningBook::getBookIndex(const Hash &hash)
{
    // simple binary search
    uint64 mid, left = 0, right = num_entries - 1;
    while (left <= right)
    {
        mid = left + (right - left) / 2;
        if (book[mid].hash == hash)
            return mid;
        if (book[mid].hash < hash)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return num_entries;
}

void OpeningBook::freeBook()
{
    if (book != nullptr)
        delete[] book;
    book = nullptr;
    num_entries = 0;
}

std::string BookEntry::toString() const
{
    return "Hash: " + std::to_string(hash) + ", Move: " + move.toString() + ", Weight: " + std::to_string(weight);
}
