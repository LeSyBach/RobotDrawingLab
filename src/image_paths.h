#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

// Goc anh o tren ben trai. Moi phan tu mat na nhan gia tri 0 hoac 1.
namespace xuLyAnh {

using std::abs;
using std::array;
using std::clamp;
using std::cos;
using std::find;
using std::lround;
using std::max;
using std::min;
using std::move;
using std::numeric_limits;
using std::pair;
using std::queue;
using std::reverse;
using std::sin;
using std::size_t;
using std::uint32_t;
using std::uint8_t;
using std::unique;
using std::vector;

struct DiemAnh {
    int x = 0;
    int y = 0;

    bool operator==(const DiemAnh& doiTuongKhac) const {
        return x == doiTuongKhac.x && y == doiTuongKhac.y;
    }

    bool operator!=(const DiemAnh& doiTuongKhac) const {
        return !(*this == doiTuongKhac);
    }
};

using NetVe = vector<DiemAnh>;
using MatNa = vector<uint8_t>;

inline size_t soDiemAnh(int rong, int cao) {
    if (rong <= 0 || cao <= 0) {
        return 0;
    }
    const auto soLuong = static_cast<size_t>(rong) * static_cast<size_t>(cao);
    return soLuong <= static_cast<size_t>(numeric_limits<int>::max()) ? soLuong : 0;
}

struct Anh {
    int rong = 0;
    int cao = 0;
    vector<uint8_t> mucXam;
    vector<uint32_t> rgb{};
    MatNa matNaHienThi{}; // Diem anh co noi dung, loai phan trong suot va le giay.

    bool hopLe() const {
        return soDiemAnh(rong, cao) != 0 && mucXam.size() == soDiemAnh(rong, cao) &&
               (rgb.empty() ||
                (rgb.size() == mucXam.size() && matNaHienThi.size() == mucXam.size()));
    }

    static Anh taoAnhMau(int w, int h) {
        const auto soLuong = soDiemAnh(w, h);
        return {w, h, vector<uint8_t>(soLuong, 255), vector<uint32_t>(soLuong, 0xffffffffu),
                MatNa(soLuong, 0)};
    }

    uint32_t mauTai(size_t chiSo) const {
        if (!rgb.empty()) {
            return rgb[chiSo];
        }
        return 0xff000000u | (mucXam[chiSo] * 0x010101u);
    }

