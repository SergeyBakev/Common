// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H
#include <vector>
#include <set>
#include <algorithm>
#include "..\Common\Arrays\ke_farray_base.h"
#include "..\Common\ke_def.h"
#include "..\Common\Helpers\ke_kernel.h"
#include "..\Common\Arrays\ke_typed_farray.h"
#include "..\Common\ke_type_traits.h"
#include "..\Common\Algorithm\ke_algorithm.h"
#include "..\Common\Algorithm\ke_string.h"
#include "..\Common\FormatedString.h"
#include "..\Common\Helpers\StringHelper.h"
#include "..\Common\Arrays\ke_array_indexer.h"
#include "..\Common\Logger\EventLogger\JournalLogger.h"
#include "..\Common\Logger\EventLogger\WinLogReaderV2.h"
#include "..\Common\Logger\EventLogger\WinLogFilter.h"
#include "..\Common\Logger\EventLogger\DummyReader.h"
#include "..\Common\VectorRepository.h"
#include "..\Common\repo_def.h"
#include "..\Common\_uuid_t.h"
#include <array>
using DoubleVector = std::vector<double>;
#endif //PCH_H
