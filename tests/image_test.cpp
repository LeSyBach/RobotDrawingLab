#include "../src/image_paths.h"

#include <cassert>
#include <iostream>
#include <set>

using namespace xuLyAnh;

using std::abs;
using std::cout;
using std::max;
using std::min;
using std::minmax;
using std::pair;
using std::set;
using std::size_t;
using std::uint8_t;
using std::vector;

static Anh anhTrang(int rong, int cao) {
    return {rong, cao, vector<uint8_t>(rong * cao, 255)};
}

static void thuToVungKin() {
    Anh anh = anhTrang(11, 11);
    veDoanThang(anh, {2, 2}, {8, 2});
    veDoanThang(anh, {8, 2}, {8, 8});
    veDoanThang(anh, {8, 8}, {2, 8});
    veDoanThang(anh, {2, 8}, {2, 2});
    const auto vungTo = timVungSang(anh, {5, 5});
    assert(vungTo.hatGiongHopLe && !vungTo.chamMep && vungTo.soDiemAnh == 25);
    const auto cacDoan = lapNetToTheoDong(vungTo.matNa, anh.rong, anh.cao);
    assert(cacDoan.size() == 5);
    assert(cacDoan[0].front() == (DiemAnh{3, 3}));
    assert(cacDoan[1].front() == (DiemAnh{7, 4}));
    size_t drawn = 0;
    for (const auto& doan : cacDoan) {
        for (const auto diemAnh : doan) {
            assert(vungTo.matNa[diemAnh.y * anh.rong + diemAnh.x] == 1);
            ++drawn;
        }
    }
    assert(drawn == vungTo.soDiemAnh);
    assert(!timVungSang(anh, {2, 2}).hatGiongHopLe);
    assert(!timVungSang(anh, {-1, 5}).hatGiongHopLe);
}

static void thuVungChamBien() {
    Anh anh = anhTrang(11, 11);
    veDoanThang(anh, {2, 2}, {8, 2});
    veDoanThang(anh, {8, 2}, {8, 8});
    veDoanThang(anh, {8, 8}, {2, 8});
    veDoanThang(anh, {2, 8}, {2, 2});
    anh.mucXam[2 * anh.rong + 5] = 255;
    const auto vungTo = timVungSang(anh, {5, 5});
    assert(vungTo.hatGiongHopLe && vungTo.chamMep);
    assert(vungTo.soDiemAnh == 98);
}

static void kiemTraLienThong(const vector<NetVe>& cacNet) {
    for (const auto& net : cacNet) {
        assert(!net.empty());
        for (size_t i = 1; i < net.size(); ++i) {
            const int dx = abs(net[i].x - net[i - 1].x);
            const int dy = abs(net[i].y - net[i - 1].y);
            assert(dx <= 1 && dy <= 1 && dx + dy != 0);
        }
    }
}

static void thuNetRoiVaReNhanh() {
    Anh anh = anhTrang(30, 30);
    veDoanThang(anh, {2, 2}, {9, 2});
    veDoanThang(anh, {20, 20}, {25, 25});
    const auto cacNet = tachNetVe(anh);
    assert(cacNet.size() == 2);
    kiemTraLienThong(cacNet);
    for (const auto& net : cacNet) {
        const bool thuocCumDau = net.front().x < 10;
        for (const auto diemAnh : net) {
            assert((diemAnh.x < 10) == thuocCumDau);
        }
    }

    Anh tichCoHuong = anhTrang(11, 11);
    veDoanThang(tichCoHuong, {5, 1}, {5, 9});
    veDoanThang(tichCoHuong, {1, 5}, {9, 5});
    const auto cacNetChuThap = tachNetVe(tichCoHuong);
    assert(cacNetChuThap.size() == 4);
    kiemTraLienThong(cacNetChuThap);
    size_t cacCanh = 0;
    for (const auto& net : cacNetChuThap) {
        cacCanh += net.size() - 1;
    }
    assert(cacCanh == 16);
}

static void thuNetDayVaVongKin() {
    Anh anh = anhTrang(40, 40);
    veDoanThang(anh, {5, 20}, {35, 20}, 3);
    const auto xuongNet = lamManhNet(tachTheoNguong(anh), anh.rong, anh.cao);
    size_t soLuong = 0;
    for (auto foreground : xuongNet) {
        soLuong += foreground;
    }
    assert(soLuong > 20 && soLuong < 40);
    const auto cacNet = lanTheoTamNet(xuongNet, anh.rong, anh.cao);
    assert(cacNet.size() == 1);
    kiemTraLienThong(cacNet);

    Anh circle = anhTrang(40, 40);
    veDuongTron(circle, {20, 20}, 12);
    const auto cacNetTron = tachNetVe(circle);
    assert(cacNetTron.size() == 1);
    assert(cacNetTron[0].front() == cacNetTron[0].back());
    kiemTraLienThong(cacNetTron);
}

static void thuDoanToRoiVaDiemDon() {
    MatNa matNa(12 * 5, 0);
    for (int y = 1; y <= 3; ++y) {
        for (int x : {1, 2, 3, 8, 9, 10}) {
            matNa[y * 12 + x] = 1;
        }
    }
    const auto cacDoan = lapNetToTheoDong(matNa, 12, 5);
    assert(cacDoan.size() == 6);
    for (const auto& doan : cacDoan) {
        assert(doan.size() == 3);
        for (auto diemAnh : doan) {
            assert(matNa[diemAnh.y * 12 + diemAnh.x]);
        }
    }
    MatNa single(25, 0);
    single[12] = 1;
    const auto singleton = lanTheoTamNet(single, 5, 5);
    assert(singleton.size() == 1 && singleton[0].size() == 1);
}