    void ganMauRGBA(size_t chiSo, int kenhDo, int kenhLuc, int kenhLam, int alpha) {
        const auto trenNenTrang = [alpha](int kenh) {
            return (alpha * kenh + (255 - alpha) * 255 + 127) / 255;
        };
        kenhDo = trenNenTrang(kenhDo);
        kenhLuc = trenNenTrang(kenhLuc);
        kenhLam = trenNenTrang(kenhLam);
        rgb[chiSo] = 0xff000000u | (kenhDo << 16) | (kenhLuc << 8) | kenhLam;
        mucXam[chiSo] =
            static_cast<uint8_t>((299 * kenhDo + 587 * kenhLuc + 114 * kenhLam + 500) / 1000);
        matNaHienThi[chiSo] = alpha != 0;
    }
};

inline bool namTrongAnh(int x, int y, int rong, int cao) {
    return x >= 0 && y >= 0 && x < rong && y < cao;
}

inline MatNa tachTheoNguong(const Anh& anh, int nguong = 180) {
    if (!anh.hopLe()) {
        return {};
    }
    MatNa matNa(anh.mucXam.size());
    for (size_t i = 0; i < matNa.size(); ++i) {
        matNa[i] = anh.mucXam[i] < nguong ? 1 : 0;
    }
    return matNa;
}

// Zhang-Suen lam manh net ve thanh xuong rong mot diem anh.
// Them le quanh anh de xu ly duoc ca cac diem sat bien.
inline MatNa lamManhNet(const MatNa& matNa, int rong, int cao) {
    if (soDiemAnh(rong, cao) == 0 || matNa.size() != soDiemAnh(rong, cao)) {
        return {};
    }
    const int buocDong = rong + 2;
    const int caoAnhDem = cao + 2;
    if (soDiemAnh(buocDong, caoAnhDem) == 0) {
        return {};
    }
    MatNa anhDem(soDiemAnh(buocDong, caoAnhDem), 0);
    for (int y = 0; y < cao; ++y) {
        for (int x = 0; x < rong; ++x) {
            anhDem[(y + 1) * buocDong + x + 1] = matNa[y * rong + x] ? 1 : 0;
        }
    }

    vector<int> canXoa;
    bool daThayDoi;
    do {
        daThayDoi = false;
        for (int pha = 0; pha < 2; ++pha) {
            canXoa.clear();
            for (int y = 1; y <= cao; ++y) {
                for (int x = 1; x <= rong; ++x) {
                    const int chiSo = y * buocDong + x;
                    if (!anhDem[chiSo]) {
                        continue;
                    }
                    const array<int, 8> langGieng = {
                        anhDem[chiSo - buocDong], anhDem[chiSo - buocDong + 1],
                        anhDem[chiSo + 1],        anhDem[chiSo + buocDong + 1],
                        anhDem[chiSo + buocDong], anhDem[chiSo + buocDong - 1],
                        anhDem[chiSo - 1],        anhDem[chiSo - buocDong - 1]};
                    int soLuong = 0;
                    int soLanChuyen = 0;
                    for (int i = 0; i < 8; ++i) {
                        soLuong += langGieng[i];
                        soLanChuyen += langGieng[i] == 0 && langGieng[(i + 1) % 8] == 1;
                    }
                    if (soLuong < 2 || soLuong > 6 || soLanChuyen != 1) {
                        continue;
                    }
                    const int tren = langGieng[0];
                    const int phai = langGieng[2];
                    const int duoi = langGieng[4];
                    const int trai = langGieng[6];
                    const bool canGiuGoc = pha == 0
                                               ? (tren && phai && duoi) || (phai && duoi && trai)
                                               : (tren && phai && trai) || (tren && duoi && trai);
                    if (!canGiuGoc) {
                        canXoa.push_back(chiSo);
                    }
                }
            }
            if (!canXoa.empty()) {
                daThayDoi = true;
            }
            for (int chiSo : canXoa) {
                anhDem[chiSo] = 0;
            }
        }
    } while (daThayDoi);

    MatNa xuongNet(matNa.size(), 0);
    for (int y = 0; y < cao; ++y) {
        for (int x = 0; x < rong; ++x) {
            xuongNet[y * rong + x] = anhDem[(y + 1) * buocDong + x + 1];
        }
    }
    // Mot cum 2 x 2 co the mat het diem sau mot luot lam manh.
    // Giu lai mot diem dai dien neu ca cum bien mat.
    MatNa daXet(matNa.size(), 0);
    vector<int> thanhPhan;
    for (int batDau = 0; batDau < static_cast<int>(matNa.size()); ++batDau) {
        if (!matNa[batDau] || daXet[batDau]) {
            continue;
        }
        thanhPhan.clear();
        thanhPhan.push_back(batDau);
        daXet[batDau] = 1;
        bool conDiem = false;
        for (size_t keTiep = 0; keTiep < thanhPhan.size(); ++keTiep) {
            const int chiSo = thanhPhan[keTiep];
            conDiem |= xuongNet[chiSo] != 0;
            const int x = chiSo % rong;
            const int y = chiSo / rong;
            for (int ny = max(0, y - 1); ny <= min(cao - 1, y + 1); ++ny) {
                for (int nx = max(0, x - 1); nx <= min(rong - 1, x + 1); ++nx) {
                    const int oKe = ny * rong + nx;
                    if (matNa[oKe] && !daXet[oKe]) {
                        daXet[oKe] = 1;
                        thanhPhan.push_back(oKe);
                    }
                }
            }
        }
        if (!conDiem) {
            xuongNet[thanhPhan[thanhPhan.size() / 2]] = 1;
        }
    }
    return xuongNet;
}

// Lan theo tung canh cua do thi diem anh. Moi canh duoc di qua mot lan.
// Tach net o cho re nhanh de robot co the nhac but.
inline vector<NetVe> lanTheoTamNet(const MatNa& xuongNet, int rong, int cao) {
    const auto soLuong = soDiemAnh(rong, cao);
    if (!soLuong || xuongNet.size() != soLuong) {
        return {};
    }
    constexpr array<int, 8> dx = {0, 1, 1, 1, 0, -1, -1, -1};
    constexpr array<int, 8> dy = {-1, -1, 0, 1, 1, 1, 0, -1};
    vector<array<int, 8>> langGieng(soLuong);
    vector<uint8_t> bac(soLuong, 0);
    vector<uint8_t> daTham(soLuong, 0);
    for (auto& danhSachKe : langGieng) {
        danhSachKe.fill(-1);
    }

    for (int y = 0; y < cao; ++y) {
        for (int x = 0; x < rong; ++x) {
            const int chiSo = y * rong + x;
            if (!xuongNet[chiSo]) {
                continue;
            }
            for (int huong = 0; huong < 8; ++huong) {
                const int nx = x + dx[huong];
                const int ny = y + dy[huong];
                if (!namTrongAnh(nx, ny, rong, cao) || !xuongNet[ny * rong + nx]) {
                    continue;
                }
                // Neu da co duong ngang hoac doc qua goc nay, bo canh cheo
                // de tranh tao tam giac nho va di de lai cung mot net.
                if (dx[huong] && dy[huong] &&
                    (xuongNet[y * rong + nx] || xuongNet[ny * rong + x])) {
                    continue;
                }
                langGieng[chiSo][huong] = ny * rong + nx;
                ++bac[chiSo];
            }
        }
    }

    vector<NetVe> cacNet;
    const auto lanNet = [&](int batDau, int huongDau) {
        NetVe net;
        net.push_back({batDau % rong, batDau / rong});
        int hienTai = batDau;
        int huong = huongDau;
        while (huong >= 0) {
            const int keTiep = langGieng[hienTai][huong];
            daTham[hienTai] |= static_cast<uint8_t>(1u << huong);
            daTham[keTiep] |= static_cast<uint8_t>(1u << ((huong + 4) % 8));
            net.push_back({keTiep % rong, keTiep / rong});
            hienTai = keTiep;
            if (hienTai == batDau || bac[hienTai] != 2) {
                break;
            }
            huong = -1;
            for (int d = 0; d < 8; ++d) {
                if (langGieng[hienTai][d] >= 0 && !(daTham[hienTai] & (1u << d))) {
                    huong = d;
                    break;
                }
            }
        }
        return net;
    };

    // Xuat phat tu dau mut hoac nut re nhanh.
    for (int chiSo = 0; chiSo < static_cast<int>(soLuong); ++chiSo) {
        if (!xuongNet[chiSo] || bac[chiSo] == 2) {
            continue;
        }
        if (bac[chiSo] == 0) {
            cacNet.push_back({{chiSo % rong, chiSo / rong}});
        }
        for (int huong = 0; huong < 8; ++huong) {
            if (langGieng[chiSo][huong] >= 0 && !(daTham[chiSo] & (1u << huong))) {
                cacNet.push_back(lanNet(chiSo, huong));
            }
        }
    }
    // Cac canh chua tham luc nay thuoc ve duong khep kin.
    for (int chiSo = 0; chiSo < static_cast<int>(soLuong); ++chiSo) {
        for (int huong = 0; huong < 8; ++huong) {
            if (langGieng[chiSo][huong] >= 0 && !(daTham[chiSo] & (1u << huong))) {
                cacNet.push_back(lanNet(chiSo, huong));
            }
        }
    }
    return cacNet;
}

enum class CachLayNet { TuDong, DuongBao, TamNet };

inline bool coMangDac(const MatNa& muc, int w, int h) {
    // Dem so diem muc cach nen it nhat bon buoc ngang hoac doc.
    // Mang dac co nhieu diem sau ben trong hon mot net manh.
    vector<int> khoangCach(muc.size());
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = y * w + x;
            if (muc[i]) {
                khoangCach[i] = 1 + min(x ? khoangCach[i - 1] : 0, y ? khoangCach[i - w] : 0);
            }
        }
    }
    size_t vungToi = 0, soDiemLoi = 0;
    for (int y = h - 1; y >= 0; --y) {
        for (int x = w - 1; x >= 0; --x) {
            int i = y * w + x;
            if (!muc[i]) {
                continue;
            }
            ++vungToi;
            khoangCach[i] = min(khoangCach[i], 1 + min(x + 1 < w ? khoangCach[i + 1] : 0,
                                                       y + 1 < h ? khoangCach[i + w] : 0));
            if (khoangCach[i] >= 4) {
                ++soDiemLoi;
            }
        }
    }
    return vungToi && soDiemLoi * 5 >= vungToi;
}

