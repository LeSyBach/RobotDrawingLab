#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>

namespace robot {

using std::abs;
using std::acos;
using std::array;
using std::atan2;
using std::clamp;
using std::cos;
using std::hypot;
using std::isfinite;
using std::numeric_limits;
using std::sin;
using std::size_t;
using std::sqrt;
using std::string;
using std::tan;

constexpr double pi = 3.14159265358979323846;

constexpr double sangRadian(double giaTriDo) {
    return giaTriDo * pi / 180.0;
}

constexpr double sangDo(double giaTriRadian) {
    return giaTriRadian * 180.0 / pi;
}

struct Vecto3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vecto3 operator+(const Vecto3& doiTuongKhac) const {
        return {x + doiTuongKhac.x, y + doiTuongKhac.y, z + doiTuongKhac.z};
    }

    Vecto3 operator-(const Vecto3& doiTuongKhac) const {
        return {x - doiTuongKhac.x, y - doiTuongKhac.y, z - doiTuongKhac.z};
    }

    Vecto3 operator-() const {
        return {-x, -y, -z};
    }

    Vecto3 operator*(double heSo) const {
        return {x * heSo, y * heSo, z * heSo};
    }

    Vecto3 operator/(double heSo) const {
        return {x / heSo, y / heSo, z / heSo};
    }

    Vecto3& operator+=(const Vecto3& doiTuongKhac) {
        x += doiTuongKhac.x;
        y += doiTuongKhac.y;
        z += doiTuongKhac.z;
        return *this;
    }
};

inline Vecto3 operator*(double heSo, const Vecto3& vecto) {
    return vecto * heSo;
}

