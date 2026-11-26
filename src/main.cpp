#ifndef UNICODE
#define UNICODE
#endif
#define _UNICODE
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <gdiplus.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "robot_math.h"
#include "image_paths.h"
#include "svg_import.h"

// Truc Z huong len. Kich thuoc trong canh dung de mo phong.
namespace {

using std::abs;
using std::ceil;
using std::clamp;
using std::cos;
using std::fclose;
using std::ferror;
using std::fill;
using std::fixed;
using std::fprintf;
using std::hypot;
using std::lround;
using std::max;
using std::min;
using std::move;
using std::none_of;
using std::setprecision;
using std::sin;
using std::size_t;
using std::swprintf;
using std::to_wstring;
using std::vector;
using std::wostringstream;
using std::wstring;
using std::chrono::duration;
using std::chrono::steady_clock;
using std::filesystem::path;

constexpr double pi = 3.14159265358979323846;
constexpr int rongBangDieuKhien = 360;
constexpr int kichThuocTranh = 512;
constexpr double caoDoGiay = .23;
constexpr double gocGiayX = 2.15, gocGiayY = -1.45, canhGiay = 2.9;

enum MaDieuKhien {
    NhomThe = 100,
    NutMoAnh,
    NutAnhHoa,
    VeNet,
    NutToVung,
    NutChonMau,
    ThanhTocDo,
    NutChayTamDung,
    NutTienMotBuoc,
    NutVeDau,
    NutLuuTranh,
    HopKieuHienThi,
    ThanhBocTach,
    OHeTruc,
    OQuyDao,
    OPhepChieu,
    OChieuSang,
    ODemDoSau,
    ThanhGocDe,
    ThanhGocVai,
    ThanhGocKhuyu,
    ThanhGocCoTay,
    NutVeTuTheDau,
    NutThuTrongTam,
    NgoaiTamVoi,
    NutThuGioiHan,
    NutDoiNhanh,
    NutXuatMaTran,
    ThanhNguong,
    NutXoaGiay,
    HopCachLayNet,
    NutToMauGoc,
    ThanhDungSaiMau,
    OToDeVien
};
enum class CheDoVe { VeNet, ToVung, ToMauGoc };

struct DiemQuyDao {
    robot::Vecto3 viTri;
    bool haBut = false;
};

struct DieuKhien {
    HWND hwnd;
    int the;
};

struct UngDung {
    HWND cuaSo{}, khungCanh{}, nhomThe{};
    HDC glDC{};
    HGLRC glRC{};
    HFONT phongChu{}, phongTieuDe{}, phongDonCach{};
    GLuint hoaTietGiay{}, kyTuTruc{};
    GLUquadric* khoiTronXoay{};
    int rong = 1400, cao = 900, rongKhungCanh = 1000, caoKhungCanh = 800, the = 0;
    vector<DieuKhien> cacDieuKhien;
    robot::KichThuocRobot kichThuoc;
    robot::GocKhop gocKhop;
    robot::NhanhKhuyu nhanhKhuyu = robot::NhanhKhuyu::KhuyuTren;
    xuLyAnh::Anh anhNguon;
    vector<uint32_t> anhXemTruoc;
    xuLyAnh::VungTo vungTo;
    xuLyAnh::DiemAnh diemHatGiong{-1, -1};
    vector<xuLyAnh::NetVe> cacNet;
    vector<DiemQuyDao> duongDi;
    vector<robot::Vecto3> quyDaoThucTe;
    vector<uint32_t> tranh = vector<uint32_t>(kichThuocTranh * kichThuocTranh, 0xffffffff);
    xuLyAnh::MatNa matNaVien = xuLyAnh::MatNa(kichThuocTranh * kichThuocTranh, 0);
    size_t keTiep = 0;
    bool dangChay = false, canCapNhatTranh = true, dangKeoChuot = false;
    bool heTruc = false, hienQuyDao = false, phoiCanh = true, chieuSang = true, kiemTraDoSau = true;
    bool hienDiemDich = false, dichHopLe = true, coVetMucTruoc = false;
    double gocPhuongVi = -59, gocNang = 31, khoangCach = 11.8, doBocTach = 0;
    double tocDo = 2.5;
    int kieuHienThi = 0, dangChon = 1, nguong = 150;
    xuLyAnh::CachLayNet cachLayNet = xuLyAnh::CachLayNet::TuDong;
    POINT viTriChuotTruoc{}, diemMucTruoc{};
    COLORREF muc = RGB(210, 58, 94);
    COLORREF mauMucLuotNay = RGB(24, 38, 55);
    xuLyAnh::MatNa matNaTo;
    CheDoVe cheDoVe = CheDoVe::VeNet;
    bool choToDeVien = true;
    bool toDeVienLuotNay = true;
    int dungSaiMau = 8;
    robot::Vecto3 dich;
    wstring trangThai = L"Chọn Vẽ nét để robot vẽ ảnh mẫu. Bấm trong ảnh để chọn vùng tô.";
    wstring tenAnh = L"Ảnh mẫu bông hoa";
    GLdouble maTranQuanSat[16]{}, maTranChieu[16]{};
    GLint khungHienThi[4]{};
    steady_clock::time_point thoiDiemTruoc = steady_clock::now();
} ungDung;

using robot::sangDo;
using robot::sangRadian;

HWND timDieuKhien(int id) {
    return GetDlgItem(ungDung.cuaSo, id);
}

void yeuCauVeLai() {
    InvalidateRect(ungDung.cuaSo, nullptr, FALSE);
    InvalidateRect(ungDung.khungCanh, nullptr, FALSE);
}

void baoTrangThai(const wstring& chu) {
    ungDung.trangThai = chu;
    yeuCauVeLai();
}

HWND taoDieuKhien(const wchar_t* lopCuaSo, const wchar_t* chu, DWORD kieu, int id, int x, int y,
                  int w, int h, int the) {
    HWND hwnd =
        CreateWindowExW(0, lopCuaSo, chu, WS_CHILD | WS_VISIBLE | kieu, x, y, w, h, ungDung.cuaSo,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr, nullptr);
    SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(ungDung.phongChu), TRUE);
    ungDung.cacDieuKhien.push_back({hwnd, the});
    return hwnd;
}

void taoNut(const wchar_t* chu, int id, int x, int y, int w = 148, int the = 0) {
    taoDieuKhien(L"BUTTON", chu, BS_PUSHBUTTON | WS_TABSTOP, id, x, y, w, 36, the);
}

void taoNhan(const wchar_t* chu, int x, int y, int the, int w = 318) {
    taoDieuKhien(L"STATIC", chu, SS_LEFT, 0, x, y, w, 23, the);
}

void taoOChon(const wchar_t* chu, int id, int y, bool duocChon, int the) {
    auto hwnd = taoDieuKhien(L"BUTTON", chu, BS_AUTOCHECKBOX | WS_TABSTOP, id, 20, y, 318, 27, the);
    SendMessageW(hwnd, BM_SETCHECK, duocChon ? BST_CHECKED : BST_UNCHECKED, 0);
}

HWND taoThanhTruot(int id, int y, int nhoNhat, int lonNhat, int giaTri, int the) {
    auto hwnd = taoDieuKhien(TRACKBAR_CLASSW, L"", TBS_HORZ | WS_TABSTOP, id, 15, y, 323, 30, the);
    SendMessageW(hwnd, TBM_SETRANGE, TRUE, MAKELONG(nhoNhat, lonNhat));
    SendMessageW(hwnd, TBM_SETPOS, TRUE, giaTri);
    return hwnd;
}

void capNhatThanhGocKhop() {
    const double cacGiaTri[] = {ungDung.gocKhop.gocDe, ungDung.gocKhop.vai, ungDung.gocKhop.khuyu,
                                ungDung.gocKhop.coTay};
    for (int i = 0; i < 4; ++i) {
        SendMessageW(timDieuKhien(ThanhGocDe + i), TBM_SETPOS, TRUE,
                     static_cast<LPARAM>(lround(sangDo(cacGiaTri[i]))));
    }
}

void chuyenThe(int chiSo) {
    ungDung.the = chiSo;
    if (chiSo != 0) {
        ungDung.dangChay = false;
    }
    if (chiSo != 1) {
        ungDung.doBocTach = 0;
        SendMessageW(timDieuKhien(ThanhBocTach), TBM_SETPOS, TRUE, 0);
    }
    for (const auto& c : ungDung.cacDieuKhien) {
        ShowWindow(c.hwnd, c.the == chiSo ? SW_SHOW : SW_HIDE);
    }
    capNhatThanhGocKhop();
    yeuCauVeLai();
}

