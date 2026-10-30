#include "types.h"
#include "log/log_error.h"
#include <filesystem>

namespace FileHandling
{
	using namespace LogUtils;

	enum ResultInfo
	{
		FoundNothing,
		FoundSomething,
		CreatedNothing,
		CreatedSomething,
		DeletedNothing,
		DeletedSomething
	};

	Result<std::filesystem::file_status, ErrorMessage> find(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> findFile(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> findDirectory(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> createDirectory(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> createFile(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> deleteDirectory(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> deleteFile(const std::filesystem::path& path);
	Result<ResultInfo, ErrorMessage> deleteRecursive(const std::filesystem::path& path);
}