// Thu duong di va chuyen dong cua ung dung ma khong mo cua so giao dien.
#define wWinMain robotLabWindowMain
#include "../src/main.cpp"
#undef wWinMain

#include <cstdlib>
#include <iostream>

namespace {

using std::all_of;
using std::cerr;
using std::count_if;
using std::cout;
using std::exit;
using std::make_pair;

size_t soPhepKiemTra = 0;

void kiemTraMoPhong(bool dieuKien, const char* ten) {
    ++soPhepKiemTra;
    if (!dieuKien) {
        cerr << "LOI: " << ten << '\n';
        exit(1);
    }
}

void khoiTaoMoPhongThu() {
    ungDung = UngDung{};
    // Dung cua so an de InvalidateRect chi cap nhat cua so thu nghiem.
    ungDung.cuaSo = CreateWindowExW(0, L"STATIC", L"Thu robot", 0, 0, 0, 1, 1, HWND_MESSAGE,
                                    nullptr, GetModuleHandleW(nullptr), nullptr);
    ungDung.khungCanh = ungDung.cuaSo;
    ungDung.anhNguon = xuLyAnh::taoHoaMau();
    veTuTheBanDau();
}

void kiemTraDiemTrenDuong(const vector<DiemQuyDao>& cacDiemQuyDao, robot::Vecto3 truocDo) {
    for (const auto& diemQuyDao : cacDiemQuyDao) {
        const double doDaiDoan = robot::khoangCach(truocDo, diemQuyDao.viTri);
        const int soMau = max(1, static_cast<int>(ceil(doDaiDoan / .06)));
        for (int mauThu = 1; mauThu <= soMau; ++mauThu) {
            const auto dich =
                robot::noiSuyTuyenTinh(truocDo, diemQuyDao.viTri, double(mauThu) / soMau);
            const auto ketQua =
                robot::giaiDongHocNghich(dich, ungDung.nhanhKhuyu, ungDung.kichThuoc);
            kiemTraMoPhong(ketQua.thanhCong(),
                           "cac diem di chuyen va ghi muc deu nam trong gioi han khop");
            kiemTraMoPhong(ketQua.saiSo < 1e-8, "dong hoc thuan va nghich cho dung toa do dich");
            kiemTraMoPhong(tuTheNamTrenBan(ketQua.gocKhop),
                           "cac diem tren duong di giu khoang cach voi mat ban");
        }
        truocDo = diemQuyDao.viTri;
    }
}

void chayDenCuoi() {
    size_t soNhip = 0;
    while (ungDung.dangChay && soNhip++ < 100000) {
        capNhatChuyenDong(.05);
        const auto tuThe = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc);
        kiemTraMoPhong(tuTheNamTrenBan(ungDung.gocKhop), "robot di chuyen phia tren mat ban");
        kiemTraMoPhong(robot::GioiHanKhop{}.chua(ungDung.gocKhop),
                       "cac goc khop luon nam trong gioi han");
        kiemTraMoPhong(robot::huuHan(tuThe.viTriDauBut), "ket qua dong hoc thuan la huu han");
    }
    kiemTraMoPhong(soNhip < 100000, "mo phong ket thuc");
    kiemTraMoPhong(ungDung.keTiep == ungDung.duongDi.size(), "but di qua du cac diem");
}