inline vector<NetVe> lanTheoDuongBao(const MatNa& muc, int w, int h,
                                     const MatNa* vungTiepGiap = nullptr) {
    // Duyet bon canh cua moi diem muc theo chieu kim dong ho.
    // Bo canh chung de phan muc luon o ben phai duong bao.
    const int buocDong = w + 1;
    MatNa cacCanh(soDiemAnh(buocDong, h + 1), 0);
    if (cacCanh.empty() || muc.size() != soDiemAnh(w, h) ||
        (vungTiepGiap && vungTiepGiap->size() != muc.size())) {
        return {};
    }
    constexpr int dx[] = {1, 0, -1, 0}, dy[] = {0, 1, 0, -1};
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = y * w + x, v = y * buocDong + x;
            if (!muc[i]) {
                continue;
            }
            if (y == 0 || !muc[i - w]) {
                cacCanh[v] |= 1u;
            }
            if (x == w - 1 || !muc[i + 1]) {
                cacCanh[v + 1] |= 2u;
            }
            if (y == h - 1 || !muc[i + w]) {
                cacCanh[v + buocDong + 1] |= 4u;
            }
            if (x == 0 || !muc[i - 1]) {
                cacCanh[v + buocDong] |= 8u;
            }
        }
    }
    vector<NetVe> cacNet;
    for (int batDau = 0; batDau < static_cast<int>(cacCanh.size()); ++batDau) {
        while (cacCanh[batDau]) {
            int v = batDau, huongCanh = 0;
            while (!(cacCanh[v] & (1u << huongCanh))) {
                ++huongCanh;
            }
            NetVe duongKhepKin;
            MatNa canhCanVe;
            for (;;) {
                DiemAnh diem{min(v % buocDong, w - 1), min(v / buocDong, h - 1)};
                duongKhepKin.push_back(diem);
                if (vungTiepGiap) {
                    // Diem muc nam ben phai canh dang duyet.
                    constexpr int lechTraiX[] = {0, 0, -1, -1}, lechTraiY[] = {-1, 0, 0, -1};
                    const int x = v % buocDong + lechTraiX[huongCanh],
                              y = v / buocDong + lechTraiY[huongCanh];
                    canhCanVe.push_back(namTrongAnh(x, y, w, h) && (*vungTiepGiap)[y * w + x]);
                }
                cacCanh[v] &= static_cast<uint8_t>(~(1u << huongCanh));
                v += dy[huongCanh] * buocDong + dx[huongCanh];
                if (v == batDau) {
                    break;
                }
                // Hai o chi cham goc van thuoc hai duong bao rieng.
                for (int huongRe : {1, 0, 3}) {
                    int keTiep = (huongCanh + huongRe) % 4;
                    if (cacCanh[v] & (1u << keTiep)) {
                        huongCanh = keTiep;
                        break;
                    }
                }
            }
            const auto choNgat = find(canhCanVe.begin(), canhCanVe.end(), 0);
            if (choNgat == canhCanVe.end()) {
                duongKhepKin.erase(unique(duongKhepKin.begin(), duongKhepKin.end()),
                                   duongKhepKin.end());
                if (duongKhepKin.back() != duongKhepKin.front()) {
                    duongKhepKin.push_back(duongKhepKin.front());
                }
                cacNet.push_back(move(duongKhepKin));
            } else {
                // Bat dau sau cho ngat de cung lien tuc qua diem goc van la mot net.
                size_t canh =
                    (static_cast<size_t>(choNgat - canhCanVe.begin()) + 1) % duongKhepKin.size();
                NetVe cung;
                for (size_t n = 0; n < duongKhepKin.size(); ++n) {
                    const size_t keTiep = (canh + 1) % duongKhepKin.size();
                    if (canhCanVe[canh]) {
                        if (cung.empty()) {
                            cung.push_back(duongKhepKin[canh]);
                        }
                        if (cung.back() != duongKhepKin[keTiep]) {
                            cung.push_back(duongKhepKin[keTiep]);
                        }
                    } else if (!cung.empty()) {
                        cacNet.push_back(move(cung));
                        cung.clear();
                    }
                    canh = keTiep;
                }
            }
        }
    }
    return cacNet;
}

