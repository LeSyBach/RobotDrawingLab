#pragma once
#include "image_paths.h"
#include <filesystem>
#include <string>

namespace docSvg {

using std::wstring;
using std::filesystem::path;

// Doc SVG va ket xuat thanh anh nen trang de robot ve va to.
bool docTep(const path& tenTep, xuLyAnh::Anh& dauRa, wstring& thongBaoLoi);
}