void thuVeHoaVaDatLai() {
    khoiTaoMoPhongThu();
    const auto batDau = robot::tinhDongHocThuan(ungDung.gocKhop, ungDung.kichThuoc).viTriDauBut;
    veNet();
    kiemTraMoPhong(ungDung.dangChay && ungDung.duongDi.size() > 100,
                   "tao duong ve hoa gom nhieu diem");
    kiemTraDiemTrenDuong(ungDung.duongDi, batDau);
    const auto diemCuoiMongDoi = ungDung.duongDi.back().viTri;
    chayDenCuoi();
    kiemTraMoPhong(robot::khoangCach(robot::tinhDongHocThuan(ungDung.gocKhop).viTriDauBut,
                                     diemCuoiMongDoi) < 1e-8,
                   "vi tri dau but trung diem cuoi duong ve");
    size_t soDiemCoMuc = 0;
    for (auto diemAnh : ungDung.tranh) {
        soDiemCoMuc += diemAnh != 0xffffffff;
    }
    kiemTraMoPhong(soDiemCoMuc > 1000, "muc hien dan khi ve net");

    datLaiDuongVe();
    kiemTraMoPhong(ungDung.keTiep == 0 && ungDung.duongDi.empty() && !ungDung.dangChay &&
                       !ungDung.coVetMucTruoc,
                   "dat lai duong di va trang thai but");
    xuLyLenh(NutChayTamDung);
    kiemTraMoPhong(!ungDung.dangChay, "can lap lai duong di sau khi dat lai");
    veNet();
    kiemTraMoPhong(ungDung.dangChay, "tao lai duong ve sau khi ve dau");
    chayDenCuoi();
    DestroyWindow(ungDung.cuaSo);
}

void thuToTungVungKin() {
    khoiTaoMoPhongThu();
    ungDung.choToDeVien = false;
    veNet();
    chayDenCuoi();
    // Tim cac vung trang khep kin, gom tung canh hoa va nhuy.
    xuLyAnh::MatNa daTham(ungDung.anhNguon.mucXam.size(), 0);
    size_t soVung = 0;
    for (int y = 0; y < ungDung.anhNguon.cao; ++y) {
        for (int x = 0; x < ungDung.anhNguon.rong; ++x) {
            const auto chiSo = static_cast<size_t>(y * ungDung.anhNguon.rong + x);
            if (daTham[chiSo] || ungDung.anhNguon.mucXam[chiSo] < ungDung.nguong) {
                continue;
            }
            auto vung = xuLyAnh::timVungSang(ungDung.anhNguon, {x, y}, ungDung.nguong);
            for (size_t i = 0; i < daTham.size(); ++i) {
                daTham[i] |= vung.matNa[i];
            }
            if (vung.chamMep || vung.soDiemAnh < 5) {
                continue;
            }
            ++soVung;
            veTuTheBanDau();
            ungDung.vungTo = vung;
            ungDung.muc = RGB(180 + soVung, 50 + soVung, 90 + soVung);
            const auto tranhTruocTo = ungDung.tranh;
            const auto batDau = robot::tinhDongHocThuan(ungDung.gocKhop).viTriDauBut;
            toVungDaChon();
            kiemTraMoPhong(ungDung.dangChay && !ungDung.duongDi.empty(),
                           "vung kin tao duoc duong to");
            const uint32_t mauMongDoi = 0xff000000 | GetRValue(ungDung.muc) << 16 |
                                        GetGValue(ungDung.muc) << 8 | GetBValue(ungDung.muc);
            // Doi lua chon tren giao dien khi robot dang chay de kiem tra
            // mau va vung cua luot to hien tai van duoc giu dung.
            ungDung.muc = RGB(3, 4, 5);
            ungDung.vungTo = {};
            kiemTraDiemTrenDuong(ungDung.duongDi, batDau);
            chayDenCuoi();

            size_t daPhu = 0, dangChon = 0, soDiemToTran = 0, soDiemDenBiDoi = 0;
            for (int py = 0; py < kichThuocTranh; ++py) {
                for (int px = 0; px < kichThuocTranh; ++px) {
                    const int ix = static_cast<int>(
                        lround(px * double(ungDung.anhNguon.rong - 1) / (kichThuocTranh - 1)));
                    const int iy = static_cast<int>(
                        lround(py * double(ungDung.anhNguon.cao - 1) / (kichThuocTranh - 1)));
                    const auto chiSoAnh = static_cast<size_t>(iy * ungDung.anhNguon.rong + ix);
                    const auto chiSoTranh = static_cast<size_t>(py * kichThuocTranh + px);
                    const bool daThayDoi = ungDung.tranh[chiSoTranh] != tranhTruocTo[chiSoTranh];
                    soDiemToTran += !vung.matNa[chiSoAnh] && daThayDoi;
                    soDiemDenBiDoi +=
                        ungDung.anhNguon.mucXam[chiSoAnh] < ungDung.nguong && daThayDoi;
                    if (ungDung.matNaVien[chiSoTranh]) {
                        kiemTraMoPhong(!daThayDoi, "giu cac net but khi tat to de vien");
                    } else if (vung.matNa[chiSoAnh]) {
                        ++dangChon;
                        daPhu += ungDung.tranh[chiSoTranh] == mauMongDoi;
                    }
                }
            }
            kiemTraMoPhong(soDiemToTran == 0, "giu nguyen diem ngoai vung chon");
            kiemTraMoPhong(soDiemDenBiDoi == 0, "bao ve net den cua anh nguon");
            kiemTraMoPhong(daPhu > dangChon * .99, "to duoc hon 99 phan tram dien tich co the to");
            cout << "Vung " << soVung << " so diem da to: " << daPhu << '/' << dangChon << '\n';
        }
    }
    kiemTraMoPhong(soVung >= 7, "moi canh hoa va nhuy deu khep kin");
    cout << "So vung kin da thu: " << soVung << '\n';
    DestroyWindow(ungDung.cuaSo);
}