inline vector<NetVe> tachNetVe(const Anh& anh, int nguong = 180,
                               CachLayNet cheDo = CachLayNet::TuDong) {
    if (!anh.hopLe()) {
        return {};
    }
    const auto vungToi = tachTheoNguong(anh, nguong);
    auto muc = vungToi;
    bool coMauSang = false;
    for (size_t i = 0; i < anh.rgb.size(); ++i) {
        if (vungToi[i] || !anh.matNaHienThi[i]) {
            continue;
        }
        const auto rgb = anh.rgb[i];
        // Mau vang sang van co gia tri kenh lam thap nen co the tach khoi nen trang.
        const int kenhNhoNhat = min({(rgb >> 16) & 255u, (rgb >> 8) & 255u, rgb & 255u});
        if (kenhNhoNhat < nguong) {
            muc[i] = 1;
            coMauSang = true;
        }
    }
    if (cheDo == CachLayNet::DuongBao ||
        (cheDo == CachLayNet::TuDong && coMangDac(muc, anh.rong, anh.cao))) {
        auto cacNet = lanTheoDuongBao(muc, anh.rong, anh.cao);
        if (coMauSang) {
            // Lay them bien trong, nhu ranh gioi giua banh rang va than bong den.
            // Bien ngoai da duoc ve o luot truoc.
            auto netBenTrong = lanTheoDuongBao(vungToi, anh.rong, anh.cao, &muc);
            for (auto& net : netBenTrong) {
                cacNet.push_back(move(net));
            }
        }
        return cacNet;
    }
    return lanTheoTamNet(lamManhNet(muc, anh.rong, anh.cao), anh.rong, anh.cao);
}

