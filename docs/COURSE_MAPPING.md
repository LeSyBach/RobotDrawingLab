# Đối chiếu chương trình với slide học phần

Nguồn là các PDF trong `reference/Slides VI`. Số trang tính từ trang đầu của PDF, bắt đầu bằng 1. Chương 4 dùng bản `CG.4-3D Graphics-fix.pdf` để thống nhất số trang khi thuyết trình.

## Các phần đã cài đặt

| Chức năng trong chương trình | Nội dung tương ứng trong slide | PDF và trang | Phần cài đặt để giải thích |
|---|---|---|---|
| Tab **Kết cấu**, lựa chọn **Vỏ đầy đủ**, **Mở vỏ — xem cơ cấu bên trong**, **Khung dây (wireframe)** | Khung kết nối và mặt đa giác | `CG.4-3D Graphics-fix.pdf`, tr. 19–26. LAB 04 trong `CG.7-Labs.pdf`, tr. 1 | Hộp, trụ và các mặt tạo thành đế, các đoạn tay, vỏ khớp và bộ phận bên trong. Khung dây hiện cạnh của lưới hình học. |
| Bốn thanh trượt khớp, hệ trục và bảng ma trận | Tọa độ thuần nhất, tịnh tiến, quay và kết hợp phép biến đổi | `CG.4-3D Graphics-fix.pdf`, tr. 5–16 | `MaTran4` tự tính ma trận 4×4. Chuỗi ma trận từ đế đến bút được dùng cho cả dựng hình và động học thuận. |
| Thanh trượt **Bóc tách các chi tiết** | Tịnh tiến, quay, mặt đa giác | `CG.4-3D Graphics-fix.pdf`, tr. 6, 11–16, 24–26 | Độ dời hiển thị tách vỏ, đĩa răng và phần mô phỏng động cơ. Dữ liệu động học vẫn gắn với cấu trúc lắp ráp gốc. |
| Kéo chuột phải, cuộn chuột, phím **1/2/3** | Phép chiếu và hệ quan sát | `CG.4-3D Graphics-fix.pdf`, tr. 28–48. LAB 04 trong `CG.7-Labs.pdf`, tr. 1–2 | Camera dùng `gluLookAt`. Phối cảnh dùng `gluPerspective`; song song trực giao dùng `glOrtho`. |
| Ô chọn **Chiếu phối cảnh (bỏ chọn: song song)** | Chiếu song song, trực giao và phối cảnh | `CG.4-3D Graphics-fix.pdf`, tr. 29–36 | Cùng một tư thế robot được quan sát bằng hai phép chiếu. |
| Các nút, thanh trượt, nhấp chọn khớp và vùng ảnh | Lập trình sự kiện chuột, bàn phím, cửa sổ | `CG.2-Programming CG in OpenGL.pdf`, tr. 10, 19–21 | Giao diện dùng Win32. Các sự kiện cập nhật trạng thái và yêu cầu vẽ lại cảnh OpenGL. |
| Vệt bút xuất hiện trên mặt giấy, lưu PNG | Rời rạc hóa nét và Bresenham | `CG.3-2D Graphics.pdf`, tr. 7, 11–15. LAB 01 trong `CG.7-Labs.pdf`, tr. 1 | `veVetMuc` dùng Bresenham nối hai vị trí bút và đặt các dấu bút tròn lên ảnh kết quả 512×512. |
| Nhấp một vùng trong ảnh xem trước | Tô theo đường biên, điểm hạt giống, 4 lân cận và hàng đợi | `CG.3-2D Graphics.pdf`, tr. 78–79, 83–89. LAB 02 trong `CG.7-Labs.pdf`, tr. 1 | Flood Fill tìm thành phần liên thông từ điểm được chọn. Với ảnh nhập, tiêu chí là độ lệch RGB so với màu hạt giống. Với ảnh hoa tạo sẵn, tiêu chí là ngưỡng sáng. Đường viền trên khung xem trước đánh dấu mặt nạ. |
| **Tô vùng đã chọn** | Tô theo dòng quét | `CG.3-2D Graphics.pdf`, tr. 79–82. LAB 02 trong `CG.7-Labs.pdf`, tr. 1 | `lapNetToTheoDong` tìm từng đoạn pixel liên tiếp trong mặt nạ raster, đổi chiều xen kẽ và tạo nét rời cho từng đoạn. Đây là áp dụng ý tưởng dòng quét trên mặt nạ ảnh. Slide tr. 80–82 trình bày dạng giao điểm cạnh đa giác. |
| Bật/tắt **Khử mặt khuất bằng Z-buffer** | Depth-buffer/Z-buffer | `CG.6-Advanced Graphics Techniques.pdf`, tr. 17–20. LAB 06 trong `CG.7-Labs.pdf`, tr. 2 | Chương trình dùng bộ đệm sâu 24 bit và `GL_DEPTH_TEST` của OpenGL. |
| Bật/tắt **Chiếu sáng và pháp tuyến** | Ánh sáng môi trường, Lambert và Gouraud | `CG.6-Advanced Graphics Techniques.pdf`, tr. 23–31. LAB 07 trong `CG.7-Labs.pdf`, tr. 2 | Vật liệu, pháp tuyến, nguồn sáng fixed-function và `GL_SMOOTH` tạo ảnh được chiếu sáng với nội suy màu ở các đỉnh. |
| Chuyển pixel ảnh thành tọa độ giấy | Tỉ lệ và tịnh tiến 2D | `CG.3-2D Graphics.pdf`, tr. 104–115 | `diemAnhSangToaDoGiay` đổi pixel gốc trên trái sang tọa độ trên mặt giấy, lấy chân đế làm gốc và trục Z hướng lên. |