void thuTuChoiDuongDiSai() {
    khoiTaoMoPhongThu();
    const auto truocDo = ungDung.gocKhop;
    thuToaDoDich({7.4, 0.0, caoDoGiay});
    kiemTraMoPhong(!ungDung.dichHopLe, "tu choi dich ngoai tam voi");
    kiemTraMoPhong(robot::khoangCach(robot::tinhDongHocThuan(truocDo).viTriDauBut,
                                     robot::tinhDongHocThuan(ungDung.gocKhop).viTriDauBut) < 1e-8,
                   "giu tu the khi dich khong hop le");
    thuToaDoDich({3.8, .4, caoDoGiay});
    kiemTraMoPhong(ungDung.dichHopLe, "chap nhan dich nam trong tam voi");

    veTuTheBanDau();
    ungDung.nhanhKhuyu = robot::NhanhKhuyu::KhuyuDuoi;
    lapDuongDi(xuLyAnh::tachNetVe(ungDung.anhNguon, ungDung.nguong));
    kiemTraMoPhong(!ungDung.dangChay && ungDung.duongDi.empty(),
                   "tu choi nhanh khuyu co the cham ban");
    DestroyWindow(ungDung.cuaSo);
}

void thuVeAnhSvg() {
    khoiTaoMoPhongThu();
    kiemTraMoPhong(nhapTepAnh(L"examples/la-co.svg"), "mo hinh la co SVG");
    veNet();
    kiemTraMoPhong(ungDung.dangChay && ungDung.cacNet.size() == 2, "la co co hai duong bao rieng");
    const auto& khungCo = ungDung.cacNet[0];
    const auto& ngoiSao = ungDung.cacNet[1];
    kiemTraMoPhong(khungCo.front() == khungCo.back() && ngoiSao.front() == ngoiSao.back(),
                   "hai duong bao la co deu khep kin");
    chayDenCuoi();
    const auto coMucTaiDiemAnh = [](double x, double y) {
        const auto px = lround(x * (kichThuocTranh - 1) / 255);
        const auto py = lround(y * (kichThuocTranh - 1) / 255);
        for (long j = py - 6; j <= py + 6; ++j) {
            for (long i = px - 6; i <= px + 6; ++i) {
                if (ungDung.tranh[j * kichThuocTranh + i] != 0xffffffff) {
                    return true;
                }
            }
        }
        return false;
    };
    for (auto goc : {xuLyAnh::DiemAnh{8, 48}, {248, 48}, {248, 208}, {8, 208}}) {
        kiemTraMoPhong(coMucTaiDiemAnh(goc.x, goc.y), "but di qua bon goc la co");
    }
    const double cacDinh[][2] = {{75, 20},       {81.74, 40.73}, {103.53, 40.73}, {85.9, 53.54},
                                 {92.63, 74.27}, {75, 61.46},    {57.37, 74.27},  {64.1, 53.54},
                                 {46.47, 40.73}, {68.26, 40.73}};
    for (const auto& dinh : cacDinh) {
        kiemTraMoPhong(coMucTaiDiemAnh(8 + 1.6 * dinh[0], 48 + 1.6 * dinh[1]),
                       "but di qua nam dinh ngoai va nam goc lom cua sao");
    }
    kiemTraMoPhong(!coMucTaiDiemAnh(128, 128) && !coMucTaiDiemAnh(55, 70),
                   "sau khi ve vien thi long sao va nen co van trong");
    ungDung.vungTo = xuLyAnh::timVungSang(ungDung.anhNguon, {128, 128}, ungDung.nguong);
    kiemTraMoPhong(ungDung.vungTo.hatGiongHopLe && !ungDung.vungTo.chamMep,
                   "ngoi sao tao thanh vung kin");
    toVungDaChon();
    kiemTraMoPhong(ungDung.dangChay, "bat dau to ngoi sao");
    chayDenCuoi();
    kiemTraMoPhong(coMucTaiDiemAnh(128, 128) && !coMucTaiDiemAnh(55, 70),
                   "to ngoi sao va giu nguyen nen co");
    const auto tranh = ungDung.tranh;
    kiemTraMoPhong(!nhapTepAnh(L"examples/absent.svg") && tranh == ungDung.tranh,
                   "mo SVG loi van giu tranh hien tai");
    kiemTraMoPhong(nhapTepAnh(L"examples/hoa.svg"), "mo hoa SVG gom cac net manh");
    veNet();
    kiemTraMoPhong(ungDung.dangChay && !ungDung.cacNet.empty(), "tao duoc duong ve hoa SVG");
    chayDenCuoi();
    DestroyWindow(ungDung.cuaSo);
}

