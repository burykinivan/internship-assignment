#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <conio.h>

namespace fs = std::filesystem;

struct MarkedItem
{
    fs::path path;
    bool exists = false;
};

fs::path currentDir = fs::current_path();
std::vector<fs::directory_entry> items;

int cursorPos = 0;
MarkedItem marked;

void loadDirectory()
{
    items.clear();

    for (const auto& entry : fs::directory_iterator(currentDir))
    {
        items.push_back(entry);
    }

    if (cursorPos >= items.size())
        cursorPos = 0;
}

void printDirectory()
{
    system("cls");

    std::cout << "Current directory:\n";
    std::cout << currentDir.string() << "\n\n";

    for (int i = 0; i < items.size(); i++)
    {
        if (i == cursorPos)
            std::cout << "> ";
        else
            std::cout << "  ";

        if (items[i].is_directory())
            std::cout << "[DIR] ";
        else
            std::cout << "      ";

        std::cout << items[i].path().filename().string();

        if (marked.exists &&
            items[i].path() == marked.path)
        {
            std::cout << "  [MARK]";
        }

        std::cout << "\n";
    }

    std::cout << "\n";
    std::cout << "UP/DOWN - move\n";
    std::cout << "ENTER - open folder\n";
    std::cout << "BACKSPACE - parent folder\n";
    std::cout << "D - delete\n";
    std::cout << "M - mark\n";
    std::cout << "C - copy marked\n";
    std::cout << "X - cut marked\n";
    std::cout << "F - view file\n";
    std::cout << "ESC - exit\n";
}

void deleteCurrent()
{
    if (items.empty())
        return;

    fs::remove_all(items[cursorPos].path());
    loadDirectory();
}

void markCurrent()
{
    if (items.empty())
        return;

    marked.path = items[cursorPos].path();
    marked.exists = true;
}

void copyMarked()
{
    if (!marked.exists)
        return;

    fs::path destination =
        currentDir / marked.path.filename();

    try
    {
        if (fs::is_directory(marked.path))
        {
            fs::copy(
                marked.path,
                destination,
                fs::copy_options::recursive
            );
        }
        else
        {
            fs::copy_file(
                marked.path,
                destination,
                fs::copy_options::overwrite_existing
            );
        }
    }
    catch (...)
    {
    }

    loadDirectory();
}

void cutMarked()
{
    if (!marked.exists)
        return;

    fs::path destination =
        currentDir / marked.path.filename();

    try
    {
        fs::rename(marked.path, destination);
        marked.exists = false;
    }
    catch (...)
    {
    }

    loadDirectory();
}

void viewFile()
{
    if (items.empty())
        return;

    if (items[cursorPos].is_directory())
        return;

    system("cls");

    std::ifstream file(items[cursorPos].path());

    std::string line;

    while (std::getline(file, line))
    {
        std::cout << line << "\n";
    }

    std::cout << "\n\nPress any key...";
    _getch();
}

int main()
{
    loadDirectory();

    while (true)
    {
        printDirectory();

        int key = _getch();

        if (key == 224)
        {
            key = _getch();

            if (key == 72)
            {
                if (cursorPos > 0)
                    cursorPos--;
            }
            else if (key == 80)
            {
                if (cursorPos < items.size() - 1)
                    cursorPos++;
            }
        }
        else if (key == 13)
        {
            if (!items.empty() &&
                items[cursorPos].is_directory())
            {
                currentDir = items[cursorPos].path();
                cursorPos = 0;
                loadDirectory();
            }
        }
        else if (key == 8)
        {
            if (currentDir.has_parent_path())
            {
                currentDir = currentDir.parent_path();
                cursorPos = 0;
                loadDirectory();
            }
        }
        else if (key == 'd' || key == 'D')
        {
            deleteCurrent();
        }
        else if (key == 'm' || key == 'M')
        {
            markCurrent();
        }
        else if (key == 'c' || key == 'C')
        {
            if (marked.exists)
            {
                copyMarked();
            }
        }
        else if (key == 'x' || key == 'X')
        {
            if (marked.exists)
            {
                cutMarked();
            }
        }
        else if (key == 'f' || key == 'F')
        {
            viewFile();
        }
        else if (key == 27)
        {
            break;
        }
    }

    return 0;
}