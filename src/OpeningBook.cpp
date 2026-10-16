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
    int fd = ::open(book_path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        log(OPENING_BOOK, "Failed to open opening book at: " + book_path);
        return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1 || sb.st_size <= 0 || (sb.st_size % sizeof(PolyglotBookEntry)) != 0)
    {
        log(OPENING_BOOK, "Invalid book file: " + book_path);
        ::close(fd);
        return false;
    }

    num_entries = static_cast<uint64>(sb.st_size / sizeof(PolyglotBookEntry));
    PolyglotBookEntry *polyglot_book = reinterpret_cast<PolyglotBookEntry *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (polyglot_book == MAP_FAILED)
    {
        log(OPENING_BOOK, "Failed to memory-map book file: " + book_path);
        ::close(fd);
        return false;
    }

    // convert polyglot book format (big-endian) to internal representation
    book = reinterpret_cast<BookEntry *>(mmap(nullptr, num_entries * sizeof(BookEntry), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (book == MAP_FAILED)
    {
        log(OPENING_BOOK, "Failed to allocate memory for book entries");
        munmap(polyglot_book, sb.st_size);
        ::close(fd);
        return false;
    }

    for (size_t i = 0; i < num_entries; i++)
    {
        book[i].hash = __builtin_bswap64(polyglot_book[i].hash);
        book[i].move = Move(__builtin_bswap16(polyglot_book[i].move));
        book[i].weight = __builtin_bswap16(polyglot_book[i].weight);
    }
    log(OPENING_BOOK, "Loaded opening book with " + std::to_string(num_entries) + " entries from: " + book_path);
    munmap(polyglot_book, sb.st_size);
    ::close(fd);
    return true;
}

Move OpeningBook::getBookMoveIfAvailable(const Hash &hash)
{
    if (!enabled || book == nullptr || num_entries == 0)
        return NULL_MOVE;

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
        munmap(book, num_entries * sizeof(BookEntry));
    book = nullptr;
    num_entries = 0;
    enabled = false;
}

std::string BookEntry::toString() const
{
    return "Hash: " + std::to_string(hash) + ", Move: " + move.toString() + ", Weight: " + std::to_string(weight);
}