void thuNgatVaDoiDuongDi() {
    khoiTaoMoPhongThu();
    veNet();
    capNhatChuyenDong(.05);
    kiemTraMoPhong(ungDung.dangChay && !ungDung.duongDi.empty(),
                   "robot dang ve khi nhan lenh ngat");
    thuToaDoDich({3.8, .4, caoDoGiay});
    kiemTraMoPhong(ungDung.dichHopLe && !ungDung.dangChay && ungDung.duongDi.empty() &&
                       ungDung.keTiep == 0 && !ungDung.coVetMucTruoc,
                   "doi dich thi bo duong cu va ngat vet muc");
    xuLyLenh(NutChayTamDung);
    kiemTraMoPhong(!ungDung.dangChay, "khong tiep tuc duong cu sau khi doi dich");

    veTuTheBanDau();
    veNet();
    kiemTraMoPhong(ungDung.dangChay, "bat dau duong di thu hai");
    lapDuongDi({});
    kiemTraMoPhong(!ungDung.dangChay && ungDung.duongDi.empty() && ungDung.keTiep == 0 &&
                       !ungDung.coVetMucTruoc,
                   "lap duong di loi thi dung va bo duong cu");
    xuLyLenh(NutChayTamDung);
    kiemTraMoPhong(!ungDung.dangChay, "khong chay tiep duong di da bi tu choi");

    veNet();
    ungDung.vungTo = {};
    toVungDaChon();
    kiemTraMoPhong(!ungDung.dangChay, "chon hat giong sai thi dung ve");
    veNet();
    ungDung.vungTo = xuLyAnh::timVungSang(ungDung.anhNguon, {0, 0}, ungDung.nguong);
    toVungDaChon();
    kiemTraMoPhong(!ungDung.dangChay, "vung tran ra mep thi dung ve");

    veTuTheBanDau();
    veNet();
    ungDung.nhanhKhuyu = robot::NhanhKhuyu::KhuyuDuoi;
    lapDuongDi(xuLyAnh::tachNetVe(ungDung.anhNguon, ungDung.nguong));
    kiemTraMoPhong(!ungDung.dangChay && ungDung.duongDi.empty(),
                   "nghiem dong hoc nghich khong hop le thi bo duong cu");
    DestroyWindow(ungDung.cuaSo);
}