## Quy ước ma trận

Chương trình dùng vectơ hàng như trang 5 của `CG.4-3D Graphics-fix.pdf`. Mỗi điểm được viết dưới dạng `P = (x, y, z, 1)`.

```text
Q = P × M
```

Trong `src/robot_math.h`, mỗi dòng của công thức là một hàng của ma trận trong slide.

| Trang | Phép biến đổi | Ký hiệu trong mã |
|---|---|---|
| 6 | Tịnh tiến | `tx`, `ty`, `tz` là ba độ dời ở hàng cuối của `M`. |
| 9 | Tỉ lệ | `sx`, `sy`, `sz` là ba hệ số trên đường chéo của `M`. |
| 12 | Quay | `alpha` là góc α tính bằng radian. `Rx`, `Ry`, `Rz` lần lượt biểu diễn R(x, α), R(y, α), R(z, α). |

Trang 16 ghép hai phép biến đổi theo thứ tự thực hiện. Nếu áp dụng M1 rồi M2 thì `Q = P × M1 × M2`. Trong ma trận tư thế, ba hàng đầu biểu diễn hướng của ba trục bộ phận, hàng cuối chứa `(tx, ty, tz, 1)`. Bảng dưới cảnh và tệp CSV đều dùng thứ tự này. Chương trình lưu từng hàng liên tiếp. Khi vẽ, OpenGL đọc cùng dãy số theo cột và dựng đúng vị trí đã tính.

Mô hình dùng trục Z hướng lên. Hai đoạn tay có chiều dài 3,4 và 3,0 đơn vị cảnh, vai cao 1,2 và khoảng cách từ tâm cổ tay đến mũi bút là 1,45. Khi đổi tọa độ từ khớp con về chân đế, trước tiên đưa điểm về khớp cha rồi nhân với ma trận của khớp cha.

```text
M_đế    = Rz(q_đế)
M_vai   = Ry(-q_vai) × T(0,0,1.2) × M_đế
M_khuỷu = Ry(-q_khuỷu) × T(3.4,0,0) × M_vai
M_cổ    = Ry(-q_cổ) × T(3.0,0,0) × M_khuỷu
M_bút   = T(0,0,-1.45) × M_cổ
P_bút   = (0,0,0,1) × M_bút
```