void taoBangDieuKhien() {
    ungDung.nhomThe =
        CreateWindowExW(0, WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 16, 70, 328, 31,
                        ungDung.cuaSo, reinterpret_cast<HMENU>(NhomThe), nullptr, nullptr);
    SendMessageW(ungDung.nhomThe, WM_SETFONT, reinterpret_cast<WPARAM>(ungDung.phongChu), TRUE);
    const wchar_t* tenThe[] = {L"Vẽ && tô", L"Kết cấu", L"Kiểm chứng"};
    for (int i = 0; i < 3; ++i) {
        TCITEMW mucThe{};
        mucThe.mask = TCIF_TEXT;
        mucThe.pszText = const_cast<wchar_t*>(tenThe[i]);
        TabCtrl_InsertItem(ungDung.nhomThe, i, &mucThe);
    }
    taoNut(L"Mở ảnh…", NutMoAnh, 20, 116);
    taoNut(L"Ảnh bông hoa", NutAnhHoa, 184, 116);
    taoNhan(L"Bấm vào một vùng trong ảnh để đổi màu", 20, 162, 0);
    taoNhan(L"Ngưỡng sáng/tối", 20, 400, 0, 150);
    auto thanhNguong = taoThanhTruot(ThanhNguong, 421, 30, 230, 150, 0);
    SetWindowPos(thanhNguong, nullptr, 15, 421, 150, 30, SWP_NOZORDER);
    taoNhan(L"Cách lấy nét", 180, 400, 0, 153);
    auto hopCachLayNet = taoDieuKhien(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_TABSTOP,
                                      HopCachLayNet, 180, 422, 152, 140, 0);
    for (auto ten : {L"Tự động", L"Đường bao", L"Tâm nét"}) {
        SendMessageW(hopCachLayNet, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(ten));
    }
    SendMessageW(hopCachLayNet, CB_SETCURSEL, 0, 0);
    taoNut(L"Vẽ nét", VeNet, 20, 459, 106);
    taoNut(L"Tô toàn ảnh theo màu gốc", NutToMauGoc, 134, 459, 198);
    taoNut(L"Chọn màu…", NutChonMau, 20, 502);
    taoNut(L"Tô vùng đã chọn", NutToVung, 184, 502);
    taoNhan(L"Dung sai màu", 20, 548, 0, 148);
    auto dungSai = taoThanhTruot(ThanhDungSaiMau, 568, 0, 100, ungDung.dungSaiMau, 0);
    SetWindowPos(dungSai, nullptr, 15, 568, 153, 30, SWP_NOZORDER);
    taoNhan(L"Tốc độ đầu bút", 184, 548, 0, 148);
    auto tocDo = taoThanhTruot(ThanhTocDo, 568, 5, 100, 25, 0);
    SetWindowPos(tocDo, nullptr, 179, 568, 158, 30, SWP_NOZORDER);
    taoNut(L"Chạy / Tạm dừng", NutChayTamDung, 20, 611);
    taoNut(L"Tiến một bước", NutTienMotBuoc, 184, 611);
    taoNut(L"Về đầu đường vẽ", NutVeDau, 20, 654);
    taoNut(L"Xóa mặt giấy", NutXoaGiay, 184, 654);
    taoNut(L"Lưu tranh PNG…", NutLuuTranh, 20, 697);
    auto oToDeVien = taoDieuKhien(L"BUTTON", L"Tô đè lên viền", BS_AUTOCHECKBOX | WS_TABSTOP,
                                  OToDeVien, 184, 697, 148, 36, 0);
    SendMessageW(oToDeVien, BM_SETCHECK, BST_CHECKED, 0);

    taoNhan(L"Kiểu hiển thị mô hình", 20, 116, 1);
    auto hopHienThi = taoDieuKhien(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_TABSTOP, HopKieuHienThi,
                                   20, 142, 312, 200, 1);
    for (auto chu : {L"Vỏ đầy đủ", L"Mở vỏ — xem cơ cấu bên trong", L"Khung dây (wireframe)"}) {
        SendMessageW(hopHienThi, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(chu));
    }
    SendMessageW(hopHienThi, CB_SETCURSEL, 0, 0);
    taoNhan(L"Bóc tách các chi tiết", 20, 184, 1);
    taoThanhTruot(ThanhBocTach, 207, 0, 100, 0, 1);
    taoOChon(L"Hiện hệ trục tọa độ các khớp", OHeTruc, 250, false, 1);
    taoNhan(L"Đế — quay quanh Z", 20, 294, 1);
    taoThanhTruot(ThanhGocDe, 317, -170, 170, 0, 1);
    taoNhan(L"Vai — quay quanh Y cục bộ", 20, 359, 1);
    taoThanhTruot(ThanhGocVai, 382, -25, 150, 60, 1);
    taoNhan(L"Khuỷu — quay quanh Y cục bộ", 20, 424, 1);
    taoThanhTruot(ThanhGocKhuyu, 447, -160, 160, -90, 1);
    taoNhan(L"Cổ tay — quay quanh Y cục bộ", 20, 489, 1);
    taoThanhTruot(ThanhGocCoTay, 512, -170, 170, 30, 1);
    taoNut(L"Tư thế ban đầu", NutVeTuTheDau, 20, 561, 312, 1);
    taoNhan(L"Bấm một khớp trong cảnh để xem ma trận.", 20, 614, 1);
    taoNhan(
        L"Bóc tách là chế độ kiểm tra cấu tạo.\nCác chi tiết cơ khí được mô hình hóa giản lược.",
        20, 649, 1);
    SetWindowPos(ungDung.cacDieuKhien.back().hwnd, nullptr, 20, 649, 318, 84, SWP_NOZORDER);

    taoOChon(L"Chiếu phối cảnh (bỏ chọn: song song)", OPhepChieu, 118, true, 2);
    taoOChon(L"Chiếu sáng và pháp tuyến", OChieuSang, 153, true, 2);
    taoOChon(L"Khử mặt khuất bằng Z-buffer", ODemDoSau, 188, true, 2);
    taoOChon(L"Hiện quỹ đạo thực tế của đầu bút", OQuyDao, 223, false, 2);
    taoNut(L"Thử điểm trong tầm với", NutThuTrongTam, 20, 274, 312, 2);
    taoNut(L"Thử điểm ngoài tầm với", NgoaiTamVoi, 20, 318, 312, 2);
    taoNut(L"Thử giới hạn khớp", NutThuGioiHan, 20, 362, 312, 2);
    taoNut(L"Đổi nhánh động học nghịch", NutDoiNhanh, 20, 406, 312, 2);
    taoNut(L"Xuất ma trận CSV…", NutXuatMaTran, 20, 463, 312, 2);
    taoNhan(
        L"Bấm trên giấy trong cảnh để thử tọa độ.\nTư thế hợp lệ mới được áp dụng.\n\nĐỏ: mục tiêu không hợp lệ.\nXanh: mục tiêu hợp lệ.",
        20, 522, 2);
    SetWindowPos(ungDung.cacDieuKhien.back().hwnd, nullptr, 20, 522, 318, 126, SWP_NOZORDER);
    chuyenThe(0);
}

robot::Vecto3 diemAnhSangToaDoGiay(xuLyAnh::DiemAnh diemAnh) {
    return {gocGiayX + diemAnh.x * canhGiay / (ungDung.anhNguon.rong - 1),
            gocGiayY + canhGiay - diemAnh.y * canhGiay / (ungDung.anhNguon.cao - 1), caoDoGiay};
}

POINT toaDoGiaySangDiemTranh(robot::Vecto3 p) {
    return {
        static_cast<LONG>(lround((p.x - gocGiayX) / canhGiay * (kichThuocTranh - 1))),
        static_cast<LONG>(lround((gocGiayY + canhGiay - p.y) / canhGiay * (kichThuocTranh - 1)))};
}

void xoaMatGiay() {
    fill(ungDung.tranh.begin(), ungDung.tranh.end(), 0xffffffffu);
    fill(ungDung.matNaVien.begin(), ungDung.matNaVien.end(), 0);
    ungDung.canCapNhatTranh = true;
}

int chiSoAnhTuDiemTranh(int x, int y) {
    const auto& anh = ungDung.anhNguon;
    const int cotAnh = lround(x * double(anh.rong - 1) / (kichThuocTranh - 1));
    const int hangAnh = lround(y * double(anh.cao - 1) / (kichThuocTranh - 1));
    return hangAnh * anh.rong + cotAnh;
}

// Moi lan but cham giay, chi cac diem trong dia tron cua ngoi but moi nhan muc.
void inDauBut(int x, int y, int banKinh) {
    uint32_t mau = 0xff000000 | GetRValue(ungDung.mauMucLuotNay) << 16 |
                   GetGValue(ungDung.mauMucLuotNay) << 8 | GetBValue(ungDung.mauMucLuotNay);
    for (int dy = -banKinh; dy <= banKinh; ++dy) {
        for (int dx = -banKinh; dx <= banKinh; ++dx) {
            int px = x + dx, py = y + dy;
            if (dx * dx + dy * dy > banKinh * banKinh || px < 0 || py < 0 || px >= kichThuocTranh ||
                py >= kichThuocTranh) {
                continue;
            }
            const int chiSoTranh = py * kichThuocTranh + px;
            uint32_t mauDiem = mau;
            if (ungDung.cheDoVe == CheDoVe::VeNet) {
                ungDung.matNaVien[chiSoTranh] = 1;
            } else {
                if (!ungDung.toDeVienLuotNay && ungDung.matNaVien[chiSoTranh]) {
                    continue;
                }
                const int chiSoAnh = chiSoAnhTuDiemTranh(px, py);
                if (!ungDung.matNaTo[chiSoAnh]) {
                    continue;
                }
                ungDung.matNaVien[chiSoTranh] = 0;
                if (ungDung.cheDoVe == CheDoVe::ToMauGoc) {
                    mauDiem = ungDung.anhNguon.mauTai(chiSoAnh);
                }
            }
            ungDung.tranh[chiSoTranh] = mauDiem;
        }
    }
    ungDung.canCapNhatTranh = true;
}

void veVetMuc(robot::Vecto3 p) {
    POINT hienTai = toaDoGiaySangDiemTranh(p);
    if (!ungDung.coVetMucTruoc) {
        ungDung.diemMucTruoc = hienTai;
    }
    // Noi hai vi tri but lien tiep bang thuat toan Bresenham.
    int x = ungDung.diemMucTruoc.x;
    int y = ungDung.diemMucTruoc.y;
    const int dx = abs(hienTai.x - x);
    const int dy = -abs(hienTai.y - y);
    const int buocX = x < hienTai.x ? 1 : -1;
    const int buocY = y < hienTai.y ? 1 : -1;
    int saiSo = dx + dy;
    for (;;) {
        inDauBut(x, y, 2);
        if (x == hienTai.x && y == hienTai.y) {
            break;
        }
        int haiLanSaiSo = 2 * saiSo;
        if (haiLanSaiSo >= dy) {
            saiSo += dy;
            x += buocX;
        }
        if (haiLanSaiSo <= dx) {
            saiSo += dx;
            y += buocY;
        }
    }
    ungDung.diemMucTruoc = hienTai;
    ungDung.coVetMucTruoc = true;
}

bool tuTheNamTrenBan(const robot::GocKhop& cacGoc) {
    auto tuThe = robot::tinhDongHocThuan(cacGoc, ungDung.kichThuoc);
    return tuThe.viTriKhuyu.z > .35 && tuThe.viTriCoTay.z > .35 &&
           tuThe.viTriDauBut.z >= caoDoGiay - .005;
}

void veTuTheBanDau() {
    ungDung.dangChay = false;
    ungDung.duongDi.clear();
    ungDung.keTiep = 0;
    ungDung.coVetMucTruoc = false;
    ungDung.hienDiemDich = false;
    auto ketQua = robot::giaiDongHocNghich({3.1, -.7, 1.4}, ungDung.nhanhKhuyu, ungDung.kichThuoc);
    if (ketQua.thanhCong()) {
        ungDung.gocKhop = ketQua.gocKhop;
    }
    capNhatThanhGocKhop();
    yeuCauVeLai();
}