struct VungTo {
    MatNa matNa;
    size_t soDiemAnh = 0;
    bool chamMep = false;
    bool hatGiongHopLe = false;
};

// Flood Fill duyet bon o ke cua moi diem tren anh goc.
// Ghi lai viec vung chon cham mep de xu ly nen ben ngoai.
template <class DieuKien>
inline VungTo timVungLienThong(const Anh& anh, DiemAnh diemHatGiong, DieuKien chapNhan) {
    VungTo ketQua;
    if (!anh.hopLe()) {
        return ketQua;
    }
    ketQua.matNa.assign(anh.mucXam.size(), 0);
    if (!namTrongAnh(diemHatGiong.x, diemHatGiong.y, anh.rong, anh.cao)) {
        return ketQua;
    }
    const int chiSoHatGiong = diemHatGiong.y * anh.rong + diemHatGiong.x;
    if (!chapNhan(chiSoHatGiong)) {
        return ketQua;
    }
    ketQua.hatGiongHopLe = true;
    queue<DiemAnh> hangDoi;
    hangDoi.push(diemHatGiong);
    ketQua.matNa[chiSoHatGiong] = 1;
    constexpr int dx[] = {1, 0, -1, 0};
    constexpr int dy[] = {0, 1, 0, -1};
    while (!hangDoi.empty()) {
        const DiemAnh diemAnh = hangDoi.front();
        hangDoi.pop();
        ++ketQua.soDiemAnh;
        if (diemAnh.x == 0 || diemAnh.y == 0 || diemAnh.x == anh.rong - 1 ||
            diemAnh.y == anh.cao - 1) {
            ketQua.chamMep = true;
        }
        for (int huong = 0; huong < 4; ++huong) {
            const int x = diemAnh.x + dx[huong];
            const int y = diemAnh.y + dy[huong];
            if (!namTrongAnh(x, y, anh.rong, anh.cao)) {
                continue;
            }
            const int chiSo = y * anh.rong + x;
            if (!ketQua.matNa[chiSo] && chapNhan(chiSo)) {
                ketQua.matNa[chiSo] = 1;
                hangDoi.push({x, y});
            }
        }
    }
    return ketQua;
}

inline VungTo timVungSang(const Anh& anh, DiemAnh diemHatGiong, int nguong = 180) {
    return timVungLienThong(anh, diemHatGiong, [&](int i) { return anh.mucXam[i] >= nguong; });
}