Dấu âm ở phép quay quanh Y làm cho góc vai dương nâng tay lên theo quy ước mô hình. Góc của thanh trượt là giá trị khớp so với mốc, tính bằng độ trên giao diện và đổi sang radian trong phép toán. Ma trận được tính lại từ các góc hiện tại.

## Các phần bổ sung cho công việc vẽ

- **Động học nghịch** là công thức giải tích của đế xoay và hai đoạn tay phẳng. Cổ tay bù góc để giữ bút thẳng đứng khi đi tới tọa độ đích. Nghiệm được kiểm tra theo giới hạn khớp; các điểm vị trí đặc trưng được kiểm tra với mặt bàn.
- **Truy nét ảnh** dùng ngưỡng độ sáng để tìm vùng tối. Với ảnh màu, chương trình còn xét giá trị nhỏ nhất trong ba kênh RGB để nhận các màu sáng trên nền trắng. Với tranh nét, chương trình làm mảnh Zhang–Suen và lần theo đồ thị pixel. Với mảng đặc, robot đi theo đường bao của mặt nạ tổng hợp và các cạnh tiếp giáp giữa vùng tối và vùng màu sáng bên trong. Chế độ tự động chọn cách truy dựa trên độ dày nét. Nút **Mở ảnh…** nhận SVG, PNG, JPG và BMP. Ảnh được giữ tỉ lệ trong khung trắng 256×256 trước khi truy nét.
- **Đường đi đầu bút** có trạng thái bút lên/bút xuống. Trong khi chạy, chương trình nội suy tuyến tính theo tọa độ Cartesian giữa các điểm liên tiếp, rồi giải động học nghịch cho các vị trí trung gian. Vị trí thực tế được tính lại bằng động học thuận để ghi nét.
- **Kiểm tra hình học** xử lý điểm ngoài tầm với, góc vượt giới hạn và một số trường hợp tư thế cắt mặt bàn. Phần này bổ sung robotics trên cơ sở biến đổi hình học.

## Phạm vi khi thuyết trình

1. Chương trình hiện tại dùng nét truy từ ảnh và các đoạn nội suy tuyến tính khi hoạt ảnh. Chương **CG.5-Curves and Surfaces.pdf**, tr. 6–15 về de Casteljau/Bézier là tài liệu có thể dùng khi phát triển chức năng dựng đường cong trong tương lai. Bézier chưa tham gia đường xử lý hiện tại.
2. Tìm vùng bằng Flood Fill và tạo đường tô theo dòng quét là hai bước. Thứ tự duyệt hàng đợi của Flood Fill chỉ phục vụ tìm mặt nạ. Khi tô toàn ảnh theo màu gốc, đầu bút đi theo các đoạn quét và ghi RGB tại vùng tiếp xúc lên mặt giấy. Khi tô một thành phần, màu mực cố định trong suốt lượt tô.
3. Ảnh mẫu hoa được tạo thành ảnh raster trước, rồi đi qua cùng chuỗi truy nét như ảnh người dùng mở. Các đường cánh mẫu dùng phương trình lượng giác và nhụy dùng đường tròn raster.
4. Tô bằng các đoạn quét raster khác bài toán scanline tìm giao điểm cạnh đa giác. Khi trả lời thầy, nói rõ dữ liệu đầu vào và phép tính đã cài đặt.
5. Đồ án tự tính ma trận, động học, Bresenham, truy nét và mặt nạ vùng. Phép chiếu, chiếu sáng, nội suy màu bề mặt và phép thử Z-buffer dùng chức năng OpenGL/GLU.
6. Kết cấu vỏ, động cơ, trục và đĩa răng được mô hình hóa giản lược để quan sát cách lắp. Các khoảng cách là đơn vị cảnh minh họa; mô phỏng chưa có tải trọng, mô-men, độ rơ, vật lý hay kiểm tra va chạm đầy đủ giữa mọi chi tiết.

