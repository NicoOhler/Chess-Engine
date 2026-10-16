#include "OpeningBook.h"

OpeningBook::OpeningBook()
{
    srand(time(0));
    initializeBook(TESTING_BOOK);
}

OpeningBook::~OpeningBook()
{
    freeBook();
}

void OpeningBook::initializeBook(const std::string &book_path)
{
    freeBook();

    std::ifstream file(book_path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        log(OPENING_BOOK, "Failed to open opening book at: " + book_path);
        return;
    }

    std::streamsize file_size = file.tellg();
    if (file_size <= 0 || (file_size % sizeof(PolyglotBookEntry)) != 0)
    {
        log(OPENING_BOOK, "Invalid book file: " + book_path);
        return;
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
        return;
    }

    // convert polyglot book format (big-endian) to internal representation
    for (size_t i = 0; i < num_entries; i++)
    {
        book[i].hash = __builtin_bswap64(polyglot_book[i].hash);
        book[i].move = Move(__builtin_bswap16(polyglot_book[i].move));
        book[i].weight = __builtin_bswap16(polyglot_book[i].weight);
    }
    log(OPENING_BOOK, "Loaded opening book with " + std::to_string(num_entries) + " entries from: " + book_path);
    getBookMove(0x463b96181691fc9c); // test book move retrieval
}

Move OpeningBook::getBookMove(const Hash &hash)
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
    uint16 total_weight = 0;
    for (uint64 i = first_entry_index; i <= last_entry_index; i++)
        total_weight += book[i].weight;

    // sample move based on relative weights
    uint16 random_weight = rand() % total_weight;
    uint16 cumulative_weight = 0;
    for (uint64 i = first_entry_index; i <= last_entry_index; i++)
    {
        cumulative_weight += book[i].weight;
        if (cumulative_weight > random_weight)
            return book[i].move;
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
    delete[] book;
    book = nullptr;
    num_entries = 0;
}

std::string BookEntry::toString() const
{
    return "Hash: " + std::to_string(hash) + ", Move: " + move.toString() + ", Weight: " + std::to_string(weight);
}