inline double tichVoHuong(const Vecto3& a, const Vecto3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vecto3 tichCoHuong(const Vecto3& a, const Vecto3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline double doDai(const Vecto3& vecto) {
    return sqrt(tichVoHuong(vecto, vecto));
}

inline Vecto3 chuanHoa(const Vecto3& vecto) {
    const double doLon = doDai(vecto);
    return doLon > 1e-12 ? vecto / doLon : Vecto3{};
}

inline double khoangCach(const Vecto3& a, const Vecto3& b) {
    return doDai(a - b);
}

inline bool huuHan(const Vecto3& vecto) {
    return isfinite(vecto.x) && isfinite(vecto.y) && isfinite(vecto.z);
}

inline Vecto3 noiSuyTuyenTinh(const Vecto3& a, const Vecto3& b, double t) {
    return a + (b - a) * t;
}

// Chuong 4: Q = P * M. Voi P * A * B, diem P di qua A truoc, roi den B.
struct MaTran4 {
    array<double, 16> m{};

    // Moi hang luu lien nhau. OpenGL doc cung 16 so theo cot cho phep bien doi tuong ung.
    const double* data() const {
        return m.data();
    }

    double* data() {
        return m.data();
    }

    double& tai(int hang, int cot) {
        return m[static_cast<size_t>(hang * 4 + cot)];
    }

    double tai(int hang, int cot) const {
        return m[static_cast<size_t>(hang * 4 + cot)];
    }

    using Hang = array<double, 4>;

    MaTran4() = default;

    // Moi dong trong loi goi ham khoi tao la mot hang cua ma tran.
    MaTran4(const Hang& hang1, const Hang& hang2, const Hang& hang3, const Hang& hang4) {
        for (int cot = 0; cot < 4; ++cot) {
            tai(0, cot) = hang1[cot];
            tai(1, cot) = hang2[cot];
            tai(2, cot) = hang3[cot];
            tai(3, cot) = hang4[cot];
        }
    }

    // clang-format off
    static MaTran4 donVi() {
        return MaTran4({1, 0, 0, 0},
                       {0, 1, 0, 0},
                       {0, 0, 1, 0},
                       {0, 0, 0, 1});
    }

    // Chuong 4, trang 5-6: ba do doi nam o hang cuoi.
    static MaTran4 tinhTien(double tx, double ty, double tz) {
        const MaTran4 M({ 1,  0,  0, 0},
                        { 0,  1,  0, 0},
                        { 0,  0,  1, 0},
                        {tx, ty, tz, 1});
        return M;
    }

    static MaTran4 tinhTien(const Vecto3& vecto) {
        return tinhTien(vecto.x, vecto.y, vecto.z);
    }

    // Chuong 4, trang 9: sx, sy, sz la he so ti le theo cac truc X, Y, Z.
    static MaTran4 tiLe(double sx, double sy, double sz) {
        const MaTran4 M({sx,  0,  0, 0},
                        { 0, sy,  0, 0},
                        { 0,  0, sz, 0},
                        { 0,  0,  0, 1});
        return M;
    }

    // Chuong 4, trang 12: alpha la goc quay tinh bang radian.
    static MaTran4 quayX(double alpha) {
        const MaTran4 Rx({1,           0,          0, 0},
                         {0,  cos(alpha), sin(alpha), 0},
                         {0, -sin(alpha), cos(alpha), 0},
                         {0,           0,          0, 1});
        return Rx;
    }

    static MaTran4 quayY(double alpha) {
        const MaTran4 Ry({cos(alpha), 0, -sin(alpha), 0},
                         {         0, 1,           0, 0},
                         {sin(alpha), 0,  cos(alpha), 0},
                         {         0, 0,           0, 1});
        return Ry;
    }

    static MaTran4 quayZ(double alpha) {
        const MaTran4 Rz({ cos(alpha), sin(alpha), 0, 0},
                         {-sin(alpha), cos(alpha), 0, 0},
                         {          0,          0, 1, 0},
                         {          0,          0, 0, 1});
        return Rz;
    }
    // clang-format on

    // Moi o ket qua la tich vo huong cua mot hang ben trai voi mot cot ben phai.
    MaTran4 operator*(const MaTran4& doiTuongKhac) const {
        MaTran4 ketQua;
        for (int hang = 0; hang < 4; ++hang) {
            for (int cot = 0; cot < 4; ++cot) {
                for (int k = 0; k < 4; ++k) {
                    ketQua.tai(hang, cot) += tai(hang, k) * doiTuongKhac.tai(k, cot);
                }
            }
        }
        return ketQua;
    }

    // Diem P = (x, y, z, 1). Moi thanh phan cua Q bang P nhan voi mot cot cua M.
    Vecto3 bienDoiDiem(const Vecto3& diem) const {
        const double w = diem.x * tai(0, 3) + diem.y * tai(1, 3) + diem.z * tai(2, 3) + tai(3, 3);
        const Vecto3 ketQua{
            diem.x * tai(0, 0) + diem.y * tai(1, 0) + diem.z * tai(2, 0) + tai(3, 0),
            diem.x * tai(0, 1) + diem.y * tai(1, 1) + diem.z * tai(2, 1) + tai(3, 1),
            diem.x * tai(0, 2) + diem.y * tai(1, 2) + diem.z * tai(2, 2) + tai(3, 2)};
        return abs(w) > 1e-12 ? ketQua / w : ketQua;
    }

    Vecto3 bienDoiVecto(const Vecto3& vecto) const {
        return {vecto.x * tai(0, 0) + vecto.y * tai(1, 0) + vecto.z * tai(2, 0),
                vecto.x * tai(0, 1) + vecto.y * tai(1, 1) + vecto.z * tai(2, 1),
                vecto.x * tai(0, 2) + vecto.y * tai(1, 2) + vecto.z * tai(2, 2)};
    }

    // Chi dung cho bien doi cung. Khoi quay 3 x 3 phai truc chuan.
    MaTran4 nghichDaoBienDoiCung() const {
        const Vecto3 trucX{tai(0, 0), tai(0, 1), tai(0, 2)};
        const Vecto3 trucY{tai(1, 0), tai(1, 1), tai(1, 2)};
        const Vecto3 trucZ{tai(2, 0), tai(2, 1), tai(2, 2)};
        const Vecto3 viTri{tai(3, 0), tai(3, 1), tai(3, 2)};
        const double tx = -tichVoHuong(viTri, trucX);
        const double ty = -tichVoHuong(viTri, trucY);
        const double tz = -tichVoHuong(viTri, trucZ);
        // clang-format off
        return MaTran4({trucX.x, trucY.x, trucZ.x, 0},
                       {trucX.y, trucY.y, trucZ.y, 0},
                       {trucX.z, trucY.z, trucZ.z, 0},
                       {     tx,      ty,      tz, 1});
        // clang-format on
    }

    static MaTran4 phoiCanh(double gocNhinDoc, double tiLeKhung, double matGan, double matXa) {
        if (tiLeKhung <= 0.0 || matGan <= 0.0 || matXa <= matGan || gocNhinDoc <= 0.0 ||
            gocNhinDoc >= pi) {
            return donVi();
        }
        const double tieuCu = 1.0 / tan(gocNhinDoc / 2.0);
        const double heSoZ = (matXa + matGan) / (matGan - matXa);
        const double doDoiZ = 2.0 * matXa * matGan / (matGan - matXa);
        // clang-format off
        return MaTran4({tieuCu / tiLeKhung,      0,     0,      0},
                       {                 0, tieuCu,     0,      0},
                       {                 0,      0,  heSoZ, -1},
                       {                 0,      0, doDoiZ,  0});
        // clang-format on
    }

    static MaTran4 nhinVe(const Vecto3& matNhin, const Vecto3& dich, const Vecto3& huongLen) {
        const Vecto3 huongNhin = chuanHoa(dich - matNhin);
        const Vecto3 phai = chuanHoa(tichCoHuong(huongNhin, huongLen));
        const Vecto3 len = tichCoHuong(phai, huongNhin);
        const Vecto3 sau = -huongNhin;
        const double tx = -tichVoHuong(phai, matNhin);
        const double ty = -tichVoHuong(len, matNhin);
        const double tz = -tichVoHuong(sau, matNhin);
        // clang-format off
        return MaTran4({phai.x, len.x, sau.x, 0},
                       {phai.y, len.y, sau.y, 0},
                       {phai.z, len.z, sau.z, 0},
                       {    tx,    ty,    tz, 1});
        // clang-format on
    }
};

struct KichThuocRobot {
    double chieuCaoVai = 1.2;
    double canhTayTren = 3.4;
    double cangTay = 3.0;
    double chieuDaiBut = 1.45; // Khoang cach tu tam co tay den dau ngoi but.
};

// Cac gia tri la goc khop tinh bang radian tai tu the hien tai.
struct GocKhop {
    double gocDe = 0.0;
    double vai = sangRadian(55.0);
    double khuyu = sangRadian(-110.0);
    double coTay = sangRadian(55.0);
};

struct KhoangGoc {
    double nhoNhat;
    double lonNhat;

    bool chua(double giaTri, double dungSai = 1e-9) const {
        return giaTri >= nhoNhat - dungSai && giaTri <= lonNhat + dungSai;
    }
};

struct GioiHanKhop {
    KhoangGoc gocDe{sangRadian(-170.0), sangRadian(170.0)};
    KhoangGoc vai{sangRadian(-25.0), sangRadian(150.0)};
    KhoangGoc khuyu{sangRadian(-160.0), sangRadian(160.0)};
    KhoangGoc coTay{sangRadian(-170.0), sangRadian(170.0)};

    bool chua(const GocKhop& gocKhop) const {
        return gocDe.chua(gocKhop.gocDe) && vai.chua(gocKhop.vai) && khuyu.chua(gocKhop.khuyu) &&
               coTay.chua(gocKhop.coTay);
    }
};

struct TuTheRobot {
    MaTran4 de;
    MaTran4 vai;
    MaTran4 khuyu;
    MaTran4 coTay;
    MaTran4 dauBut;
    Vecto3 viTriDe;
    Vecto3 viTriVai;
    Vecto3 viTriKhuyu;
    Vecto3 viTriCoTay;
    Vecto3 viTriDauBut;
};

inline TuTheRobot tinhDongHocThuan(const GocKhop& gocKhop, const KichThuocRobot& kichThuoc = {}) {
    TuTheRobot ketQua;
    ketQua.de = MaTran4::quayZ(gocKhop.gocDe);
    // Tu mot diem tren khau con: quay va tinh tien ve khau cha, roi di tiep ve chan de.
    ketQua.vai = MaTran4::quayY(-gocKhop.vai) * MaTran4::tinhTien(0.0, 0.0, kichThuoc.chieuCaoVai) *
                 ketQua.de;
    ketQua.khuyu = MaTran4::quayY(-gocKhop.khuyu) *
                   MaTran4::tinhTien(kichThuoc.canhTayTren, 0.0, 0.0) * ketQua.vai;
    ketQua.coTay = MaTran4::quayY(-gocKhop.coTay) * MaTran4::tinhTien(kichThuoc.cangTay, 0.0, 0.0) *
                   ketQua.khuyu;
    ketQua.dauBut = MaTran4::tinhTien(0.0, 0.0, -kichThuoc.chieuDaiBut) * ketQua.coTay;
    ketQua.viTriDe = ketQua.de.bienDoiDiem({});
    ketQua.viTriVai = ketQua.vai.bienDoiDiem({});
    ketQua.viTriKhuyu = ketQua.khuyu.bienDoiDiem({});
    ketQua.viTriCoTay = ketQua.coTay.bienDoiDiem({});
    ketQua.viTriDauBut = ketQua.dauBut.bienDoiDiem({});
    return ketQua;
}

enum class NhanhKhuyu { KhuyuTren, KhuyuDuoi };
enum class TrangThaiNghiem { ThanhCong, NgoaiTamVoi, VuotGioiHanKhop, DauVaoKhongHopLe };

struct KetQuaDongHocNghich {
    TrangThaiNghiem trangThai = TrangThaiNghiem::DauVaoKhongHopLe;
    GocKhop gocKhop;
    double saiSo = numeric_limits<double>::infinity();
    bool suyBien = false;

    bool thanhCong() const {
        return trangThai == TrangThaiNghiem::ThanhCong;
    }

    string thongBao() const {
        switch (trangThai) {
            case TrangThaiNghiem::ThanhCong:
                return suyBien ? "Vi tri suy bien: can than khi doi nhanh quy dao"
                               : "Co nghiem dong hoc nghich";
            case TrangThaiNghiem::NgoaiTamVoi:
                return "Diem dich nam ngoai vung voi cua robot";
            case TrangThaiNghiem::VuotGioiHanKhop:
                return "Nghiem vuot gioi han goc khop";
            case TrangThaiNghiem::DauVaoKhongHopLe:
                return "Toa do hoac kich thuoc robot khong hop le";
        }
        return "";
    }
};

// Giai dong hoc nghich cho de xoay va hai doan tay trong mat phang.
// Goc co tay bang am tong goc vai va khuyu de giu but thang dung.
// Nhanh khuyu tren co goc khuyu am. Mo phong se kiem tra khoang cach den ban.
inline KetQuaDongHocNghich giaiDongHocNghich(const Vecto3& dich,
                                             NhanhKhuyu nhanhKhuyu = NhanhKhuyu::KhuyuTren,
                                             const KichThuocRobot& kichThuoc = {},
                                             const GioiHanKhop& gioiHan = {}) {
    KetQuaDongHocNghich ketQua;
    if (!huuHan(dich) || !isfinite(kichThuoc.chieuCaoVai) || !isfinite(kichThuoc.canhTayTren) ||
        !isfinite(kichThuoc.cangTay) || !isfinite(kichThuoc.chieuDaiBut) ||
        kichThuoc.canhTayTren <= 0.0 || kichThuoc.cangTay <= 0.0 || kichThuoc.chieuDaiBut < 0.0) {
        return ketQua;
    }

    const double banKinhNgang = hypot(dich.x, dich.y);
    const double doCaoTuongDoi = dich.z + kichThuoc.chieuDaiBut - kichThuoc.chieuCaoVai;
    const double binhPhuongTamVoi = banKinhNgang * banKinhNgang + doCaoTuongDoi * doCaoTuongDoi;
    const double daiCanhTay = kichThuoc.canhTayTren;
    const double daiCangTay = kichThuoc.cangTay;
    const double cosGocKhuyu =
        (binhPhuongTamVoi - daiCanhTay * daiCanhTay - daiCangTay * daiCangTay) /
        (2.0 * daiCanhTay * daiCangTay);
    if (!isfinite(cosGocKhuyu) || cosGocKhuyu < -1.0 - 1e-10 || cosGocKhuyu > 1.0 + 1e-10) {
        ketQua.trangThai = TrangThaiNghiem::NgoaiTamVoi;
        return ketQua;
    }

    const double doLonGocKhuyu = acos(clamp(cosGocKhuyu, -1.0, 1.0));
    ketQua.gocKhop.gocDe = banKinhNgang > 1e-12 ? atan2(dich.y, dich.x) : 0.0;
    ketQua.gocKhop.khuyu = nhanhKhuyu == NhanhKhuyu::KhuyuTren ? -doLonGocKhuyu : doLonGocKhuyu;
    ketQua.gocKhop.vai = atan2(doCaoTuongDoi, banKinhNgang) -
                         atan2(daiCangTay * sin(ketQua.gocKhop.khuyu),
                               daiCanhTay + daiCangTay * cos(ketQua.gocKhop.khuyu));
    ketQua.gocKhop.coTay = -(ketQua.gocKhop.vai + ketQua.gocKhop.khuyu);
    ketQua.suyBien = banKinhNgang < 1e-8 || abs(sin(ketQua.gocKhop.khuyu)) < 1e-8;
    ketQua.saiSo = khoangCach(tinhDongHocThuan(ketQua.gocKhop, kichThuoc).viTriDauBut, dich);
    ketQua.trangThai = gioiHan.chua(ketQua.gocKhop) ? TrangThaiNghiem::ThanhCong
                                                    : TrangThaiNghiem::VuotGioiHanKhop;
    return ketQua;
}

} // Khong gian ten robot.
