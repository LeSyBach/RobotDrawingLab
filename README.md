# Robot Drawing Lab

Chương trình C++/OpenGL cho Windows mô phỏng cánh tay robot vẽ và tô ảnh trên mặt giấy phẳng. Giao diện tiếng Việt có ba tab để chạy mô phỏng, quan sát kết cấu và kiểm chứng các tình huống.

## Chạy chương trình

Mở `D:\RobotDrawingLab\bin\RobotDrawingLab.exe`. Bản thực thi dùng các thư viện hệ thống Windows, chạy độc lập và có sẵn ảnh bông hoa. Có thể chạy ngoại tuyến; chương trình không tải mô hình, gửi ảnh hay kết nối dịch vụ mạng.

Khi thuyết trình, mở rộng cửa sổ để xem bảng ma trận dưới cảnh. Hướng dẫn trình bày và đối chiếu bài học nằm trong

- [Kịch bản demo và các tình huống kiểm tra](docs/DEMO.md)
- [Đối chiếu thuật toán với slide và số trang PDF](docs/COURSE_MAPPING.md)

## Vẽ và tô từ ảnh

Trong tab **Vẽ & tô**, bấm **Mở ảnh…** để nhập SVG, PNG, JPG, JPEG hoặc BMP. Thử `examples/bong-den.png` để tô màu, `examples/hoa.svg` để vẽ nét hoặc `examples/la-co.svg` để quan sát đường bao.

Ảnh được giữ đúng tỉ lệ trong khung 256×256. Khung xem trước hiển thị màu RGB của ảnh trên giấy trắng. Chương trình tính thêm độ sáng tại từng pixel để **Vẽ nét**. **Ngưỡng sáng/tối** điều chỉnh bước lấy nét này. SVG được kết xuất từ các hình và đường vector. Với SVG chứa chữ hoặc ký hiệu dùng lại, có thể chuyển chúng thành path trước khi nhập.

Trong **Cách lấy nét**, chọn **Tự động** để chương trình ước lượng độ dày của nét. Với mảng đặc, **Đường bao** vẽ mép ngoài và ranh giới giữa vùng tối với vùng màu sáng. Chương trình xét cả các kênh RGB để nhận ra đường cong của bóng đèn vàng trên giấy trắng. Với lá cờ, robot vẽ khung rồi nhấc bút để đi quanh ngôi sao. **Tâm nét** làm mảnh bằng thuật toán Zhang–Suen, phù hợp với tranh nét như bông hoa.

Ô **Tô đè lên viền** bên cạnh **Lưu tranh PNG…** được bật mặc định. Sau khi **Vẽ nét**, bấm **Tô toàn ảnh theo màu gốc** để robot tô cả phần viền bằng màu của ảnh. Khi **Tô vùng đã chọn**, màu mới chỉ đè lên phần viền nằm trong vùng đó. Muốn giữ viền đậm, bỏ dấu chọn trước khi bắt đầu lượt tô. Nếu đổi lựa chọn giữa chừng, thiết lập mới áp dụng từ lượt tô kế tiếp.

- **Tô toàn ảnh theo màu gốc** cho robot quét từng dòng để tái tạo hình. Đầu bút mô phỏng lấy màu RGB của ảnh ở từng điểm trong vệt tiếp xúc, giữ cả sắc độ và chuyển màu. Mực xuất hiện dần trên giấy. Bấm lại nút này sau khi đổi màu riêng để khôi phục hình gốc bằng một lượt tô.
- **Tô vùng đã chọn** dùng một màu cho một phần của hình. Nhấp vào phần đó trên ảnh xem trước, bấm **Chọn màu…**, rồi **Tô vùng đã chọn**. Đường viền đứt đoạn đánh dấu vùng chọn. **Dung sai màu** thấp giúp tách hai sắc độ gần nhau. Tăng dung sai để lấy thêm các sắc độ liền kề. Có thể lần lượt tô nhiều phần trên cùng tờ giấy.
- Khi nhập ảnh, vùng tô được giới hạn trong phần có nội dung. Nền trắng đục cũng chọn được. Với ảnh bông hoa đen trắng tạo sẵn, hãy chọn một vùng sáng khép kín giữa các nét.