void datLaiDuongVe() {
    veTuTheBanDau();
    ungDung.quyDaoThucTe.clear();
    baoTrangThai(L"Đã về tư thế đầu. Bấm Vẽ nét hoặc Tô vùng để tạo lại đường đi.");
}

void lapDuongDi(const vector<xuLyAnh::NetVe>& cacNet) {
    ungDung.dangChay = false;
    ungDung.duongDi.clear();
    ungDung.keTiep = 0;
    ungDung.coVetMucTruoc = false;
    vector<DiemQuyDao> duongDiDuKien;
    robot::Vecto3 truocDo = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
    const double caoDoNhacBut = 1.05;
    duongDiDuKien.push_back({{truocDo.x, truocDo.y, max(truocDo.z, caoDoNhacBut)}, false});
    for (const auto& net : cacNet) {
        if (net.empty()) {
            continue;
        }
        auto dau = diemAnhSangToaDoGiay(net.front());
        duongDiDuKien.push_back({{dau.x, dau.y, caoDoNhacBut}, false});
        for (auto p : net) {
            duongDiDuKien.push_back({diemAnhSangToaDoGiay(p), true});
        }
        auto cuoi = diemAnhSangToaDoGiay(net.back());
        duongDiDuKien.push_back({{cuoi.x, cuoi.y, caoDoNhacBut}, false});
    }
    if (duongDiDuKien.size() < 2) {
        baoTrangThai(L"Không tìm được nét. Thử ảnh nét đen trên nền trắng.");
        return;
    }
    // Kiem tra ca cac diem noi suy tren duong di truoc khi cho robot chay.
    truocDo = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
    for (const auto& diem : duongDiDuKien) {
        double doDai = robot::khoangCach(diem.viTri, truocDo);
        int soLuong = max(1, static_cast<int>(ceil(doDai / .06)));
        for (int i = 1; i <= soLuong; ++i) {
            auto p = robot::noiSuyTuyenTinh(truocDo, diem.viTri, double(i) / soLuong);
            auto nghiem = robot::giaiDongHocNghich(p, ungDung.nhanhKhuyu, ungDung.kichThuoc);
            if (!nghiem.thanhCong() || !tuTheNamTrenBan(nghiem.gocKhop)) {
                baoTrangThai(
                    L"Đường đi không hợp lệ với nhánh IK hiện tại. Chọn lại nhánh hoặc về tư thế đầu.");
                return;
            }
        }
        truocDo = diem.viTri;
    }
    ungDung.duongDi = move(duongDiDuKien);
    ungDung.cacNet = cacNet;
    ungDung.keTiep = 0;
    ungDung.dangChay = true;
    ungDung.coVetMucTruoc = false;
    ungDung.quyDaoThucTe.clear();
    ungDung.hienDiemDich = false;
    baoTrangThai(L"Đang thực hiện. Robot tự nhấc bút khi chuyển giữa các nét rời.");
}

void veNet() {
    ungDung.dangChay = false;
    ungDung.mauMucLuotNay = RGB(24, 38, 55);
    ungDung.cheDoVe = CheDoVe::VeNet;
    ungDung.matNaTo.clear();
    auto cacNet = xuLyAnh::tachNetVe(ungDung.anhNguon, ungDung.nguong, ungDung.cachLayNet);
    lapDuongDi(cacNet);
}

void toVungDaChon() {
    ungDung.dangChay = false;
    if (!ungDung.vungTo.hatGiongHopLe || ungDung.vungTo.soDiemAnh == 0) {
        baoTrangThai(L"Bấm vào phần muốn đổi màu trong ảnh bên trái.");
        return;
    }
    if (ungDung.anhNguon.rgb.empty() && ungDung.vungTo.chamMep) {
        baoTrangThai(L"Vùng chạm mép ảnh: hãy chọn vùng có đường viền khép kín.");
        return;
    }
    ungDung.cheDoVe = CheDoVe::ToVung;
    ungDung.matNaTo = ungDung.vungTo.matNa;
    ungDung.mauMucLuotNay = ungDung.muc;
    ungDung.toDeVienLuotNay = ungDung.choToDeVien;
    lapDuongDi(xuLyAnh::lapNetToTheoDong(ungDung.vungTo.matNa, ungDung.anhNguon.rong,
                                         ungDung.anhNguon.cao, 1));
}

void toToanAnhTheoMauGoc() {
    ungDung.toDeVienLuotNay = ungDung.choToDeVien;
    ungDung.matNaTo.resize(ungDung.anhNguon.mucXam.size());
    for (size_t i = 0; i < ungDung.matNaTo.size(); ++i) {
        ungDung.matNaTo[i] = ungDung.anhNguon.mauTai(i) != 0xffffffffu;
    }
    // Them vet muc cu tren nen trang vao luot khoi phuc mau goc.
    for (int y = 0; y < kichThuocTranh; ++y) {
        for (int x = 0; x < kichThuocTranh; ++x) {
            const int chiSoTranh = y * kichThuocTranh + x;
            if (ungDung.tranh[chiSoTranh] == 0xffffffffu) {
                continue;
            }
            if (!ungDung.toDeVienLuotNay && ungDung.matNaVien[chiSoTranh]) {
                continue;
            }
            ungDung.matNaTo[chiSoAnhTuDiemTranh(x, y)] = 1;
        }
    }
    ungDung.cheDoVe = CheDoVe::ToMauGoc;
    lapDuongDi(
        xuLyAnh::lapNetToTheoDong(ungDung.matNaTo, ungDung.anhNguon.rong, ungDung.anhNguon.cao));
    if (ungDung.dangChay) {
        baoTrangThai(L"Robot đang tô toàn ảnh theo màu gốc, từng dòng theo đầu bút.");
    } else if (none_of(ungDung.matNaTo.begin(), ungDung.matNaTo.end(),
                       [](auto giaTri) { return giaTri != 0; })) {
        baoTrangThai(L"Ảnh và mặt giấy đều trắng.");
    }
}

void capNhatChuyenDong(double khoangThoiGian) {
    if (ungDung.keTiep >= ungDung.duongDi.size()) {
        ungDung.dangChay = false;
        return;
    }
    double quangDuongConLai = ungDung.tocDo * khoangThoiGian;
    int soLanLap = 0;
    while (quangDuongConLai > 1e-8 && ungDung.keTiep < ungDung.duongDi.size() &&
           soLanLap++ < 2000) {
        auto hienTai = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
        const auto& diemQuyDao = ungDung.duongDi[ungDung.keTiep];
        const double doDai = robot::khoangCach(diemQuyDao.viTri, hienTai);
        const double buoc = min({quangDuongConLai, doDai, .025});
        auto diem = doDai < 1e-7 ? diemQuyDao.viTri
                                 : robot::noiSuyTuyenTinh(hienTai, diemQuyDao.viTri, buoc / doDai);
        auto ketQua = robot::giaiDongHocNghich(diem, ungDung.nhanhKhuyu, ungDung.kichThuoc);
        if (!ketQua.thanhCong() || !tuTheNamTrenBan(ketQua.gocKhop)) {
            ungDung.dangChay = false;
            ungDung.coVetMucTruoc = false;
            baoTrangThai(L"Dừng: tư thế vượt giới hạn hoặc cắt qua mặt bàn.");
            return;
        }
        ungDung.gocKhop = ketQua.gocKhop;
        auto viTriThuc = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
        if (diemQuyDao.haBut && abs(viTriThuc.z - caoDoGiay) < .002) {
            veVetMuc(viTriThuc);
        } else {
            ungDung.coVetMucTruoc = false;
        }
        if (ungDung.quyDaoThucTe.empty() ||
            robot::khoangCach(viTriThuc, ungDung.quyDaoThucTe.back()) > .035) {
            ungDung.quyDaoThucTe.push_back(viTriThuc);
        }
        quangDuongConLai -= max(buoc, 1e-7);
        if (doDai < .00001 || buoc >= doDai - 1e-7) {
            ++ungDung.keTiep;
        }
    }
    if (ungDung.keTiep >= ungDung.duongDi.size()) {
        ungDung.dangChay = false;
        baoTrangThai(L"Hoàn tất đường vẽ. Có thể chọn vùng và màu để tô tiếp.");
    }
}