static void thuLanDuCacCanh() {
    // Thu tat ca cach sap xep diem trong o 3 x 3. Giu canh cheo neu
    // do la cau noi duy nhat. Moi dinh phai duoc tham va moi canh
    // chi duoc ve mot lan.
    for (int arrangement = 1; arrangement < (1 << 9); ++arrangement) {
        MatNa matNa(9, 0);
        for (int i = 0; i < 9; ++i) {
            matNa[i] = (arrangement >> i) & 1;
        }
        set<pair<int, int>> canhMongDoi;
        for (int chiSo = 0; chiSo < 9; ++chiSo) {
            if (!matNa[chiSo]) {
                continue;
            }
            const int x = chiSo % 3;
            const int y = chiSo / 3;
            for (int ny = max(0, y - 1); ny <= min(2, y + 1); ++ny) {
                for (int nx = max(0, x - 1); nx <= min(2, x + 1); ++nx) {
                    const int oKe = ny * 3 + nx;
                    if (oKe <= chiSo || !matNa[oKe]) {
                        continue;
                    }
                    if (nx != x && ny != y && (matNa[y * 3 + nx] || matNa[ny * 3 + x])) {
                        continue;
                    }
                    canhMongDoi.insert({chiSo, oKe});
                }
            }
        }
        set<pair<int, int>> canhThucTe;
        set<int> cacDinhDaTham;
        const auto cacNet = lanTheoTamNet(matNa, 3, 3);
        kiemTraLienThong(cacNet);
        for (const auto& net : cacNet) {
            for (size_t i = 0; i < net.size(); ++i) {
                const int chiSo = net[i].y * 3 + net[i].x;
                assert(matNa[chiSo]);
                cacDinhDaTham.insert(chiSo);
                if (i == 0) {
                    continue;
                }
                const int truocDo = net[i - 1].y * 3 + net[i - 1].x;
                const auto canh = minmax(truocDo, chiSo);
                assert(canhThucTe.insert({canh.first, canh.second}).second);
            }
        }
        assert(canhThucTe == canhMongDoi);
        for (int i = 0; i < 9; ++i) {
            assert((cacDinhDaTham.count(i) != 0) == (matNa[i] != 0));
        }
    }
}

static void thuNguongVaChamNho() {
    Anh levels{3, 1, {149, 150, 151}};
    const auto foreground = tachTheoNguong(levels, 150);
    assert(foreground[0] && !foreground[1] && !foreground[2]);
    assert(!timVungSang(levels, {0, 0}, 150).hatGiongHopLe);
    assert(timVungSang(levels, {1, 0}, 150).soDiemAnh == 2);

    Anh dots = anhTrang(12, 8);
    dots.mucXam[1 * 12 + 1] = 0;
    dots.mucXam[1 * 12 + 2] = 0;
    dots.mucXam[2 * 12 + 1] = 0;
    dots.mucXam[2 * 12 + 2] = 0;
    dots.mucXam[5 * 12 + 9] = 0;
    const auto cacNet = tachNetVe(dots);
    assert(cacNet.size() == 2);
    kiemTraLienThong(cacNet);
}

static void thuAnhMauVaDauVaoLoi() {
    const auto flower = taoHoaMau();
    assert(flower.hopLe());
    const auto cacNet = tachNetVe(flower);
    assert(cacNet.size() >= 8 && cacNet.size() < 60);
    kiemTraLienThong(cacNet);
    const auto tam = timVungSang(flower, {128, 110});
    assert(tam.hatGiongHopLe && !tam.chamMep && tam.soDiemAnh > 500);
    const auto canhHoa = timVungSang(flower, {128, 60});
    assert(canhHoa.hatGiongHopLe && !canhHoa.chamMep && canhHoa.soDiemAnh > 500);
    assert(tachNetVe({}).empty());
    assert(lamManhNet({}, 0, 0).empty());
    assert(lanTheoTamNet({}, 2, 2).empty());
    assert(lapNetToTheoDong({}, 2, 2).empty());
    assert(!timVungSang({}, {0, 0}).hatGiongHopLe);
}

static void thuChonVungMauLienThong() {
    auto anh = Anh::taoAnhMau(6, 1);
    anh.ganMauRGBA(0, 255, 0, 0, 255);
    anh.ganMauRGBA(1, 253, 2, 1, 255);
    anh.ganMauRGBA(2, 0, 130, 0, 255);
    anh.ganMauRGBA(3, 255, 0, 0, 255);
    anh.ganMauRGBA(4, 0, 0, 255, 128);
    anh.ganMauRGBA(5, 42, 42, 42, 0);
    assert(anh.mucXam[0] == anh.mucXam[2]);
    assert(timVungCungMau(anh, {0, 0}, 0).soDiemAnh == 1);
    auto kenhDo = timVungCungMau(anh, {0, 0}, 8);
    assert(kenhDo.soDiemAnh == 2 && !kenhDo.matNa[2] && !kenhDo.matNa[3]);
    assert(anh.mauTai(4) == 0xff7f7fffu);
    assert(anh.mauTai(5) == 0xffffffffu && !timVungCungMau(anh, {5, 0}, 100).hatGiongHopLe);
}

int main() {
    thuToVungKin();
    thuVungChamBien();
    thuNetRoiVaReNhanh();
    thuNetDayVaVongKin();
    thuDoanToRoiVaDiemDon();
    thuLanDuCacCanh();
    thuNguongVaChamNho();
    thuAnhMauVaDauVaoLoi();
    thuChonVungMauLienThong();
    cout << "DAT: kiem tra tach net, to vung, lam manh va anh mau.\n";
}