| Điều khiển | Chức năng |
|---|---|
| **Chạy / Tạm dừng** | Tiếp tục hoặc tạm dừng đường hiện tại |
| **Tiến một bước** | Chạy thêm một khoảng thời gian ngắn rồi dừng |
| **Về đầu đường vẽ** | Về tư thế ban đầu, xóa đường đi đang thực hiện; bấm Vẽ nét hoặc Tô vùng để tạo đường đi mới |
| **Xóa mặt giấy** | Xóa ảnh mực đã ghi |
| **Lưu tranh PNG…** | Xuất ảnh kết quả 512×512 tại thời điểm lưu |
| **Tốc độ di chuyển đầu bút** | Điều chỉnh tốc độ hoạt ảnh |

Nút **Vẽ nét** dùng mực đen đậm. Mực tùy chỉnh mặc định là hồng. Ô màu dưới nút lưu cho biết mã màu đã chọn. Mỗi lượt tô vùng giữ bản sao màu và vùng đã chọn. Đổi ảnh sẽ đặt lại mặt giấy và đường hiện tại.

## Quan sát kết cấu

Tab **Kết cấu** có ba kiểu hiển thị **Vỏ đầy đủ**, **Mở vỏ — xem cơ cấu bên trong**, **Khung dây (wireframe)**. Thanh **Bóc tách các chi tiết** tách các phần để quan sát. Mô hình có đế, hai đoạn tay, cổ tay, bút, vỏ khớp, trục và các khối mô phỏng động cơ, đĩa răng.

Bốn thanh trượt điều khiển góc đế, vai, khuỷu và cổ tay. Bật **Hiện hệ trục tọa độ các khớp** để quan sát trục cục bộ. Bấm vào đế, vai, khuỷu, cổ tay hoặc đầu bút trong cảnh. Bảng **Ma trận tổng hợp** cho biết hướng và vị trí của bộ phận vừa chọn so với hệ trục cố định ở chân đế. Đọc ba số đầu của hàng **Vị trí** từ trái sang phải để biết tọa độ X, Y, Z. Nút **Tư thế ban đầu** đưa robot về tư thế chuẩn.

Tab **Kiểm chứng** cho phép thử điểm trong/ngoài tầm với, giới hạn khớp, đổi nhánh động học nghịch và xuất ma trận tổng hợp của cả năm bộ phận ra CSV. Nhấp trên mặt giấy trong cảnh để thử một tọa độ. Các ô chọn điều khiển phối cảnh/song song, ánh sáng, Z-buffer và quỹ đạo thực tế.

Chuyển sang tab **Kết cấu** hoặc **Kiểm chứng** sẽ tạm dừng hoạt ảnh. Khi rời tab **Kết cấu**, độ bóc tách tự trở về 0.

## Camera và phím

- Kéo chuột phải trong cảnh để xoay góc nhìn.
- Cuộn chuột để thu/phóng.
- Phím **1**, **2**, **3** chọn góc tổng thể, gần nhìn từ trên và góc thấp.
- Phím **Space** gọi Chạy/Tạm dừng khi cửa sổ chính nhận bàn phím. Có thể bấm vào cảnh trước để đưa tiêu điểm về cửa sổ chính.

## Mã nguồn và biên dịch

Để đọc phần tô màu trong `src/main.cpp`, bắt đầu từ `toVungDaChon` hoặc `toToanAnhTheoMauGoc`. Hai hàm lập đường đi bằng `lapDuongDi`. Mỗi nhịp mô phỏng, `capNhatChuyenDong` cập nhật vị trí đầu bút. Khi chạm giấy, `veVetMuc` nối hai vị trí bút và `inDauBut` quyết định màu nào được ghi lên từng điểm, kể cả điểm nằm trên viền.