wstring chonTep(bool luu, const wchar_t* boLoc, const wchar_t* duoiTep) {
    wchar_t tenTep[32768]{};
    OPENFILENAMEW hopThoai{};
    hopThoai.lStructSize = sizeof(hopThoai);
    hopThoai.hwndOwner = ungDung.cuaSo;
    hopThoai.lpstrFile = tenTep;
    hopThoai.nMaxFile = 32768;
    hopThoai.lpstrFilter = boLoc;
    hopThoai.lpstrDefExt = duoiTep;
    hopThoai.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST |
                     (luu ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
    bool hoanTat = luu ? GetSaveFileNameW(&hopThoai) : GetOpenFileNameW(&hopThoai);
    return hoanTat ? tenTep : L"";
}

void capNhatAnhXemTruoc() {
    const auto& anh = ungDung.anhNguon;
    const auto& matNa = ungDung.vungTo.matNa;
    ungDung.anhXemTruoc.resize(anh.mucXam.size());
    for (int y = 0; y < anh.cao; ++y) {
        for (int x = 0; x < anh.rong; ++x) {
            const int i = y * anh.rong + x;
            ungDung.anhXemTruoc[i] = anh.mauTai(i);
            if (!ungDung.vungTo.hatGiongHopLe || !matNa[i]) {
                continue;
            }
            const bool canh = x == 0 || y == 0 || x + 1 == anh.rong || y + 1 == anh.cao ||
                              !matNa[i - 1] || !matNa[i + 1] || !matNa[i - anh.rong] ||
                              !matNa[i + anh.rong];
            if (canh) {
                ungDung.anhXemTruoc[i] = ((x + y) / 4) % 2 ? 0xff171717u : 0xffffffffu;
            }
        }
    }
    EnableWindow(timDieuKhien(NutToVung),
                 ungDung.vungTo.hatGiongHopLe &&
                     (!ungDung.anhNguon.rgb.empty() || !ungDung.vungTo.chamMep));
}

void chonVungTo(xuLyAnh::DiemAnh diemHatGiong) {
    ungDung.diemHatGiong = diemHatGiong;
    ungDung.vungTo =
        ungDung.anhNguon.rgb.empty()
            ? xuLyAnh::timVungSang(ungDung.anhNguon, diemHatGiong, ungDung.nguong)
            : xuLyAnh::timVungCungMau(ungDung.anhNguon, diemHatGiong, ungDung.dungSaiMau);
    capNhatAnhXemTruoc();
    if (!ungDung.vungTo.hatGiongHopLe) {
        baoTrangThai(L"Bấm bên trong vùng ảnh có thể tô.");
    } else if (ungDung.anhNguon.rgb.empty() && ungDung.vungTo.chamMep) {
        baoTrangThai(L"Vùng chạm mép ảnh. Hãy chọn một vùng kín trong hình mẫu.");
    } else {
        baoTrangThai(L"Đã chọn vùng. Bấm Chọn màu… rồi Tô vùng đã chọn.");
    }
}

void datAnhNguon(xuLyAnh::Anh anh, const wstring& ten) {
    ungDung.anhNguon = move(anh);
    ungDung.tenAnh = ten;
    ungDung.diemHatGiong = {-1, -1};
    ungDung.vungTo = {};
    xoaMatGiay();
    ungDung.cacNet.clear();
    ungDung.matNaTo.clear();
    ungDung.cheDoVe = CheDoVe::VeNet;
    capNhatAnhXemTruoc();
    EnableWindow(timDieuKhien(ThanhDungSaiMau), !ungDung.anhNguon.rgb.empty());
    ungDung.quyDaoThucTe.clear();
    veTuTheBanDau();
    baoTrangThai(L"Đã nạp ảnh. Bấm Tô toàn ảnh theo màu gốc hoặc chọn một vùng để đổi màu.");
}

bool nhapTepAnh(const wstring& tenTep) {
    const path duongDanTep(tenTep);
    if (_wcsicmp(duongDanTep.extension().c_str(), L".svg") == 0) {
        xuLyAnh::Anh anh;
        wstring thongBaoLoi;
        if (!docSvg::docTep(duongDanTep, anh, thongBaoLoi)) {
            baoTrangThai(thongBaoLoi);
            return false;
        }
        datAnhNguon(move(anh), duongDanTep.filename().wstring());
        return true;
    }
    Gdiplus::Bitmap anhNguon(tenTep.c_str());
    if (anhNguon.GetLastStatus() != Gdiplus::Ok || anhNguon.GetWidth() == 0 ||
        anhNguon.GetHeight() == 0) {
        baoTrangThai(L"Không đọc được ảnh. Hãy thử một tệp PNG, JPEG, BMP hoặc SVG khác.");
        return false;
    }
    Gdiplus::Bitmap anhThuPhong(256, 256, PixelFormat32bppARGB);
    double tiLeThuPhong = min(240.0 / anhNguon.GetWidth(), 240.0 / anhNguon.GetHeight());
    int rong = max(1, static_cast<int>(anhNguon.GetWidth() * tiLeThuPhong));
    int cao = max(1, static_cast<int>(anhNguon.GetHeight() * tiLeThuPhong));
    const Gdiplus::Rect khung((256 - rong) / 2, (256 - cao) / 2, rong, cao);
    {
        Gdiplus::Graphics boVe(&anhThuPhong);
        boVe.Clear(Gdiplus::Color(0, 0, 0, 0));
        boVe.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        Gdiplus::ImageAttributes thuocTinh;
        thuocTinh.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
        if (boVe.DrawImage(&anhNguon, khung, 0, 0, anhNguon.GetWidth(), anhNguon.GetHeight(),
                           Gdiplus::UnitPixel, &thuocTinh) != Gdiplus::Ok) {
            baoTrangThai(L"Không kết xuất được ảnh.");
            return false;
        }
    }
    auto anh = xuLyAnh::Anh::taoAnhMau(256, 256);
    for (int y = khung.Y; y < khung.GetBottom(); ++y) {
        for (int x = khung.X; x < khung.GetRight(); ++x) {
            Gdiplus::Color mau;
            if (anhThuPhong.GetPixel(x, y, &mau) != Gdiplus::Ok) {
                baoTrangThai(L"Không đọc được màu ảnh.");
                return false;
            }
            anh.ganMauRGBA(y * 256 + x, mau.GetR(), mau.GetG(), mau.GetB(), mau.GetA());
        }
    }
    datAnhNguon(move(anh), duongDanTep.filename().wstring());
    return true;
}

void moAnh() {
    auto tenTep = chonTep(
        false,
        L"Ảnh SVG / PNG / JPEG / BMP\0*.svg;*.png;*.jpg;*.jpeg;*.bmp\0Đồ họa SVG (*.svg)\0*.svg\0\0",
        L"png");
    if (!tenTep.empty()) {
        nhapTepAnh(tenTep);
    }
}

bool luuTranhPng(const wstring& tenTep) {
    CLSID boMaHoa{0x557cf406, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
    Gdiplus::Bitmap anh(kichThuocTranh, kichThuocTranh, kichThuocTranh * 4, PixelFormat32bppARGB,
                        reinterpret_cast<BYTE*>(ungDung.tranh.data()));
    return anh.Save(tenTep.c_str(), &boMaHoa, nullptr) == Gdiplus::Ok;
}

void xuatTranh() {
    auto tenTep = chonTep(true, L"Tranh PNG\0*.png\0\0", L"png");
    if (tenTep.empty()) {
        return;
    }
    if (luuTranhPng(tenTep)) {
        baoTrangThai(L"Đã lưu tranh PNG theo những vệt bút đã thực hiện.");
    } else {
        baoTrangThai(L"Không lưu được PNG. Hãy chọn thư mục có quyền ghi.");
    }
}

void xuatMaTran() {
    auto tenTep = chonTep(true, L"Bảng CSV\0*.csv\0\0", L"csv");
    if (tenTep.empty()) {
        return;
    }
    FILE* tep = _wfopen(tenTep.c_str(), L"wb");
    if (!tep) {
        baoTrangThai(L"Không ghi được tệp CSV.");
        return;
    }
    auto tuThe = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc);
    fprintf(tep, "part,row,c0,c1,c2,c3\n");
    const robot::MaTran4 cacMaTran[] = {tuThe.de, tuThe.vai, tuThe.khuyu, tuThe.coTay,
                                        tuThe.dauBut};
    const char* cacTen[] = {"base", "shoulder", "elbow", "wrist", "tool"};
    for (int boPhan = 0; boPhan < 5; ++boPhan) {
        const auto& maTran = cacMaTran[boPhan];
        for (int hang = 0; hang < 4; ++hang) {
            fprintf(tep, "%s,%d,%.9f,%.9f,%.9f,%.9f\n", cacTen[boPhan], hang, maTran.tai(hang, 0),
                    maTran.tai(hang, 1), maTran.tai(hang, 2), maTran.tai(hang, 3));
        }
    }
    bool coLoi = ferror(tep) != 0;
    fclose(tep);
    baoTrangThai(coLoi ? L"Ghi CSV bị lỗi."
                       : L"Đã xuất ma trận của 5 bộ phận so với hệ trục cố định ở chân đế.");
}

void datVatLieu(float r, float g, float b) {
    GLfloat mauKhuechTan[] = {r, g, b, 1}, mauPhanXa[] = {.45f, .45f, .48f, 1};
    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mauKhuechTan);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mauPhanXa);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 42);
}

void veKhoiHop(double x, double y, double z) {
    const double X = x / 2, Y = y / 2, Z = z / 2;
    glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glVertex3d(-X, -Y, Z);
    glVertex3d(X, -Y, Z);
    glVertex3d(X, Y, Z);
    glVertex3d(-X, Y, Z);
    glNormal3d(0, 0, -1);
    glVertex3d(-X, Y, -Z);
    glVertex3d(X, Y, -Z);
    glVertex3d(X, -Y, -Z);
    glVertex3d(-X, -Y, -Z);
    glNormal3d(0, -1, 0);
    glVertex3d(-X, -Y, -Z);
    glVertex3d(X, -Y, -Z);
    glVertex3d(X, -Y, Z);
    glVertex3d(-X, -Y, Z);
    glNormal3d(0, 1, 0);
    glVertex3d(X, Y, -Z);
    glVertex3d(-X, Y, -Z);
    glVertex3d(-X, Y, Z);
    glVertex3d(X, Y, Z);
    glNormal3d(1, 0, 0);
    glVertex3d(X, -Y, -Z);
    glVertex3d(X, Y, -Z);
    glVertex3d(X, Y, Z);
    glVertex3d(X, -Y, Z);
    glNormal3d(-1, 0, 0);
    glVertex3d(-X, Y, -Z);
    glVertex3d(-X, -Y, -Z);
    glVertex3d(-X, -Y, Z);
    glVertex3d(-X, Y, Z);
    glEnd();
}

void veHinhTru(double banKinh, double doDai, int soMat = 36) {
    gluCylinder(ungDung.khoiTronXoay, banKinh, banKinh, doDai, soMat, 1);
    glPushMatrix();
    glRotated(180, 1, 0, 0);
    gluDisk(ungDung.khoiTronXoay, 0, banKinh, soMat, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, doDai);
    gluDisk(ungDung.khoiTronXoay, 0, banKinh, soMat, 1);
    glPopMatrix();
}

void veHopTaiViTri(double x, double y, double z, double w, double d, double h) {
    glPushMatrix();
    glTranslated(x, y, z);
    veKhoiHop(w, d, h);
    glPopMatrix();
}

GLuint taoKyTuTruc() {
    const GLuint dauDanhSach = glGenLists(3);
    if (!dauDanhSach) {
        return 0;
    }
    const auto phongCu = SelectObject(ungDung.glDC, ungDung.phongTieuDe);
    const BOOL daTao = wglUseFontBitmapsW(ungDung.glDC, L'X', 3, dauDanhSach);
    SelectObject(ungDung.glDC, phongCu);
    if (!daTao) {
        glDeleteLists(dauDanhSach, 3);
        return 0;
    }
    return dauDanhSach;
}