inline VungTo timVungCungMau(const Anh& anh, DiemAnh diemHatGiong, int dungSai) {
    if (!anh.hopLe() || anh.rgb.empty() ||
        !namTrongAnh(diemHatGiong.x, diemHatGiong.y, anh.rong, anh.cao)) {
        return {};
    }
    const auto mauHatGiong = anh.mauTai(diemHatGiong.y * anh.rong + diemHatGiong.x);
    return timVungLienThong(anh, diemHatGiong, [&](int i) {
        if (!anh.matNaHienThi[i]) {
            return false;
        }
        const auto mau = anh.mauTai(i);
        for (int doDichBit : {16, 8, 0}) {
            const int kenh = (mau >> doDichBit) & 255;
            const int giaTriGoc = (mauHatGiong >> doDichBit) & 255;
            if (abs(kenh - giaTriGoc) > dungSai) {
                return false;
            }
        }
        return true;
    });
}

// Moi net to la mot doan ngang lien tuc trong mat na.
// But nhac len khi chuyen giua hai doan roi tren cung mot dong.
// Buoc quet can nho hon hoac bang duong kinh but de phu kin vung.
inline vector<NetVe> lapNetToTheoDong(const MatNa& matNa, int rong, int cao, int buocQuet = 1) {
    if (soDiemAnh(rong, cao) == 0 || matNa.size() != soDiemAnh(rong, cao)) {
        return {};
    }
    buocQuet = max(1, buocQuet);
    vector<NetVe> cacNet;
    bool daoChieu = false;
    int hangDau = 0;
    while (hangDau < cao) {
        bool coDiem = false;
        for (int x = 0; x < rong; ++x) {
            coDiem |= matNa[hangDau * rong + x] != 0;
        }
        if (coDiem) {
            break;
        }
        ++hangDau;
    }
    for (int y = hangDau; y < cao; y += buocQuet) {
        vector<pair<int, int>> cacDoan;
        for (int x = 0; x < rong;) {
            if (!matNa[y * rong + x]) {
                ++x;
                continue;
            }
            const int trai = x;
            while (x + 1 < rong && matNa[y * rong + x + 1]) {
                ++x;
            }
            cacDoan.push_back({trai, x});
            ++x;
        }
        if (daoChieu) {
            reverse(cacDoan.begin(), cacDoan.end());
        }
        for (const auto& doan : cacDoan) {
            NetVe net;
            net.reserve(doan.second - doan.first + 1);
            if (daoChieu) {
                for (int x = doan.second; x >= doan.first; --x) {
                    net.push_back({x, y});
                }
            } else {
                for (int x = doan.first; x <= doan.second; ++x) {
                    net.push_back({x, y});
                }
            }
            cacNet.push_back(move(net));
        }
        if (!cacDoan.empty()) {
            daoChieu = !daoChieu;
        }
    }
    return cacNet;
}

inline void veDia(Anh& anh, DiemAnh tam, int banKinh = 0, uint8_t mucXam = 0) {
    if (!anh.hopLe()) {
        return;
    }
    banKinh = max(0, banKinh);
    for (int y = max(0, tam.y - banKinh); y <= min(anh.cao - 1, tam.y + banKinh); ++y) {
        for (int x = max(0, tam.x - banKinh); x <= min(anh.rong - 1, tam.x + banKinh); ++x) {
            const long long dx = static_cast<long long>(x) - tam.x;
            const long long dy = static_cast<long long>(y) - tam.y;
            if (dx * dx + dy * dy <= static_cast<long long>(banKinh) * banKinh) {
                anh.mucXam[y * anh.rong + x] = mucXam;
            }
        }
    }
}

// Dung Bresenham de ve ca cac doan thang cua hoa mau.
inline void veDoanThang(Anh& anh, DiemAnh tuDiem, DiemAnh denDiem, int banKinh = 0,
                        uint8_t mucXam = 0) {
    if (!anh.hopLe()) {
        return;
    }
    int dx = abs(denDiem.x - tuDiem.x);
    int dy = -abs(denDiem.y - tuDiem.y);
    int buocX = tuDiem.x < denDiem.x ? 1 : -1;
    int buocY = tuDiem.y < denDiem.y ? 1 : -1;
    int saiSo = dx + dy;
    for (;;) {
        veDia(anh, tuDiem, banKinh, mucXam);
        if (tuDiem == denDiem) {
            break;
        }
        const int haiLanSaiSo = 2 * saiSo;
        if (haiLanSaiSo >= dy) {
            saiSo += dy;
            tuDiem.x += buocX;
        }
        if (haiLanSaiSo <= dx) {
            saiSo += dx;
            tuDiem.y += buocY;
        }
    }
}