| Tệp | Nội dung |
|---|---|
| `src/main.cpp` | Giao diện Win32, cảnh OpenGL, camera, hiển thị kết cấu, hoạt ảnh, dấu bút và lưu PNG/CSV |
| `src/robot_math.h` | Vectơ, ma trận 4×4, động học thuận/nghịch và giới hạn khớp |
| `src/image_paths.h` | Ngưỡng hóa, làm mảnh, truy nét, Flood Fill, đường tô quét dòng và ảnh mẫu |
| `src/svg_import.cpp` | Đọc hình vector SVG bằng NanoSVG và kết xuất ảnh đầu vào |
| `build.ps1` | Biên dịch bằng g++ C++17 trên Windows |
| `CMakeLists.txt` | Cấu hình CMake cho Windows |
| `reference/Slides VI` | Bộ PDF học phần do người dùng cung cấp |

Dùng g++ của MSYS2 UCRT64 đã cài trên máy. Trong PowerShell

```powershell
Set-Location -LiteralPath 'D:\RobotDrawingLab'
.\build.ps1
```

Script tìm `g++.exe` trong PATH hoặc tại `C:\msys64\ucrt64\bin\g++.exe`, biên dịch bản thực thi vào `bin`. Các thư viện liên kết là OpenGL/GLU, GDI+, Common Controls và các thư viện hệ thống Windows. Thư viện runtime C++ được liên kết tĩnh.

Chạy `.\test.ps1` để biên dịch và chạy lại các kiểm tra toán học, truy nét, tô vùng và hoạt ảnh. Kiểm tra tích hợp gọi trực tiếp các hàm thực tế trong ứng dụng, chạy trọn đường vẽ mẫu và tô cả 8 vùng kín. Phần giao diện còn được kiểm tra bằng cách mở ứng dụng Windows.

Chương trình dùng ngữ cảnh OpenGL tương thích cổ điển trên Windows và fixed-function pipeline, cùng cách tiếp cận ma trận, phép chiếu, ánh sáng và depth test trong bộ slide. Giao diện dùng sự kiện Win32 thay GLUT.

## Phạm vi mô phỏng

- Kết cấu bên trong được dựng giản lược phục vụ quan sát và bóc tách. Mô phỏng tập trung vào hình học và động học. Tải trọng, mô-men động cơ, độ rơ và vật lý tiếp xúc chưa được tính.
- Kiểm tra tự động bao gồm tầm với, giới hạn góc và vị trí một số điểm của tay so với mặt bàn. Kiểm tra va chạm đầy đủ giữa mọi bộ phận chưa được cài đặt.
- Nhận ảnh dựa trên ngưỡng sáng/tối và truy nét. Ảnh chụp có bóng, nền phức tạp, vùng đặc hoặc chi tiết nhỏ có thể cho nhiều nét vụn khi thu về 256×256.
- Đường tô quét các đoạn liên tiếp trên mặt nạ raster. Bài toán scanline tìm giao điểm cạnh đa giác trong slide được đối chiếu như nguyên lý liên quan.
- Đường vẽ hiện tại dùng pixel truy nét và nội suy tuyến tính Cartesian giữa các điểm khi hoạt ảnh. Chưa dùng khớp đường cong Bézier. Nội dung Bézier trong slide được ghi nhận như hướng phát triển.
- Ma trận dùng vectơ hàng như slide chương 4. Điểm biến đổi theo `Q = P × M`. Bảng ma trận và tệp CSV mới có vị trí ở hàng cuối. Các CSV đã xuất bằng bản cũ có vị trí ở cột cuối.
- Độ dài là đơn vị cảnh minh họa. Kích thước giấy và robot được cấu hình để có vùng vẽ phù hợp trong mô phỏng.