void veHeTruc(const robot::Vecto3& doDai) {
    const robot::Vecto3 dauTruc[] = {{doDai.x, 0, 0}, {0, doDai.y, 0}, {0, 0, doDai.z}};
    const GLfloat mauTruc[][3] = {{1, .38f, .35f}, {.35f, .92f, .47f}, {.40f, .68f, 1}};
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    // Ve he truc tren canh de ca tam khop nam trong vo van de nhan ra.
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2);
    glBegin(GL_LINES);
    for (int truc = 0; truc < 3; ++truc) {
        const auto& diem = dauTruc[truc];
        glColor3fv(mauTruc[truc]);
        glVertex3d(0, 0, 0);
        glVertex3d(diem.x, diem.y, diem.z);
    }
    glEnd();
    if (ungDung.kyTuTruc) {
        for (int truc = 0; truc < 3; ++truc) {
            const auto& diem = dauTruc[truc];
            // Chu nam tai dau truc 3D, luon doc theo chieu man hinh.
            glColor3f(.04f, .06f, .09f);
            glRasterPos3d(diem.x, diem.y, diem.z);
            glBitmap(0, 0, 0, 0, 5, -5, nullptr);
            glCallList(ungDung.kyTuTruc + truc);
            glColor3fv(mauTruc[truc]);
            glRasterPos3d(diem.x, diem.y, diem.z);
            glBitmap(0, 0, 0, 0, 4, -4, nullptr);
            glCallList(ungDung.kyTuTruc + truc);
        }
    }
    glPopAttrib();
}

void veHeTrucCacKhop(const robot::TuTheRobot& tuThe) {
    const robot::MaTran4* maTran[] = {&tuThe.de, &tuThe.vai, &tuThe.khuyu, &tuThe.coTay,
                                      &tuThe.dauBut};
    // Ngan truc Z cua de de nhan khong nam tren tam khop vai.
    const robot::Vecto3 doDai[] = {
        {1.3, 1.3, .72}, {.9, .9, .9}, {.9, .9, .9}, {.9, .9, .9}, {.7, .7, .7}};
    for (int boPhan = 0; boPhan < 5; ++boPhan) {
        glPushMatrix();
        glMultMatrixd(maTran[boPhan]->data());
        veHeTruc(doDai[boPhan]);
        glPopMatrix();
    }
}

