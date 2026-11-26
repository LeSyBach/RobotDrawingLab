#include "../src/robot_math.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

using std::abs;
using std::cerr;
using std::cout;
using std::exit;
using std::numeric_limits;
using std::string;

int soPhepThu = 0;

void kiemTra(bool dieuKien, const string& ten) {
    ++soPhepThu;
    if (!dieuKien) {
        cerr << "LOI: " << ten << '\n';
        exit(1);
    }
}

bool ganBang(double a, double b, double epsilon = 1e-8) {
    return abs(a - b) < epsilon;
}

bool ganBang(const robot::Vecto3& a, const robot::Vecto3& b) {
    return robot::khoangCach(a, b) < 1e-8;
}
}

int main() {
    using namespace robot;
    const MaTran4 thuTu({1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16});
    const MaTran4 donVi = MaTran4::donVi();
    for (int hang = 0; hang < 4; ++hang) {
        for (int cot = 0; cot < 4; ++cot) {
            const int giaTri = hang * 4 + cot + 1;
            kiemTra(thuTu.tai(hang, cot) == giaTri && thuTu.data()[hang * 4 + cot] == giaTri,
                    "luu dung bon hang cua ma tran trong slide");
            kiemTra(donVi.tai(hang, cot) == (hang == cot ? 1 : 0), "phan tu cua ma tran don vi");
        }
    }
    kiemTra(ganBang(MaTran4::quayX(pi / 2).bienDoiVecto({0, 1, 0}), {0, 0, 1}),
            "quay quanh X dua truc Y sang truc Z");
    const MaTran4 chieu = MaTran4::phoiCanh(pi / 2, 2, 1, 9);
    kiemTra(ganBang(chieu.bienDoiDiem({2, 1, -1}), {1, 1, -1}) &&
                ganBang(chieu.bienDoiDiem({0, 0, -9}), {0, 0, 1}),
            "phep chieu dua goc mat gan va tam mat xa ve dung toa do chuan");
    const MaTran4 tinhTien = MaTran4::tinhTien(2, -1, 3);
    kiemTra(tinhTien.tai(3, 0) == 2 && tinhTien.tai(3, 1) == -1 && tinhTien.tai(3, 2) == 3 &&
                tinhTien.tai(0, 3) == 0 && tinhTien.tai(1, 3) == 0 && tinhTien.tai(2, 3) == 0,
            "trang 6: tx, ty, tz o hang cuoi");
    kiemTra(ganBang(tinhTien.bienDoiVecto({1, 2, 3}), {1, 2, 3}), "tinh tien giu nguyen huong");
    const MaTran4 quay = MaTran4::quayZ(sangRadian(90.0));
    const MaTran4 bienDoi = quay * tinhTien;
    kiemTra(ganBang(bienDoi.bienDoiDiem({1.0, 0.0, 0.0}), {2.0, 0.0, 3.0}),
            "trang 16: quay truoc, tinh tien sau");
    kiemTra(ganBang((tinhTien * quay).bienDoiDiem({1, 0, 0}), {1, 3, 3}),
            "doi thu tu phep bien doi cho ket qua khac");
    kiemTra(
        ganBang(bienDoi.nghichDaoBienDoiCung().bienDoiDiem(bienDoi.bienDoiDiem({4.0, 5.0, 6.0})),
                {4.0, 5.0, 6.0}),
        "nghich dao dua diem ve vi tri ban dau");
    kiemTra(ganBang(MaTran4::tiLe(2.0, 3.0, 4.0).bienDoiDiem({1.0, 2.0, 3.0}), {2.0, 6.0, 12.0}),
            "co dan theo ba truc");
    kiemTra(ganBang(MaTran4::quayY(-pi / 2).bienDoiDiem({1.0, 0.0, 0.0}), {0.0, 0.0, 1.0}),
            "dau cua goc quay vai");

    GocKhop tayDuoiThang{0.0, 0.0, 0.0, 0.0};
    const KichThuocRobot kichThuoc;
    const double caoDoButDuoiThang = kichThuoc.chieuCaoVai - kichThuoc.chieuDaiBut;
    const auto tuTheDau = tinhDongHocThuan(tayDuoiThang);
    kiemTra(ganBang(tuTheDau.viTriDauBut, {6.4, 0.0, caoDoButDuoiThang}),
            "vi tri but khi cac goc bang khong");
    tayDuoiThang.gocDe = pi / 2;
    const auto tuTheDaXoay = tinhDongHocThuan(tayDuoiThang);
    kiemTra(ganBang(tuTheDaXoay.viTriKhuyu, {0.0, 3.4, 1.2}), "xoay de lam doi vi tri khuyu");
    kiemTra(ganBang(tuTheDaXoay.viTriDauBut, {0.0, 6.4, caoDoButDuoiThang}),
            "xoay de lam doi vi tri but");
    kiemTra(ganBang(khoangCach(tuTheDaXoay.viTriVai, tuTheDaXoay.viTriKhuyu), 3.4),
            "bien doi phan cap giu do dai doan tay");

    for (double x : {2.2, 3.4, 4.8}) {
        for (double y : {-1.4, 0.0, 1.4}) {
            const Vecto3 dich{x, y, 0.22};
            const auto nghiem = giaiDongHocNghich(dich);
            kiemTra(nghiem.thanhCong(), "dich tren giay nam trong tam voi");
            const auto tuThe = tinhDongHocThuan(nghiem.gocKhop);
            kiemTra(ganBang(tuThe.viTriDauBut, dich),
                    "dong hoc thuan va nghich cung tra ve diem tren giay");
            kiemTra(ganBang(tuThe.viTriCoTay.z - dich.z, kichThuoc.chieuDaiBut),
                    "do cao co tay cach giay bang chieu dai but");
            kiemTra(ganBang(tuThe.coTay.bienDoiVecto({0.0, 0.0, -1.0}), {0.0, 0.0, -1.0}),
                    "co tay giu but thang dung");
        }
    }

    const Vecto3 dichTrenCao{4.0, 0.6, 3.0};
    const auto nghiemKhuyuTren = giaiDongHocNghich(dichTrenCao, NhanhKhuyu::KhuyuTren);
    const auto nghiemKhuyuDuoi = giaiDongHocNghich(dichTrenCao, NhanhKhuyu::KhuyuDuoi);
    kiemTra(nghiemKhuyuTren.thanhCong() && nghiemKhuyuDuoi.thanhCong(),
            "du hai nhanh khuyu cho dich tren cao");
    kiemTra(nghiemKhuyuTren.gocKhop.khuyu < 0.0 && nghiemKhuyuDuoi.gocKhop.khuyu > 0.0,
            "hai nhanh khuyu co goc trai dau");
    kiemTra(ganBang(tinhDongHocThuan(nghiemKhuyuTren.gocKhop).viTriDauBut,
                    tinhDongHocThuan(nghiemKhuyuDuoi.gocKhop).viTriDauBut),
            "hai nhanh khuyu den cung mot dich");

    kiemTra(giaiDongHocNghich({10.0, 0.0, 1.0}).trangThai == TrangThaiNghiem::NgoaiTamVoi,
            "dich vuot tam voi toi da");
    kiemTra(giaiDongHocNghich({0.0, 0.0, caoDoButDuoiThang}).trangThai ==
                TrangThaiNghiem::NgoaiTamVoi,
            "dich gan hon tam voi nho nhat cua co tay");
    kiemTra(giaiDongHocNghich({numeric_limits<double>::quiet_NaN(), 0.0, 0.0}).trangThai ==
                TrangThaiNghiem::DauVaoKhongHopLe,
            "tu choi dich khong huu han");
    KichThuocRobot kichThuocSai;
    kichThuocSai.canhTayTren = 0.0;
    kiemTra(giaiDongHocNghich({3.0, 0.0, 1.0}, NhanhKhuyu::KhuyuTren, kichThuocSai).trangThai ==
                TrangThaiNghiem::DauVaoKhongHopLe,
            "tu choi kich thuoc robot khong hop le");
    GioiHanKhop gioiHanHep;
    gioiHanHep.gocDe = {sangRadian(-5.0), sangRadian(5.0)};
    const auto gocDeVuotGioiHan =
        giaiDongHocNghich({3.0, 2.0, 0.22}, NhanhKhuyu::KhuyuTren, {}, gioiHanHep);
    kiemTra(gocDeVuotGioiHan.trangThai == TrangThaiNghiem::VuotGioiHanKhop,
            "phan biet vuot goc khop va vuot tam voi");
    kiemTra(gocDeVuotGioiHan.saiSo < 1e-8, "nghiem vuot goc van bao sai so hinh hoc");
    const auto suyBien = giaiDongHocNghich({6.4, 0.0, caoDoButDuoiThang});
    kiemTra(suyBien.thanhCong() && suyBien.suyBien, "canh tay duoi het duoc danh dau suy bien");

    const GioiHanKhop gioiHanThanhTruot;
    kiemTra(ganBang(sangDo(gioiHanThanhTruot.gocDe.nhoNhat), -170.0) &&
                ganBang(sangDo(gioiHanThanhTruot.gocDe.lonNhat), 170.0),
            "khoang goc de khop voi thanh truot");
    kiemTra(ganBang(sangDo(gioiHanThanhTruot.vai.nhoNhat), -25.0) &&
                ganBang(sangDo(gioiHanThanhTruot.vai.lonNhat), 150.0),
            "khoang goc vai khop voi thanh truot");
    kiemTra(ganBang(sangDo(gioiHanThanhTruot.coTay.nhoNhat), -170.0) &&
                ganBang(sangDo(gioiHanThanhTruot.coTay.lonNhat), 170.0),
            "khoang goc co tay khop voi thanh truot");
    kiemTra(giaiDongHocNghich({-3.0, 0.0, 0.23}).trangThai == TrangThaiNghiem::VuotGioiHanKhop,
            "tu choi goc de 180 do");

    const MaTran4 camera = MaTran4::nhinVe({5.0, 4.0, 3.0}, {1.0, 1.0, 1.0}, {0.0, 0.0, 1.0});
    kiemTra(ganBang(camera.bienDoiDiem({5.0, 4.0, 3.0}), {}), "ma tran nhin dua mat ve goc toa do");
    const Vecto3 centered = camera.bienDoiDiem({1.0, 1.0, 1.0});
    kiemTra(ganBang(centered.x, 0.0) && ganBang(centered.y, 0.0) && centered.z < 0.0,
            "dich nhin nam tren nua truc Z am");

    cout << "DAT: " << soPhepThu << " phep kiem tra hinh hoc robot\n";
}
