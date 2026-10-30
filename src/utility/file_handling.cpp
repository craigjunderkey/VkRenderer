#include "file_handling.h"
#include "log/log_utils.h"
#include "helper_funcs.h"
#include "macros.h"
#include <fstream>

namespace FileHandling
{
    using namespace LogUtils;

    Result<std::filesystem::file_status, ErrorMessage> find(const std::filesystem::path& path)
    {
        try
        {
            std::filesystem::file_status status = std::filesystem::status(path);
            return std::move(status);
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            return ErrorMessage({ .channel = "resources" }, "for {}, {}", path, error.what());
        }
    }

    Result<ResultInfo, ErrorMessage> findFile(const std::filesystem::path& path)
    {
        auto findResult = find(path);
        CHECK_KEEP(findResult);
        std::filesystem::file_status status = findResult.take();

        using std::filesystem::file_type;

        switch (status.type())
        {
            case file_type::regular:
            {
                return FoundSomething;
            } break;
            case file_type::directory:
            {
                return ErrorMessage({ .channel = "resources" },
                    "Found a directory for {}, not a file.", path);
            } break;
            case file_type::not_found:
            {
                return FoundNothing;
            } break;
            default:
            {
                return ErrorMessage({ .channel = "resources" },
                    "Found something for {} that wasn't a file.", path);
            } break;
        }
    }

    Result<ResultInfo, ErrorMessage> findDirectory(const std::filesystem::path& path)
    {
        auto findResult = find(path);
        CHECK_KEEP(findResult);
        std::filesystem::file_status status = findResult.take();

        using std::filesystem::file_type;

        switch (status.type())
        {
            case file_type::regular:
            {
                return ErrorMessage({ .channel = "resources" },
                    "Found a file for {}, not a directory.", path);
            } break;
            case file_type::directory:
            {
                return FoundSomething;
            } break;
            case file_type::not_found:
            {
                return FoundNothing;
            } break;
            default:
            {
                return ErrorMessage({ .channel = "resources"},
                    "Found something for {} that wasn't a directory.", path);
            } break;
        }
    }

    Result<ResultInfo, ErrorMessage> createDirectory(const std::filesystem::path& path)
    {
        try
        {
            bool actuallyCreated = std::filesystem::create_directory(path);
            if (!actuallyCreated)
            {
                warn({ .channel = "resources", .verbosity = 2 },
                    "Directory {} was not created since it already exists.", path);
                return CreatedNothing;
            }
            else
            {
                return CreatedSomething;
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            return ErrorMessage({ .channel = "resources" }, "for {}, {}", path, error.what());
        }
    }

    Result<ResultInfo, ErrorMessage> createFile(const std::filesystem::path& path)
    {
        auto findResult = findFile(path);
        CHECK_KEEP(findResult);

        auto out = std::ofstream(path);
        if (!out)
        {
            return ErrorMessage({ .channel = "resources" }, "Failed to open file {}.", path);
        }
        else if (out.bad())
        {
            return ErrorMessage({ .channel = "resources" }, "Mysteroius I/O error...! for {}", path);
        }

        if (findResult.value() == FoundSomething)
        {
            warn({ .channel = "resources", .verbosity = 2 },
                "File {} was not created since it already exists.", path);
            return CreatedNothing;
        }
        else
        {
            return CreatedSomething;
        }
    }

    Result<ResultInfo, ErrorMessage> deleteDirectory(const std::filesystem::path& path)
    {
        auto findResult = findDirectory(path);
        CHECK_KEEP(findResult);

        if (findResult.value() == FoundNothing)
        {
            warn({ .channel = "resources", .verbosity = 2 },
                "Directory {} was not deleted since it doesn't exist.", path);
            return DeletedNothing;
        }

        try
        {
            bool actuallyRemoved = std::filesystem::remove(path);
            if (!actuallyRemoved)
            {
                warn({ .channel = "resources", .verbosity = 2 },
                    "Directory {} was not deleted since it doesn't exist.", path);
                return DeletedNothing;
            }
            else
            {
                return DeletedSomething;
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            return ErrorMessage({ .channel = "resources" }, "for {}, {}", path, error.what());
        }
    }

    Result<ResultInfo, ErrorMessage> deleteFile(const std::filesystem::path& path)
    {
        auto findResult = findFile(path);
        CHECK_KEEP(findResult);

        if (findResult.value()  == FoundNothing)
        {
            warn({ .channel = "resources", .verbosity = 2 },
                "File {} was not deleted since it doesn't exist.", path);
            return DeletedNothing;
        }

        try
        {
            bool actuallyRemoved = std::filesystem::remove(path);
            if (!actuallyRemoved)
            {
                warn({ .channel = "resources", .verbosity = 2 },
                    "File {} was not deleted since it doesn't exist.", path);
                return DeletedNothing;
            }
            else
            {
                return DeletedSomething;
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            return ErrorMessage({ .channel = "resources" }, "for {}, {}", path, error.what());
        }
    }

    Result<ResultInfo, ErrorMessage> deleteRecursive(const std::filesystem::path& path)
    {
        auto findResult = findDirectory(path);
        CHECK_KEEP(findResult);

        if (findResult.value() == FoundNothing)
        {
            warn({ .channel = "resources", .verbosity = 2 },
                "Directory {} not recursively deleted since it doesn't exist.", path);
            return DeletedNothing;
        }

        try
        {
            bool actuallyRemoved = std::filesystem::remove_all(path) > 0;
            if (!actuallyRemoved)
            {
                warn({ .channel = "resources", .verbosity = 2 },
                    "Directory {} not recursively deleted since it doesn't exist.", path);
                return DeletedNothing;
            }
            else
            {
                return DeletedSomething;
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            return ErrorMessage({ .channel = "resources" }, "for {}, {}", path, error.what());
        }
    }
}