void veVoKhop(const robot::MaTran4& maTran, int chiSo) {
    glPushMatrix();
    glMultMatrixd(maTran.data());
    double e = ungDung.doBocTach;
    // Khi mo vo khop, co the quan sat truc, dong co va dia rang.
    datVatLieu(.28f, .34f, .4f);
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glTranslated(0, 0, -.48);
    veHinhTru(.28, .96);
    glPopMatrix();
    datVatLieu(.13f, .17f, .22f);
    veHopTaiViTri(0, .38 + e * .9, 0, .46, .32, .46);
    datVatLieu(.65f, .7f, .76f);
    glPushMatrix();
    glTranslated(0, -.23 - e * .5, 0);
    glRotated(90, 1, 0, 0);
    veHinhTru(.36, .1);
    for (int i = 0; i < 18; ++i) {
        glPushMatrix();
        glRotated(i * 20, 0, 0, 1);
        veHopTaiViTri(.35, 0, .05, .09, .07, .12);
        glPopMatrix();
    }
    glPopMatrix();
    if (ungDung.kieuHienThi != 1) {
        if (chiSo == ungDung.dangChon) {
            datVatLieu(1, .67f, .16f);
        } else {
            datVatLieu(.95f, .36f, .11f);
        }
        for (int phia : {-1, 1}) {
            glPushMatrix();
            glTranslated(0, phia * (.44 + e * 1.5), 0);
            glRotated(90, 1, 0, 0);
            veHinhTru(.43, .09);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void veDoanTay(const robot::MaTran4& maTran, double doDai) {
    glPushMatrix();
    glMultMatrixd(maTran.data());
    datVatLieu(.38f, .45f, .52f);
    veHopTaiViTri(doDai / 2, 0, 0, doDai - .35, .22, .22);
    datVatLieu(.08f, .11f, .15f);
    veHopTaiViTri(doDai / 2, 0, .17, doDai - .65, .12, .08);
    if (ungDung.kieuHienThi != 1) {
        datVatLieu(.9f, .31f, .085f);
        for (int phia : {-1, 1}) {
            veHopTaiViTri(doDai / 2, phia * (.24 + ungDung.doBocTach), 0, doDai - .5, .10, .48);
        }
    }
    datVatLieu(.7f, .74f, .78f);
    for (double x : {.5, doDai - .5}) {
        for (int phia : {-1, 1}) {
            glPushMatrix();
            glTranslated(x, phia * (.31 + ungDung.doBocTach), 0);
            gluSphere(ungDung.khoiTronXoay, .065, 10, 8);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void veBut(const robot::TuTheRobot& tuThe) {
    // Ve but trong cung he toa do voi diem cham giay.
    constexpr double daiNgoi = .13, daiChop = .17, sauDauKep = .31;
    constexpr double banKinhThanBut = .095;
    glPushMatrix();
    glMultMatrixd(tuThe.dauBut.data());
    datVatLieu(.12f, .15f, .19f);
    gluCylinder(ungDung.khoiTronXoay, 0, .044, daiNgoi, 24, 1);
    glTranslated(0, 0, daiNgoi);
    datVatLieu(.86f, .90f, .94f);
    gluCylinder(ungDung.khoiTronXoay, .044, banKinhThanBut, daiChop, 24, 1);
    glTranslated(0, 0, daiChop);
    if (ungDung.dangChon == 4) {
        datVatLieu(1, .67f, .16f);
    } else {
        datVatLieu(.13f, .78f, .80f);
    }
    veHinhTru(banKinhThanBut, ungDung.kichThuoc.chieuDaiBut - sauDauKep - daiNgoi - daiChop, 24);
    glPopMatrix();

    glPushMatrix();
    glMultMatrixd(tuThe.coTay.data());
    datVatLieu(.24f, .30f, .37f);
    glTranslated(0, 0, -sauDauKep);
    veHinhTru(.145, sauDauKep);
    glPopMatrix();
}

void veRobot(const robot::TuTheRobot& tuThe) {
    if (ungDung.kieuHienThi == 2) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    datVatLieu(.16f, .20f, .26f);
    veHopTaiViTri(0, 0, .12, 1.65, 1.45, .24);
    datVatLieu(.38f, .45f, .51f);
    glPushMatrix();
    glTranslated(0, 0, .24);
    veHinhTru(.51, .66);
    glPopMatrix();
    glPushMatrix();
    glMultMatrixd(tuThe.de.data());
    if (ungDung.dangChon == 0) {
        datVatLieu(1, .67f, .16f);
    } else {
        datVatLieu(.95f, .36f, .11f);
    }
    glTranslated(0, 0, .91);
    veHinhTru(.43, .29);
    glPopMatrix();
    veVoKhop(tuThe.vai, 1);
    veDoanTay(tuThe.vai, ungDung.kichThuoc.canhTayTren);
    veVoKhop(tuThe.khuyu, 2);
    veDoanTay(tuThe.khuyu, ungDung.kichThuoc.cangTay);
    veVoKhop(tuThe.coTay, 3);
    veBut(tuThe);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void veKhungCanh() {
    if (!ungDung.glRC) {
        return;
    }
    wglMakeCurrent(ungDung.glDC, ungDung.glRC);
    glViewport(0, 0, ungDung.rongKhungCanh, ungDung.caoKhungCanh);
    glClearColor(.065f, .09f, .13f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (ungDung.kiemTraDoSau) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double tiLeKhung = double(ungDung.rongKhungCanh) / max(1, ungDung.caoKhungCanh);
    if (ungDung.phoiCanh) {
        gluPerspective(43, tiLeKhung, .1, 70);
    } else {
        double kichCo = ungDung.khoangCach * .38;
        glOrtho(-kichCo * tiLeKhung, kichCo * tiLeKhung, -kichCo, kichCo, .1, 70);
    }
    glGetDoublev(GL_PROJECTION_MATRIX, ungDung.maTranChieu);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    double a = sangRadian(ungDung.gocPhuongVi), e = sangRadian(ungDung.gocNang),
           r = ungDung.khoangCach;
    gluLookAt(2 + r * cos(e) * cos(a), r * cos(e) * sin(a), 1.3 + r * sin(e), 2, 0, 1.3, 0, 0, 1);
    glGetDoublev(GL_MODELVIEW_MATRIX, ungDung.maTranQuanSat);
    glGetIntegerv(GL_VIEWPORT, ungDung.khungHienThi);
    GLfloat nguonSang[] = {-3, -4, 10, 1}, anhSangNen[] = {.25f, .28f, .34f, 1},
            mauKhuechTan[] = {.9f, .88f, .82f, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, nguonSang);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, mauKhuechTan);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, anhSangNen);
    if (ungDung.chieuSang) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    } else {
        glDisable(GL_LIGHTING);
    }
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    datVatLieu(.12f, .16f, .21f);
    veHopTaiViTri(2, 0, -.08, 13, 10, .12);
    glDisable(GL_LIGHTING);
    glColor3f(.20f, .26f, .32f);
    glLineWidth(1);
    glBegin(GL_LINES);
    for (int i = -5; i <= 8; ++i) {
        glVertex3d(i, -5, 0);
        glVertex3d(i, 5, 0);
    }
    for (int i = -5; i <= 5; ++i) {
        glVertex3d(-4, i, 0);
        glVertex3d(8, i, 0);
    }
    glEnd();
    if (ungDung.chieuSang) {
        glEnable(GL_LIGHTING);
    }
    datVatLieu(.25f, .31f, .38f);
    veHopTaiViTri(gocGiayX + canhGiay / 2, 0, .10, canhGiay + .22, canhGiay + .22, .22);
    if (ungDung.canCapNhatTranh) {
        glBindTexture(GL_TEXTURE_2D, ungDung.hoaTietGiay);
        // Tranh luu gia tri ARGB. Tren Windows little-endian, thu tu byte la BGRA.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kichThuocTranh, kichThuocTranh, 0, GL_BGRA_EXT,
                     GL_UNSIGNED_BYTE, ungDung.tranh.data());
        ungDung.canCapNhatTranh = false;
    }
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ungDung.hoaTietGiay);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex3d(gocGiayX, gocGiayY, caoDoGiay);
    glTexCoord2d(1, 1);
    glVertex3d(gocGiayX + canhGiay, gocGiayY, caoDoGiay);
    glTexCoord2d(1, 0);
    glVertex3d(gocGiayX + canhGiay, gocGiayY + canhGiay, caoDoGiay);
    glTexCoord2d(0, 0);
    glVertex3d(gocGiayX, gocGiayY + canhGiay, caoDoGiay);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (ungDung.hienQuyDao && ungDung.quyDaoThucTe.size() > 1) {
        glColor3f(.22f, .74f, .85f);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        for (auto p : ungDung.quyDaoThucTe) {
            glVertex3d(p.x, p.y, p.z + .012);
        }
        glEnd();
    }
    if (ungDung.chieuSang) {
        glEnable(GL_LIGHTING);
    }
    const auto tuThe = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc);
    veRobot(tuThe);
    if (ungDung.hienDiemDich) {
        glDisable(GL_LIGHTING);
        glColor3f(ungDung.dichHopLe ? .2f : 1, ungDung.dichHopLe ? .9f : .2f, .3f);
        glPushMatrix();
        glTranslated(ungDung.dich.x, ungDung.dich.y, ungDung.dich.z);
        gluSphere(ungDung.khoiTronXoay, .12, 20, 12);
        glPopMatrix();
    }
    if (ungDung.heTruc) {
        veHeTrucCacKhop(tuThe);
    }
    glFlush();
    SwapBuffers(ungDung.glDC);
}

void veChu(HDC dc, int x, int y, const wstring& giaTri, COLORREF mau = RGB(37, 51, 69),
           HFONT phongChu = nullptr) {
    SelectObject(dc, phongChu ? phongChu : ungDung.phongChu);
    SetTextColor(dc, mau);
    SetBkMode(dc, TRANSPARENT);
    TextOutW(dc, x, y, giaTri.c_str(), static_cast<int>(giaTri.size()));
}

wstring dinhDangSo(double giaTri, int soChuSoLe = 1) {
    wostringstream s;
    s << fixed << setprecision(soChuSoLe) << giaTri;
    return s.str();
}

void veBangMaTran(HDC dc, int trai, int tren, const wchar_t* tenBoPhan,
                  const robot::MaTran4& maTran) {
    const COLORREF mauSo = RGB(47, 62, 80);
    const COLORREF mauViTri = RGB(37, 88, 168);
    veChu(dc, trai, tren + 6, wstring(L"Ma trận tổng hợp · ") + tenBoPhan, mauViTri);
    veChu(dc, trai, tren + 28, L"Về hệ trục cố định ở chân đế", mauSo);

    const wchar_t* tenCot[] = {L"X", L"Y", L"Z", L"W"};
    const wchar_t* tenHang[] = {L"Trục X", L"Trục Y", L"Trục Z", L"Vị trí"};
    const int mepTrai = trai + 55;
    constexpr int rongCot = 55;
    for (int cot = 0; cot < 4; ++cot) {
        veChu(dc, mepTrai + 24 + cot * rongCot, tren + 52, tenCot[cot], mauSo);
    }
    for (int hang = 0; hang < 4; ++hang) {
        const int y = tren + 77 + hang * 22;
        const COLORREF mau = hang == 3 ? mauViTri : mauSo;
        veChu(dc, trai, y - 2, tenHang[hang], mau);
        for (int cot = 0; cot < 4; ++cot) {
            const double giaTri = maTran.tai(hang, cot);
            wchar_t so[24];
            swprintf(so, 24, L"%5.2f", abs(giaTri) < 0.005 ? 0.0 : giaTri);
            veChu(dc, mepTrai + 9 + cot * rongCot, y, so, mau, ungDung.phongDonCach);
        }
    }

    const int phai = mepTrai + 4 * rongCot + 2;
    POINT ngoacTrai[] = {{mepTrai + 7, tren + 75},
                         {mepTrai, tren + 75},
                         {mepTrai, tren + 162},
                         {mepTrai + 7, tren + 162}};
    POINT ngoacPhai[] = {
        {phai - 7, tren + 75}, {phai, tren + 75}, {phai, tren + 162}, {phai - 7, tren + 162}};
    const auto butCu = SelectObject(dc, GetStockObject(DC_PEN));
    const auto mauCu = SetDCPenColor(dc, mauSo);
    Polyline(dc, ngoacTrai, 4);
    Polyline(dc, ngoacPhai, 4);
    SetDCPenColor(dc, mauCu);
    SelectObject(dc, butCu);
}

void veBangDieuKhien(HDC dc) {
    RECT vungKhach{0, 0, ungDung.rong, ungDung.cao};
    HBRUSH nen = CreateSolidBrush(RGB(245, 247, 250));
    FillRect(dc, &vungKhach, nen);
    DeleteObject(nen);
    veChu(dc, 20, 16, L"ROBOT DRAWING LAB", RGB(24, 38, 55), ungDung.phongTieuDe);
    veChu(dc, 21, 46, L"C++ / OpenGL · Mô phỏng tương tác", RGB(87, 103, 122));
    if (ungDung.the == 0) {
        const int kichCo = 200, ox = 80, oy = 191;
        BITMAPINFO anhBitmap{};
        anhBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        anhBitmap.bmiHeader.biWidth = ungDung.anhNguon.rong;
        anhBitmap.bmiHeader.biHeight = -ungDung.anhNguon.cao;
        anhBitmap.bmiHeader.biPlanes = 1;
        anhBitmap.bmiHeader.biBitCount = 32;
        anhBitmap.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(dc, ox, oy, kichCo, kichCo, 0, 0, ungDung.anhNguon.rong, ungDung.anhNguon.cao,
                      ungDung.anhXemTruoc.data(), &anhBitmap, DIB_RGB_COLORS, SRCCOPY);
        if (ungDung.diemHatGiong.x >= 0) {
            int x = ox + ungDung.diemHatGiong.x * kichCo / ungDung.anhNguon.rong,
                y = oy + ungDung.diemHatGiong.y * kichCo / ungDung.anhNguon.cao;
            HPEN haBut = CreatePen(PS_SOLID, 2, RGB(35, 99, 220));
            auto cu = SelectObject(dc, haBut);
            MoveToEx(dc, x - 6, y, nullptr);
            LineTo(dc, x + 6, y);
            MoveToEx(dc, x, y - 6, nullptr);
            LineTo(dc, x, y + 6);
            SelectObject(dc, cu);
            DeleteObject(haBut);
        }
        RECT oMau{20, 744, 41, 763};
        HBRUSH choiMau = CreateSolidBrush(ungDung.muc);
        FillRect(dc, &oMau, choiMau);
        DeleteObject(choiMau);
        FrameRect(dc, &oMau, GetSysColorBrush(COLOR_WINDOWFRAME));
        wchar_t nhanMuc[20];
        swprintf(nhanMuc, 20, L"Mực #%02X%02X%02X", unsigned(GetRValue(ungDung.muc)),
                 unsigned(GetGValue(ungDung.muc)), unsigned(GetBValue(ungDung.muc)));
        veChu(dc, 48, 742, nhanMuc);
        veChu(dc, 20, 768,
              L"Ngưỡng " + to_wstring(ungDung.nguong) + L"  ·  Dung sai " +
                  to_wstring(ungDung.dungSaiMau) + L"  ·  Tốc độ " + dinhDangSo(ungDung.tocDo));
        veChu(dc, 20, 792,
              L"Nét: " + to_wstring(ungDung.cacNet.size()) + L"  ·  Điểm: " +
                  to_wstring(ungDung.keTiep) + L" / " + to_wstring(ungDung.duongDi.size()));
    }
    int vx = rongBangDieuKhien + 20;
    RECT tren{rongBangDieuKhien, 0, ungDung.rong, 62};
    HBRUSH vungToi = CreateSolidBrush(RGB(20, 30, 43));
    FillRect(dc, &tren, vungToi);
    DeleteObject(vungToi);
    veChu(dc, vx, 11,
          ungDung.the == 0   ? L"MẶT GIẤY & CHUYỂN ĐỘNG"
          : ungDung.the == 1 ? L"KIỂM TRA KẾT CẤU"
                             : L"TÌNH HUỐNG & THUẬT TOÁN",
          RGB(238, 242, 248));
    veChu(dc, vx, 35, L"Kéo chuột phải: xoay cảnh   ·   Cuộn: thu/phóng   ·   Phím 1/2/3: góc nhìn",
          RGB(159, 179, 200));
    int duoi = ungDung.cao - 178;
    RECT bangThongTin{rongBangDieuKhien, duoi, ungDung.rong, ungDung.cao};
    HBRUSH trang = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(dc, &bangThongTin, trang);
    DeleteObject(trang);
    auto tuThe = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc);
    auto p = tuThe.viTriDauBut;
    veChu(dc, vx, duoi + 10,
          L"Đầu bút   X " + dinhDangSo(p.x, 3) + L"    Y " + dinhDangSo(p.y, 3) + L"    Z " +
              dinhDangSo(p.z, 3));
    veChu(dc, vx, duoi + 36,
          L"Góc khớp   " + dinhDangSo(sangDo(ungDung.gocKhop.gocDe)) + L"°    " +
              dinhDangSo(sangDo(ungDung.gocKhop.vai)) + L"°    " +
              dinhDangSo(sangDo(ungDung.gocKhop.khuyu)) + L"°    " +
              dinhDangSo(sangDo(ungDung.gocKhop.coTay)) + L"°");
    const robot::MaTran4* cacMaTran[] = {&tuThe.de, &tuThe.vai, &tuThe.khuyu, &tuThe.coTay,
                                         &tuThe.dauBut};
    const wchar_t* tenBoPhan[] = {L"Đế", L"Vai", L"Khuỷu", L"Cổ tay", L"Đầu bút"};
    const int boPhan = ungDung.dangChon;
    const wchar_t* tenMaTran[] = {L"Ma trận đế", L"Ma trận vai", L"Ma trận khuỷu",
                                  L"Ma trận cổ tay", L"Ma trận đầu bút"};
    const wchar_t* bienDoiTaiCho[] = {
        L"Quay đế quanh trục Z", L"Quay tại vai × Tịnh tiến theo chiều cao vai",
        L"Quay tại khuỷu × Tịnh tiến dọc đoạn tay trên",
        L"Quay tại cổ tay × Tịnh tiến dọc cẳng tay", L"Tịnh tiến từ tâm cổ tay đến ngòi bút"};
    veChu(dc, vx, duoi + 58, wstring(tenMaTran[boPhan]) + L" =", RGB(47, 62, 80));
    veChu(dc, vx + 12, duoi + 80, bienDoiTaiCho[boPhan], RGB(37, 88, 168));
    if (boPhan > 0) {
        veChu(dc, vx + 12, duoi + 102, wstring(L"× ") + tenMaTran[boPhan - 1], RGB(37, 88, 168));
    }
    RECT khungTrangThai{vx, duoi + 130, ungDung.rong - 320, ungDung.cao - 7};
    SelectObject(dc, ungDung.phongChu);
    SetTextColor(dc, RGB(51, 70, 92));
    DrawTextW(dc, ungDung.trangThai.c_str(), -1, &khungTrangThai, DT_WORDBREAK);
    veBangMaTran(dc, ungDung.rong - 302, duoi, tenBoPhan[boPhan], *cacMaTran[boPhan]);
}

void thuToaDoDich(robot::Vecto3 dich) {
    ungDung.dangChay = false;
    ungDung.dich = dich;
    ungDung.hienDiemDich = true;
    auto ketQua = robot::giaiDongHocNghich(dich, ungDung.nhanhKhuyu, ungDung.kichThuoc);
    ungDung.dichHopLe = ketQua.thanhCong() && tuTheNamTrenBan(ketQua.gocKhop);
    if (ungDung.dichHopLe) {
        ungDung.duongDi.clear();
        ungDung.keTiep = 0;
        ungDung.coVetMucTruoc = false;
        ungDung.gocKhop = ketQua.gocKhop;
        capNhatThanhGocKhop();
        baoTrangThai(L"Mục tiêu hợp lệ. Đã kiểm tra lại vị trí đầu bút bằng động học thuận.");
    } else {
        baoTrangThai(
            L"Mục tiêu bị từ chối: ngoài tầm với, vượt góc khớp hoặc chạm mặt bàn. Tư thế trước được giữ.");
    }
}

void chonTrongCanh(int x, int y) {
    if (ungDung.the == 1) {
        auto tuThe = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc);
        const robot::Vecto3 cacViTri[] = {tuThe.viTriDe, tuThe.viTriVai, tuThe.viTriKhuyu,
                                          tuThe.viTriCoTay, tuThe.viTriDauBut};
        double ganNhat = 50;
        for (int i = 0; i < 5; ++i) {
            GLdouble sx, sy, sz;
            auto p = cacViTri[i];
            gluProject(p.x, p.y, p.z, ungDung.maTranQuanSat, ungDung.maTranChieu,
                       ungDung.khungHienThi, &sx, &sy, &sz);
            double d = hypot(x - sx, y - (ungDung.caoKhungCanh - sy));
            if (d < ganNhat) {
                ganNhat = d;
                ungDung.dangChon = i;
            }
        }
        yeuCauVeLai();
        return;
    }
    if (ungDung.the != 2) {
        return;
    }
    GLdouble nx, ny, nz, fx, fy, fz;
    gluUnProject(x, ungDung.caoKhungCanh - y, 0, ungDung.maTranQuanSat, ungDung.maTranChieu,
                 ungDung.khungHienThi, &nx, &ny, &nz);
    gluUnProject(x, ungDung.caoKhungCanh - y, 1, ungDung.maTranQuanSat, ungDung.maTranChieu,
                 ungDung.khungHienThi, &fx, &fy, &fz);
    if (abs(fz - nz) < 1e-8) {
        return;
    }
    double t = (caoDoGiay - nz) / (fz - nz);
    if (t > 0) {
        thuToaDoDich({nx + t * (fx - nx), ny + t * (fy - ny), caoDoGiay});
    }
}