void thuToMauGocVaTuyChon() {
    khoiTaoMoPhongThu();
    kiemTraMoPhong(nhapTepAnh(L"examples/bong-den.png"), "nap anh PNG bong den nhieu mau");
    const xuLyAnh::DiemAnh thanBong{66, 98}, banhRang{155, 37}, tamBanhRang{157, 82};
    auto chiSo = [&](xuLyAnh::DiemAnh p) { return p.y * ungDung.anhNguon.rong + p.x; };
    kiemTraMoPhong(ungDung.anhXemTruoc[chiSo(thanBong)] == 0xfffdb441u &&
                       ungDung.anhXemTruoc[chiSo(banhRang)] == 0xff433f43u &&
                       ungDung.anhXemTruoc[chiSo(tamBanhRang)] == 0xffe2dee2u,
                   "anh xem truoc giu mau cam, xam dam va tam sang");
    xuLyLenh(NutToMauGoc);
    kiemTraMoPhong(ungDung.dangChay && all_of(ungDung.tranh.begin(), ungDung.tranh.end(),
                                              [](auto p) { return p == 0xffffffffu; }),
                   "bat dau to mau goc tren giay trang");
    while (ungDung.keTiep < ungDung.duongDi.size() / 3 && ungDung.dangChay) {
        capNhatChuyenDong(.05);
    }
    const auto soDiemDaTo = count_if(ungDung.tranh.begin(), ungDung.tranh.end(),
                                     [](auto p) { return p != 0xffffffffu; });
    kiemTraMoPhong(soDiemDaTo > 100 && ungDung.dangChay,
                   "muc xuat hien truoc khi ket thuc luot to");
    xuLyLenh(NutChayTamDung);
    kiemTraMoPhong(!ungDung.dangChay, "tam dung luot to mau goc");
    const auto viTriTamDung = ungDung.keTiep;
    xuLyLenh(NutTienMotBuoc);
    kiemTraMoPhong(!ungDung.dangChay && ungDung.keTiep >= viTriTamDung,
                   "tien mot buoc khi dang tam dung");
    chonVungTo(banhRang);
    kiemTraMoPhong(ungDung.anhXemTruoc[chiSo(thanBong)] == 0xfffdb441u &&
                       ungDung.anhXemTruoc[chiSo(banhRang)] == 0xff433f43u,
                   "vung chon van hien dung mau goc");
    xuLyLenh(NutChayTamDung);
    chayDenCuoi();
    const auto tranhGoc = ungDung.tranh;
    vector<int> chiSoAnh(kichThuocTranh * kichThuocTranh);
    for (int y = 0; y < kichThuocTranh; ++y) {
        for (int x = 0; x < kichThuocTranh; ++x) {
            int ix = static_cast<int>(
                lround(x * double(ungDung.anhNguon.rong - 1) / (kichThuocTranh - 1)));
            int iy = static_cast<int>(
                lround(y * double(ungDung.anhNguon.cao - 1) / (kichThuocTranh - 1)));
            int p = y * kichThuocTranh + x;
            chiSoAnh[p] = iy * ungDung.anhNguon.rong + ix;
            kiemTraMoPhong(tranhGoc[p] == ungDung.anhNguon.mauTai(chiSoAnh[p]),
                           "moi diem tren giay khop RGB anh nguon");
        }
    }
    auto mongDoi = tranhGoc;
    for (const auto& [diemHatGiong, mau] :
         {make_pair(thanBong, RGB(37, 157, 209)), make_pair(banhRang, RGB(51, 123, 75))}) {
        chonVungTo(diemHatGiong);
        const auto matNa = ungDung.vungTo.matNa;
        kiemTraMoPhong(ungDung.vungTo.soDiemAnh > 100 && !matNa[chiSo(tamBanhRang)],
                       "chon mot mang mau lien thong");
        ungDung.muc = mau;
        xuLyLenh(NutToVung);
        kiemTraMoPhong(ungDung.dangChay, "bat dau to bang mau rieng");
        ungDung.muc = RGB(181, 12, 21);
        chonVungTo(tamBanhRang);
        chayDenCuoi();
        const auto argb = 0xff000000u | GetRValue(mau) << 16 | GetGValue(mau) << 8 | GetBValue(mau);
        for (size_t p = 0; p < mongDoi.size(); ++p) {
            if (matNa[chiSoAnh[p]]) {
                mongDoi[p] = argb;
            }
        }
        kiemTraMoPhong(ungDung.tranh == mongDoi,
                       "chi doi mau vung chon va giu ket qua cac luot truoc");
    }
    chonVungTo({28, 10});
    kiemTraMoPhong(ungDung.vungTo.hatGiongHopLe && !ungDung.vungTo.matNa[0],
                   "chon nen trang khong lan vao le giay");
    ungDung.muc = RGB(191, 227, 248);
    toVungDaChon();
    chayDenCuoi();
    xuLyLenh(NutToMauGoc);
    chayDenCuoi();
    kiemTraMoPhong(ungDung.tranh == tranhGoc, "to lai khoi phuc mau nguon va nen trang");
    kiemTraMoPhong(nhapTepAnh(L"examples/la-co.svg"), "mo tep SVG mau");
    kiemTraMoPhong(ungDung.anhNguon.mauTai(128 * 256 + 128) == 0xffffff00u &&
                       ungDung.anhNguon.mauTai(120 * 256 + 45) == 0xffda251du,
                   "SVG giu mau vang cua sao va mau do cua nen co");
    xuLyLenh(NutToMauGoc);
    chayDenCuoi();
    kiemTraMoPhong(ungDung.tranh[256 * kichThuocTranh + 256] == 0xffffff00u,
                   "robot to dung mau ngoi sao tren giay");
    DestroyWindow(ungDung.cuaSo);
}

