#include "svg_import.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "../third_party/nanosvg/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "../third_party/nanosvg/nanosvgrast.h"

namespace docSvg {

using std::ifstream;
using std::ios;
using std::isfinite;
using std::istreambuf_iterator;
using std::max;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

bool docTep(const path& tenTep, xuLyAnh::Anh& dauRa, wstring& thongBaoLoi) {
    ifstream dauVao(tenTep, ios::binary);
    if (!dauVao) {
        thongBaoLoi = L"Không mở được tệp SVG.";
        return false;
    }
    string xml{istreambuf_iterator<char>(dauVao), istreambuf_iterator<char>()};
    if (dauVao.bad() || xml.find("<svg") == string::npos) {
        thongBaoLoi = L"Không đọc được nội dung SVG. Hãy dùng tệp SVG văn bản UTF-8.";
        return false;
    }
    unique_ptr<NSVGimage, decltype(&nsvgDelete)> hinhSvg(nsvgParse(xml.data(), "px", 96),
                                                         nsvgDelete);
    if (!hinhSvg || !hinhSvg->shapes || !isfinite(hinhSvg->width) || !isfinite(hinhSvg->height) ||
        hinhSvg->width <= 0 || hinhSvg->height <= 0) {
        thongBaoLoi =
            L"SVG thiếu hình có thể vẽ. Nếu ảnh chứa chữ hoặc ký hiệu, hãy chuyển chúng thành path.";
        return false;
    }
    unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)> boKetXuat(nsvgCreateRasterizer(),
                                                                          nsvgDeleteRasterizer);
    if (!boKetXuat) {
        thongBaoLoi = L"Không đủ bộ nhớ để mở SVG.";
        return false;
    }
    constexpr int kichCo = 256;
    const float tiLe = 240.f / max(hinhSvg->width, hinhSvg->height);
    if (!isfinite(tiLe)) {
        thongBaoLoi = L"Kích thước SVG không hợp lệ.";
        return false;
    }
    const float trai = (kichCo - hinhSvg->width * tiLe) / 2;
    const float tren = (kichCo - hinhSvg->height * tiLe) / 2;
    vector<unsigned char> rgba(kichCo * kichCo * 4);
    nsvgRasterize(boKetXuat.get(), hinhSvg.get(), trai, tren, tiLe, rgba.data(), kichCo, kichCo,
                  kichCo * 4);
    auto anh = xuLyAnh::Anh::taoAnhMau(kichCo, kichCo);
    bool coNoiDung = false;
    for (int y = 0; y < kichCo; ++y) {
        for (int x = 0; x < kichCo; ++x) {
            if (x + .5f < trai || x + .5f >= kichCo - trai || y + .5f < tren ||
                y + .5f >= kichCo - tren) {
                continue;
            }
            const int chiSo = y * kichCo + x;
            const auto* diemAnh = &rgba[chiSo * 4];
            anh.ganMauRGBA(chiSo, diemAnh[0], diemAnh[1], diemAnh[2], diemAnh[3]);
            coNoiDung |= diemAnh[3] != 0;
        }
    }
    if (!coNoiDung) {
        thongBaoLoi = L"SVG không có nét nhìn thấy trên nền trắng. Hãy thử chuyển hình thành path.";
        return false;
    }
    dauRa = move(anh);
    return true;
}
}