LRESULT CALLBACK xuLySuKienKhungCanh(HWND hwnd, UINT thongBao, WPARAM wp, LPARAM lp) {
    switch (thongBao) {
        case WM_PAINT: {
            PAINTSTRUCT thongTinVe;
            BeginPaint(hwnd, &thongTinVe);
            veKhungCanh();
            EndPaint(hwnd, &thongTinVe);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_RBUTTONDOWN:
            ungDung.dangKeoChuot = true;
            ungDung.viTriChuotTruoc = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            SetCapture(hwnd);
            return 0;
        case WM_RBUTTONUP:
            ungDung.dangKeoChuot = false;
            ReleaseCapture();
            return 0;
        case WM_CAPTURECHANGED:
            ungDung.dangKeoChuot = false;
            return 0;
        case WM_MOUSEMOVE:
            if (ungDung.dangKeoChuot) {
                int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                ungDung.gocPhuongVi += (x - ungDung.viTriChuotTruoc.x) * .45;
                ungDung.gocNang =
                    clamp(ungDung.gocNang + (y - ungDung.viTriChuotTruoc.y) * .35, 8.0, 88.0);
                ungDung.viTriChuotTruoc = {x, y};
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEWHEEL:
            ungDung.khoangCach =
                clamp(ungDung.khoangCach - GET_WHEEL_DELTA_WPARAM(wp) / 500.0, 5.0, 24.0);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_LBUTTONDOWN:
            SetFocus(ungDung.cuaSo);
            chonTrongCanh(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            return 0;
    }
    return DefWindowProcW(hwnd, thongBao, wp, lp);
}

void capNhatKichThuoc() {
    RECT r;
    GetClientRect(ungDung.cuaSo, &r);
    ungDung.rong = r.right;
    ungDung.cao = r.bottom;
    ungDung.rongKhungCanh = max(1, ungDung.rong - rongBangDieuKhien);
    ungDung.caoKhungCanh = max(1, ungDung.cao - 240);
    MoveWindow(ungDung.khungCanh, rongBangDieuKhien, 62, ungDung.rongKhungCanh,
               ungDung.caoKhungCanh, TRUE);
    yeuCauVeLai();
}

void xuLyLenh(int id) {
    switch (id) {
        case NutMoAnh:
            moAnh();
            break;
        case HopCachLayNet:
            ungDung.cachLayNet = static_cast<xuLyAnh::CachLayNet>(
                SendMessageW(timDieuKhien(HopCachLayNet), CB_GETCURSEL, 0, 0));
            break;
        case NutAnhHoa:
            datAnhNguon(xuLyAnh::taoHoaMau(), L"Ảnh mẫu bông hoa");
            break;
        case VeNet:
            veNet();
            break;
        case NutToMauGoc:
            toToanAnhTheoMauGoc();
            break;
        case OToDeVien:
            ungDung.choToDeVien =
                SendMessageW(timDieuKhien(OToDeVien), BM_GETCHECK, 0, 0) == BST_CHECKED;
            baoTrangThai(ungDung.choToDeVien ? L"Lượt tô tiếp theo sẽ tô đè lên viền trong vùng tô."
                                             : L"Lượt tô tiếp theo sẽ giữ viền đã vẽ.");
            break;
        case NutToVung:
            toVungDaChon();
            break;
        case NutChonMau: {
            static COLORREF bangMauRieng[16]{};
            CHOOSECOLORW hopThoai{};
            hopThoai.lStructSize = sizeof(hopThoai);
            hopThoai.hwndOwner = ungDung.cuaSo;
            hopThoai.rgbResult = ungDung.muc;
            hopThoai.lpCustColors = bangMauRieng;
            hopThoai.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColorW(&hopThoai)) {
                ungDung.muc = hopThoai.rgbResult;
                yeuCauVeLai();
            }
            break;
        }
        case NutLuuTranh:
            xuatTranh();
            break;
        case NutChayTamDung:
            if (ungDung.duongDi.empty()) {
                baoTrangThai(L"Bấm Vẽ nét hoặc Tô vùng để tạo đường đi trước.");
            } else if (ungDung.keTiep >= ungDung.duongDi.size()) {
                baoTrangThai(L"Đã hoàn tất. Bấm Về đầu đường vẽ để chạy lại.");
            } else {
                ungDung.dangChay = !ungDung.dangChay;
                baoTrangThai(ungDung.dangChay ? L"Đang chạy."
                                              : L"Đã tạm dừng. Có thể kiểm tra ma trận hiện tại.");
            }
            break;
        case NutTienMotBuoc:
            ungDung.dangChay = false;
            capNhatChuyenDong(.07);
            yeuCauVeLai();
            break;
        case NutVeDau:
            datLaiDuongVe();
            break;
        case NutXoaGiay:
            xoaMatGiay();
            yeuCauVeLai();
            break;
        case NutVeTuTheDau:
            veTuTheBanDau();
            break;
        case HopKieuHienThi:
            ungDung.kieuHienThi =
                static_cast<int>(SendMessageW(timDieuKhien(HopKieuHienThi), CB_GETCURSEL, 0, 0));
            yeuCauVeLai();
            break;
        case OHeTruc:
            ungDung.heTruc = SendMessageW(timDieuKhien(OHeTruc), BM_GETCHECK, 0, 0) == BST_CHECKED;
            yeuCauVeLai();
            break;
        case OQuyDao:
            ungDung.hienQuyDao =
                SendMessageW(timDieuKhien(OQuyDao), BM_GETCHECK, 0, 0) == BST_CHECKED;
            yeuCauVeLai();
            break;
        case OPhepChieu:
            ungDung.phoiCanh =
                SendMessageW(timDieuKhien(OPhepChieu), BM_GETCHECK, 0, 0) == BST_CHECKED;
            yeuCauVeLai();
            break;
        case OChieuSang:
            ungDung.chieuSang =
                SendMessageW(timDieuKhien(OChieuSang), BM_GETCHECK, 0, 0) == BST_CHECKED;
            yeuCauVeLai();
            break;
        case ODemDoSau:
            ungDung.kiemTraDoSau =
                SendMessageW(timDieuKhien(ODemDoSau), BM_GETCHECK, 0, 0) == BST_CHECKED;
            yeuCauVeLai();
            break;
        case NutThuTrongTam:
            thuToaDoDich({3.8, .4, caoDoGiay});
            break;
        case NgoaiTamVoi:
            thuToaDoDich({7.4, 0, caoDoGiay});
            break;
        case NutThuGioiHan: {
            ungDung.dangChay = false;
            auto ungVien = ungDung.gocKhop;
            ungVien.vai = sangRadian(170);
            if (!robot::GioiHanKhop{}.chua(ungVien)) {
                baoTrangThai(L"Yêu cầu vai 170° bị từ chối. Giới hạn mô hình là -25° đến 150°.");
            }
            break;
        }
        case NutDoiNhanh: {
            ungDung.dangChay = false;
            auto truocDo = ungDung.nhanhKhuyu;
            ungDung.nhanhKhuyu = truocDo == robot::NhanhKhuyu::KhuyuTren
                                     ? robot::NhanhKhuyu::KhuyuDuoi
                                     : robot::NhanhKhuyu::KhuyuTren;
            auto viTri = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
            auto ketQua = robot::giaiDongHocNghich(viTri, ungDung.nhanhKhuyu, ungDung.kichThuoc);
            if (!ketQua.thanhCong() || !tuTheNamTrenBan(ketQua.gocKhop)) {
                ungDung.nhanhKhuyu = truocDo;
                baoTrangThai(
                    L"Nhánh IK còn lại bị giới hạn góc hoặc cắt mặt bàn tại mục tiêu này.");
            } else {
                ungDung.gocKhop = ketQua.gocKhop;
                capNhatThanhGocKhop();
                baoTrangThai(
                    L"Đã đổi nhánh IK, tọa độ đầu bút giữ nguyên. Đường vẽ cũ cần được tạo lại.");
                ungDung.duongDi.clear();
                ungDung.keTiep = 0;
            }
            yeuCauVeLai();
            break;
        }
        case NutXuatMaTran:
            xuatMaTran();
            break;
    }
}

LRESULT CALLBACK xuLySuKienCuaSo(HWND hwnd, UINT thongBao, WPARAM wp, LPARAM lp) {
    switch (thongBao) {
        case WM_SIZE:
            if (ungDung.khungCanh) {
                capNhatKichThuoc();
            }
            return 0;
        case WM_GETMINMAXINFO:
            reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize = {1180, 890};
            return 0;
        case WM_NOTIFY:
            if (reinterpret_cast<NMHDR*>(lp)->idFrom == NhomThe &&
                reinterpret_cast<NMHDR*>(lp)->code == TCN_SELCHANGE) {
                chuyenThe(TabCtrl_GetCurSel(ungDung.nhomThe));
            }
            return 0;
        case WM_COMMAND:
            xuLyLenh(LOWORD(wp));
            return 0;
        case WM_HSCROLL: {
            int id = GetDlgCtrlID(reinterpret_cast<HWND>(lp));
            int giaTri =
                static_cast<int>(SendMessageW(reinterpret_cast<HWND>(lp), TBM_GETPOS, 0, 0));
            if (id == ThanhTocDo) {
                ungDung.tocDo = giaTri / 10.0;
            } else if (id == ThanhBocTach) {
                ungDung.doBocTach = giaTri / 100.0;
            } else if (id == ThanhNguong) {
                ungDung.nguong = giaTri;
                if (ungDung.anhNguon.rgb.empty() && ungDung.diemHatGiong.x >= 0) {
                    chonVungTo(ungDung.diemHatGiong);
                }
            } else if (id == ThanhDungSaiMau) {
                ungDung.dungSaiMau = giaTri;
                if (!ungDung.anhNguon.rgb.empty() && ungDung.diemHatGiong.x >= 0) {
                    chonVungTo(ungDung.diemHatGiong);
                }
            } else if (id >= ThanhGocDe && id <= ThanhGocCoTay) {
                ungDung.dangChay = false;
                ungDung.duongDi.clear();
                ungDung.keTiep = 0;
                ungDung.coVetMucTruoc = false;
                auto ungVien = ungDung.gocKhop;
                double* gocKhop[] = {&ungVien.gocDe, &ungVien.vai, &ungVien.khuyu, &ungVien.coTay};
                *gocKhop[id - ThanhGocDe] = sangRadian(giaTri);
                if (robot::GioiHanKhop{}.chua(ungVien)) {
                    ungDung.gocKhop = ungVien;
                } else {
                    capNhatThanhGocKhop();
                }
                ungDung.dangChon = id - ThanhGocDe;
                baoTrangThai(
                    L"Điều khiển từng khớp. Góc khớp con tương đối giữ nguyên khi thay đổi khớp cha.");
            }
            yeuCauVeLai();
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (ungDung.the == 0 && x >= 80 && x < 280 && y >= 191 && y < 391) {
                chonVungTo({(x - 80) * ungDung.anhNguon.rong / 200,
                            (y - 191) * ungDung.anhNguon.cao / 200});
            }
            return 0;
        }
        case WM_TIMER: {
            auto hienTai = steady_clock::now();
            double khoangThoiGian = duration<double>(hienTai - ungDung.thoiDiemTruoc).count();
            ungDung.thoiDiemTruoc = hienTai;
            if (ungDung.dangChay) {
                capNhatChuyenDong(min(khoangThoiGian, .05));
                yeuCauVeLai();
            }
            return 0;
        }
        case WM_KEYDOWN:
            if (wp == '1') {
                ungDung.gocPhuongVi = -59;
                ungDung.gocNang = 31;
            } else if (wp == '2') {
                ungDung.gocPhuongVi = -90;
                ungDung.gocNang = 88;
            } else if (wp == '3') {
                ungDung.gocPhuongVi = -90;
                ungDung.gocNang = 12;
            } else if (wp == VK_SPACE) {
                xuLyLenh(NutChayTamDung);
            }
            yeuCauVeLai();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT thongTinVe;
            HDC dc = BeginPaint(hwnd, &thongTinVe);
            HDC boDem = CreateCompatibleDC(dc);
            HBITMAP anhBitmap = CreateCompatibleBitmap(dc, ungDung.rong, ungDung.cao);
            auto truocDo = SelectObject(boDem, anhBitmap);
            veBangDieuKhien(boDem);
            BitBlt(dc, 0, 0, ungDung.rong, ungDung.cao, boDem, 0, 0, SRCCOPY);
            SelectObject(boDem, truocDo);
            DeleteObject(anhBitmap);
            DeleteDC(boDem);
            EndPaint(hwnd, &thongTinVe);
            return 0;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN: {
            HDC dc = reinterpret_cast<HDC>(wp);
            SetBkColor(dc, RGB(245, 247, 250));
            SetTextColor(dc, RGB(37, 51, 69));
            static HBRUSH choi = CreateSolidBrush(RGB(245, 247, 250));
            return reinterpret_cast<LRESULT>(choi);
        }
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, thongBao, wp, lp);
}
} // Ket thuc khong gian ten noi bo.

int WINAPI wWinMain(HINSTANCE theHien, HINSTANCE, PWSTR, int cachHienThi) {
    SetProcessDPIAware();
    Gdiplus::GdiplusStartupInput dauVao;
    ULONG_PTR gdiplus{};
    if (Gdiplus::GdiplusStartup(&gdiplus, &dauVao, nullptr) != Gdiplus::Ok) {
        return 1;
    }
    INITCOMMONCONTROLSEX dieuKhienChung{sizeof(dieuKhienChung), ICC_TAB_CLASSES | ICC_BAR_CLASSES};
    InitCommonControlsEx(&dieuKhienChung);
    ungDung.phongChu = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0,
                                   0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    ungDung.phongTieuDe = CreateFontW(-22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                      0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    ungDung.phongDonCach = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Consolas");
    WNDCLASSW lopCuaSoChinh{};
    lopCuaSoChinh.lpfnWndProc = xuLySuKienCuaSo;
    lopCuaSoChinh.hInstance = theHien;
    lopCuaSoChinh.lpszClassName = L"RobotDrawingLab";
    lopCuaSoChinh.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    lopCuaSoChinh.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterClassW(&lopCuaSoChinh);
    WNDCLASSW lopKhungCanh{};
    lopKhungCanh.style = CS_OWNDC;
    lopKhungCanh.lpfnWndProc = xuLySuKienKhungCanh;
    lopKhungCanh.hInstance = theHien;
    lopKhungCanh.lpszClassName = L"RobotOpenGLView";
    lopKhungCanh.hCursor = LoadCursorW(nullptr, IDC_CROSS);
    RegisterClassW(&lopKhungCanh);
    ungDung.cuaSo =
        CreateWindowExW(0, lopCuaSoChinh.lpszClassName, L"Robot Drawing Lab — C++ / OpenGL",
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 1400,
                        940, nullptr, nullptr, theHien, nullptr);
    ungDung.khungCanh =
        CreateWindowExW(0, lopKhungCanh.lpszClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                        rongBangDieuKhien, 62, 1000, 660, ungDung.cuaSo, nullptr, theHien, nullptr);
    ungDung.glDC = GetDC(ungDung.khungCanh);
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    int dinhDang = ChoosePixelFormat(ungDung.glDC, &pfd);
    if (!dinhDang || !SetPixelFormat(ungDung.glDC, dinhDang, &pfd) ||
        !(ungDung.glRC = wglCreateContext(ungDung.glDC))) {
        MessageBoxW(ungDung.cuaSo, L"Không tạo được ngữ cảnh OpenGL.", L"Robot Drawing Lab",
                    MB_ICONERROR);
        return 2;
    }
    wglMakeCurrent(ungDung.glDC, ungDung.glRC);
    ungDung.kyTuTruc = taoKyTuTruc();
    ungDung.khoiTronXoay = gluNewQuadric();
    glGenTextures(1, &ungDung.hoaTietGiay);
    glBindTexture(GL_TEXTURE_2D, ungDung.hoaTietGiay);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    taoBangDieuKhien();
    datAnhNguon(xuLyAnh::taoHoaMau(), L"Ảnh mẫu bông hoa");
    capNhatKichThuoc();
    ShowWindow(ungDung.cuaSo, cachHienThi);
    UpdateWindow(ungDung.cuaSo);
    SetTimer(ungDung.cuaSo, 1, 16, nullptr);
    MSG thongBao{};
    while (GetMessageW(&thongBao, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(ungDung.cuaSo, &thongBao)) {
            TranslateMessage(&thongBao);
            DispatchMessageW(&thongBao);
        }
    }
    gluDeleteQuadric(ungDung.khoiTronXoay);
    glDeleteTextures(1, &ungDung.hoaTietGiay);
    if (ungDung.kyTuTruc) {
        glDeleteLists(ungDung.kyTuTruc, 3);
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(ungDung.glRC);
    ReleaseDC(ungDung.khungCanh, ungDung.glDC);
    DeleteObject(ungDung.phongChu);
    DeleteObject(ungDung.phongTieuDe);
    DeleteObject(ungDung.phongDonCach);
    Gdiplus::GdiplusShutdown(gdiplus);
    return 0;
}