void thuVienVaMauBongDen() {
    khoiTaoMoPhongThu();
    taoBangDieuKhien();
    const HWND oToDeVien = timDieuKhien(OToDeVien);
    kiemTraMoPhong(oToDeVien && SendMessageW(oToDeVien, BM_GETCHECK, 0, 0) == BST_CHECKED &&
                       ungDung.choToDeVien,
                   "mac dinh cho phep to de vien");
    const auto chonToDeVien = [oToDeVien](bool bat) {
        SendMessageW(oToDeVien, BM_SETCHECK, bat ? BST_CHECKED : BST_UNCHECKED, 0);
        xuLyLenh(OToDeVien);
        kiemTraMoPhong(ungDung.choToDeVien == bat,
                       "lua chon tren giao dien dieu khien cach to vien");
    };
    chonToDeVien(false);
    kiemTraMoPhong(nhapTepAnh(L"examples/bong-den.png"), "nap bong den de ve vien");
    xuLyLenh(VeNet);
    kiemTraMoPhong(ungDung.dangChay, "bat dau ve vien tu dong");
    chayDenCuoi();
    const auto tranhVien = ungDung.tranh;
    // Mep trai cua than bong chi gom mau vang tiep giap voi giay trang.
    for (int y = 50; y <= 165; y += 5) {
        int x = 0;
        while (x < 128 && (ungDung.anhNguon.mauTai(y * ungDung.anhNguon.rong + x) & 255u) >= 100) {
            ++x;
        }
        kiemTraMoPhong(x < 128, "tim thay mep vang tren dong mau");
        int cx = lround(x * double(kichThuocTranh - 1) / (ungDung.anhNguon.rong - 1));
        int cy = lround(y * double(kichThuocTranh - 1) / (ungDung.anhNguon.cao - 1));
        bool coMuc = false;
        for (int dx = -4; dx <= 4; ++dx) {
            coMuc |= tranhVien[cy * kichThuocTranh + cx + dx] != 0xffffffffu;
        }
        kiemTraMoPhong(coMuc, "robot ve dung mep sang cua bong den");
    }
    xuLyLenh(NutToMauGoc);
    chayDenCuoi();
    auto chiSoTranh = [](int x, int y) {
        return lround(y * (kichThuocTranh - 1) / 255.0) * kichThuocTranh +
               lround(x * (kichThuocTranh - 1) / 255.0);
    };
    kiemTraMoPhong(ungDung.tranh[chiSoTranh(66, 98)] == 0xfffdb441u,
                   "to mau goc ben trong bong den");
    chonVungTo({66, 98});
    ungDung.muc = RGB(37, 157, 209);
    xuLyLenh(NutToVung);
    chayDenCuoi();
    kiemTraMoPhong(ungDung.tranh[chiSoTranh(66, 98)] == 0xff259dd1u,
                   "to mau tuy chon ben trong bong den");
    for (size_t i = 0; i < tranhVien.size(); ++i) {
        if (tranhVien[i] != 0xffffffffu) {
            kiemTraMoPhong(ungDung.tranh[i] == tranhVien[i],
                           "hai cach to deu bao ve vien khi tat tuy chon to de");
        }
    }
    chonToDeVien(true);
    chonVungTo({66, 98});
    const auto vungChon = ungDung.vungTo.matNa;
    const auto tranhTruoc = ungDung.tranh;
    xuLyLenh(NutToVung);
    chonToDeVien(false);
    chayDenCuoi();
    int soDiemVienBiTo = 0;
    for (int y = 0; y < kichThuocTranh; ++y) {
        for (int x = 0; x < kichThuocTranh; ++x) {
            int i = y * kichThuocTranh + x;
            int ix = lround(x * 255.0 / (kichThuocTranh - 1)),
                iy = lround(y * 255.0 / (kichThuocTranh - 1));
            if (vungChon[iy * 256 + ix] && tranhVien[i] != 0xffffffffu) {
                kiemTraMoPhong(ungDung.tranh[i] == 0xff259dd1u && !ungDung.matNaVien[i],
                               "to de vien trong vung da chon");
                ++soDiemVienBiTo;
            } else if (!vungChon[iy * 256 + ix]) {
                kiemTraMoPhong(ungDung.tranh[i] == tranhTruoc[i],
                               "giu nguyen phan nam ngoai vung to");
            }
        }
    }
    kiemTraMoPhong(soDiemVienBiTo > 0, "vung thu nghiem co giao voi vien bong den");
    chonToDeVien(true);
    xuLyLenh(NutToMauGoc);
    chayDenCuoi();
    for (int y = 0; y < kichThuocTranh; ++y) {
        for (int x = 0; x < kichThuocTranh; ++x) {
            const int ix = lround(x * 255.0 / (kichThuocTranh - 1)),
                      iy = lround(y * 255.0 / (kichThuocTranh - 1));
            kiemTraMoPhong(ungDung.tranh[y * kichThuocTranh + x] ==
                               ungDung.anhNguon.mauTai(iy * 256 + ix),
                           "to toan anh khoi phuc mau goc tren ca vi tri co vien");
        }
    }
    DestroyWindow(ungDung.cuaSo);
}
}