inline void veDuongTron(Anh& anh, DiemAnh tam, int banKinh, int banKinhBut = 0,
                        uint8_t mucXam = 0) {
    if (!anh.hopLe() || banKinh < 0) {
        return;
    }
    int x = banKinh;
    int y = 0;
    int bienQuyetDinh = 1 - banKinh;
    while (x >= y) {
        for (DiemAnh doLech : {DiemAnh{x, y}, DiemAnh{y, x}, DiemAnh{-y, x}, DiemAnh{-x, y},
                               DiemAnh{-x, -y}, DiemAnh{-y, -x}, DiemAnh{y, -x}, DiemAnh{x, -y}}) {
            veDia(anh, {tam.x + doLech.x, tam.y + doLech.y}, banKinhBut, mucXam);
        }
        ++y;
        if (bienQuyetDinh < 0) {
            bienQuyetDinh += 2 * y + 1;
        } else {
            --x;
            bienQuyetDinh += 2 * (y - x) + 1;
        }
    }
}

// Anh mau co sau canh va nhuy khep kin. Moi lan tao deu cho cung mot anh
// de co the kiem tra lai cung thuat toan tach net nhu voi anh nhap.
inline Anh taoHoaMau(int kichCo = 256) {
    kichCo = clamp(kichCo, 64, 1024);
    Anh anh{kichCo, kichCo, vector<uint8_t>(soDiemAnh(kichCo, kichCo), 255)};
    const double pi = 3.14159265358979323846;
    const double cx = kichCo * 0.5;
    const double cy = kichCo * 0.43;
    const double banKinhTam = kichCo * 0.075;
    const auto diem = [](double x, double y) {
        return DiemAnh{static_cast<int>(lround(x)), static_cast<int>(lround(y))};
    };
    const int banKinhBut = max(0, kichCo / 256 - 1);
    for (int canhHoa = 0; canhHoa < 6; ++canhHoa) {
        const double goc = -pi / 2.0 + canhHoa * pi / 3.0;
        const double ux = cos(goc);
        const double uy = sin(goc);
        const double vx = -uy;
        const double vy = ux;
        const double doDai = kichCo * 0.245;
        const double nuaChieuRong = kichCo * 0.090;
        const double benTrong = banKinhTam * 1.17;
        DiemAnh truocDo;
        for (int buoc = 0; buoc <= 240; ++buoc) {
            const double t = 2.0 * pi * buoc / 240.0;
            const double banKinhNgang = benTrong + doDai * (1.0 - cos(t)) / 2.0;
            const double lechNgang = nuaChieuRong * sin(t);
            const DiemAnh hienTai = diem(cx + ux * banKinhNgang + vx * lechNgang,
                                         cy + uy * banKinhNgang + vy * lechNgang);
            if (buoc != 0) {
                veDoanThang(anh, truocDo, hienTai, banKinhBut);
            }
            truocDo = hienTai;
        }
    }
    veDuongTron(anh, diem(cx, cy), static_cast<int>(lround(banKinhTam)), banKinhBut);
    const DiemAnh dauCuong = diem(cx, cy + kichCo * 0.335);
    const DiemAnh cuoiCuong = diem(cx, kichCo * 0.94);
    veDoanThang(anh, dauCuong, cuoiCuong, banKinhBut);
    const DiemAnh gocLa = diem(cx, kichCo * 0.84);
    const DiemAnh dinhLa = diem(cx + kichCo * 0.17, kichCo * 0.73);
    const DiemAnh mepLa = diem(cx + kichCo * 0.10, kichCo * 0.85);
    veDoanThang(anh, gocLa, dinhLa, banKinhBut);
    veDoanThang(anh, dinhLa, mepLa, banKinhBut);
    veDoanThang(anh, mepLa, gocLa, banKinhBut);
    return anh;
}

} // Khong gian ten xuLyAnh.