int main() {
    // Kiem tra cach GLU doc 16 so cua ma tran ma canh OpenGL nhan duoc.
    const auto M = robot::MaTran4::quayZ(robot::pi / 2) * robot::MaTran4::tinhTien(0.2, 0.3, 0.4);
    const GLint khung[4] = {0, 0, 200, 200};
    GLdouble x, y, z;
    kiemTraMoPhong(gluProject(0.25, 0, 0.1, M.data(), robot::MaTran4::donVi().data(), khung, &x, &y,
                              &z) == GL_TRUE &&
                       abs(x - 120) < 1e-6 && abs(y - 155) < 1e-6 && abs(z - 0.75) < 1e-6,
                   "OpenGL ve diem cung vi tri voi cong thuc vecto hang");
    Gdiplus::GdiplusStartupInput options;
    ULONG_PTR token{};
    kiemTraMoPhong(Gdiplus::GdiplusStartup(&token, &options, nullptr) == Gdiplus::Ok,
                   "khoi dong bo doc anh");
    thuVeHoaVaDatLai();
    thuToTungVungKin();
    thuVeAnhSvg();
    thuTuChoiDuongDiSai();
    thuNgatVaDoiDuongDi();
    thuToMauGocVaTuyChon();
    thuVienVaMauBongDen();
    Gdiplus::GdiplusShutdown(token);
    cout << "DAT: " << soPhepKiemTra << " phep kiem tra mo phong\n";
}
