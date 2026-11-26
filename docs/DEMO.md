# Kịch bản trình bày và kiểm tra

Mở `D:\RobotDrawingLab\bin\RobotDrawingLab.exe`. Ảnh hoa có sẵn xuất hiện trong tab **Vẽ & tô**. Cảnh bên phải là robot và mặt giấy. Có thể bật quỹ đạo đầu bút ở tab **Kiểm chứng**. Bảng dưới cảnh cho biết tọa độ bút, bốn góc khớp và ma trận khâu đang chọn.

## 1. Vẽ lại ảnh

1. Trong tab **Vẽ & tô**, bấm **Ảnh bông hoa** để bắt đầu với cùng một dữ liệu mẫu.
2. Bấm **Vẽ nét**. Robot tự truy nét, lập đường đi và chạy.
3. Kéo thanh **Tốc độ di chuyển đầu bút** để quan sát nhanh hoặc chậm. Bấm **Chạy / Tạm dừng** để giữ tư thế hiện tại; bấm tiếp để chạy lại.
4. Khi đang dừng, bấm **Tiến một bước** để chạy thêm một khoảng thời gian ngắn.
5. Quan sát đầu bút nhấc lên khi chuyển sang nét rời, rồi hạ xuống trước khi ghi mực.
6. Bấm **Lưu tranh PNG…** để lưu phần tranh thực tế đã vẽ. Nút **Về đầu đường vẽ** đưa robot về tư thế chuẩn và xóa đường đi hiện tại; sau đó bấm **Vẽ nét** hoặc **Tô vùng đã chọn** để tạo đường mới. Nút **Xóa mặt giấy** xóa mực đã ghi.

Để thử lá cờ, bấm **Mở ảnh…**, chọn `examples/la-co.svg`, để **Cách lấy nét** ở **Tự động** rồi bấm **Vẽ nét**. Quan sát bút vẽ hết khung chữ nhật, nhấc lên và vẽ đủ năm cánh sao. Có thể bấm vào tâm sao trên ảnh xem trước, chọn màu rồi bấm **Tô vùng đã chọn**. Phím **2** chuyển sang góc gần nhìn từ trên để đối chiếu vệt bút với ảnh gốc.

Ảnh riêng có thể là SVG, PNG, JPG hoặc BMP. Chương trình đặt ảnh vừa khung trắng 256×256 và giữ tỉ lệ. Với hình có mảng đặc, chọn **Đường bao** để lần theo biên ngoài và biên trong. Với bản vẽ nét mảnh, chọn **Tâm nét**. Chỉnh **Ngưỡng sáng/tối** khi ảnh quá nhạt để thay đổi đường vẽ.

Thầy có thể kiểm tra nét mực có theo đúng đầu bút và các đoạn rời có giữ được sự tách biệt. Thuật toán raster hóa nét là Bresenham, tương ứng **CG.3-2D Graphics.pdf**, tr. 11–15. Đọc ảnh và truy nét là phần xử lý ảnh bổ sung.

## 2. Tô màu theo ảnh và tùy chỉnh từng phần

1. Bấm **Mở ảnh…**, chọn `examples/bong-den.png`. Đối chiếu màu vàng cam của bóng đèn, màu xám của bánh răng và vành sáng ở tâm.
2. Bấm **Tô toàn ảnh theo màu gốc**. Tạm dừng giữa lượt tô và quan sát vùng còn trắng phía trước đầu bút. Tiếp tục đến khi hình đủ màu.
3. Bấm vào thân bóng trên ảnh xem trước. Bấm **Chọn màu…**, chọn xanh dương rồi bấm **Tô vùng đã chọn**. Sau khi robot tô xong, lặp lại với bánh răng và một màu khác. Quan sát các vùng còn lại trên giấy.
4. Giảm **Dung sai màu** để chọn riêng phần bóng đổ, hoặc tăng dung sai để lấy các sắc độ liền kề. **Lưu tranh PNG…** xuất đúng phần robot đã tô tới thời điểm bấm.

Có thể đối chiếu với hai bản xuất từ mô phỏng. [Bóng đèn theo màu gốc](bong-den-mau-goc.png) · [Ví dụ đổi màu hai vùng](bong-den-doi-mau.png)

Để so sánh hai lượt tô, mở lại ảnh, chọn **Tự động** và bấm **Vẽ nét**. Bỏ chọn **Tô đè lên viền**, đợi nét cuối kết thúc rồi bấm **Tô toàn ảnh theo màu gốc** để giữ đường viền đậm. Sau đó bật **Tô đè lên viền** và tô toàn ảnh lần nữa. Quan sát màu gốc phủ dần đường viền theo đầu bút. [Hình khi bỏ chọn tô đè](bong-den-co-vien.png)

### Thử tô một cánh hoa trên hình nét

1. Bấm vào khoảng trắng bên trong một cánh hoa trên ảnh nhỏ bên trái.
2. Quan sát đường viền đánh dấu vùng chọn. Dấu cộng chỉ điểm hạt giống.
3. Bấm **Chọn màu…**, chọn màu mong muốn và xác nhận hộp thoại.
4. Bấm **Tô vùng đã chọn**. Robot chạy các đoạn quét ngang bên trong vùng.
5. Tạm dừng để đối chiếu vị trí bút với vệt màu. Chọn tiếp cánh khác và màu khác sau khi xong.

Flood Fill 4 lân cận dùng hàng đợi tìm mặt nạ vùng. Từ mặt nạ, chương trình tìm các đoạn pixel liên tiếp trên mỗi dòng và đổi chiều xen kẽ để lập đường tô. Liên hệ **CG.3-2D Graphics.pdf**, tr. 83–89 và tr. 79–82. Dạng dòng quét trong slide dùng giao cạnh đa giác; chương trình áp dụng ý tưởng đó vào mặt nạ raster.

## 3. Kiểm tra quan hệ khớp cha và khớp con

1. Chuyển sang tab **Kết cấu**. Việc chuyển tab tạm dừng hoạt ảnh.
2. Bật **Hiện hệ trục tọa độ các khớp**. Chữ X, Y, Z ở đầu mỗi trục đi theo tư thế của khớp. X màu đỏ, Y màu xanh lá và Z màu xanh dương.
3. Thay đổi thanh **Đế — quay quanh Z**. Quan sát cả chuỗi vai, khuỷu, cổ tay và bút quay theo.
4. Giữ đế, thay đổi **Khuỷu — quay quanh Y cục bộ**. Góc tương đối của các khớp khác giữ nguyên.
5. Trong tab **Kết cấu**, bấm gần đế, vai, khuỷu, cổ tay hoặc đầu bút. Bảng **Ma trận tổng hợp** dưới cảnh hiện tên bộ phận và lấy hệ trục cố định ở chân đế làm mốc. Ba hàng **Trục X**, **Trục Y**, **Trục Z** cho biết hướng các trục của bộ phận. Ba số đầu trong hàng **Vị trí** lần lượt là tọa độ X, Y, Z của bộ phận đó.
6. Chọn **Cổ tay** để đọc công thức **Quay tại cổ tay × Tịnh tiến dọc cẳng tay × Ma trận khuỷu**. Các thừa số được ghi theo thứ tự nhân với vectơ hàng. Chọn **Đầu bút** rồi đối chiếu hàng **Vị trí** với dòng **Đầu bút** bên trái. Bảng ma trận làm tròn đến hai chữ số thập phân.
7. Bấm **Tư thế ban đầu** để trở lại tư thế kiểm tra chuẩn.

Liên hệ **CG.4-3D Graphics-fix.pdf**, tr. 5–16. Với vectơ hàng, `M_con = M_cục_bộ × M_cha`.

## 4. Bóc tách kết cấu

1. Trong tab **Kết cấu**, ở **Kiểu hiển thị mô hình**, chọn **Mở vỏ — xem cơ cấu bên trong**.
2. Quan sát trục khớp, khối mô phỏng động cơ và đĩa răng.
3. Kéo thanh **Bóc tách các chi tiết** để tăng độ dời các phần. Kéo chuột phải trong cảnh để xem từ phía khác.
4. Chọn **Khung dây (wireframe)** để thấy cạnh hình học, rồi chọn **Vỏ đầy đủ** để lắp hình hiển thị trở lại.
5. Chuyển về tab **Vẽ & tô**. Độ bóc tách tự về 0 để tiếp tục thao tác vẽ trên mô hình lắp ráp.

Liên hệ **CG.4-3D Graphics-fix.pdf**, tr. 19–26 về khung và mặt đa giác. Các cơ cấu là mô hình cơ khí giản lược dùng để quan sát kết cấu và phép biến đổi.

## 5. Thử các tình huống robot

Trong tab **Kiểm chứng**, thực hiện lần lượt

| Điều khiển | Quan sát mong đợi |
|---|---|
| **Thử điểm trong tầm với** | Robot nhận tư thế hợp lệ, mục tiêu hiện xanh và bảng tọa độ cập nhật |
| **Thử điểm ngoài tầm với** | Mục tiêu hiện đỏ, thông báo từ chối và giữ tư thế hợp lệ trước |
| **Thử giới hạn khớp** | Thông báo trường hợp yêu cầu góc vai vượt giới hạn cấu hình |
| **Đổi nhánh động học nghịch** | Nếu nhánh khác hợp lệ, tư thế thay đổi nhưng đầu bút giữ tọa độ. Nếu nhánh bị giới hạn hoặc cắt bàn, chương trình thông báo và giữ trạng thái trước |
| Nhấp một điểm trên mặt giấy trong cảnh | Điểm nhấp được đổi sang tọa độ giấy rồi thử động học nghịch |
| **Xuất ma trận CSV…** | Lưu ma trận tổng hợp của đế, vai, khuỷu, cổ tay và đầu bút so với cùng hệ trục cố định ở chân đế. Mỗi dòng ghi tên khâu, số hàng và bốn giá trị. |

Động học nghịch và kiểm tra tầm với là phần robotics bổ sung. Các ma trận dùng nền tảng biến đổi của **CG.4-3D Graphics-fix.pdf**, tr. 5–16.

## 6. Kiểm tra camera và thuật toán hiển thị

1. Kéo chuột phải trong cảnh để xoay góc quan sát. Cuộn chuột để thu/phóng.
2. Bấm **1**, **2**, **3** khi cảnh có tiêu điểm để chuyển lần lượt sang góc tổng thể, gần nhìn từ trên và góc thấp. Có thể bấm trong cảnh trước để nhận phím.
3. Trong tab **Kiểm chứng**, bỏ chọn rồi chọn lại **Chiếu phối cảnh (bỏ chọn: song song)**.
4. Bật/tắt **Chiếu sáng và pháp tuyến** để so sánh hình khối.
5. Bật/tắt **Khử mặt khuất bằng Z-buffer** để quan sát các khâu che khuất nhau.
6. Bật/tắt **Hiện quỹ đạo thực tế của đầu bút**. Đường màu xanh nhạt biểu diễn các vị trí đã đi qua, gồm cả chuyển động nhấc bút.

Liên hệ **CG.4-3D Graphics-fix.pdf**, tr. 28–48 và **CG.6-Advanced Graphics Techniques.pdf**, tr. 17–20, 23–31. Phép chiếu, chiếu sáng và thử độ sâu dùng chức năng OpenGL/GLU.

## 7. Thử dữ liệu ảnh

| Trường hợp | Cách kiểm tra |
|---|---|
| Ảnh trắng | Bấm **Vẽ nét**, đọc thông báo thiếu nét và nạp lại ảnh mẫu |
| Bấm vào viền đen | Đọc thông báo chọn lại khoảng trắng bên trong vùng |
| Bấm nền ngoài bông hoa | Vùng chạm mép ảnh bị từ chối khi chạy tô |
| Vùng hở viền | Mặt nạ có thể thông ra mép ảnh. Quan sát cảnh báo, điều chỉnh ngưỡng hoặc dùng ảnh có đường viền kín |
| Hình có nhiều nét rời | Quan sát thao tác nhấc bút giữa các nét |
| Ảnh rất nhiều nhiễu hoặc nét nhỏ | Sau khi thu về 256×256, chi tiết có thể mất hoặc bị tách thành nhiều nét. So sánh với ảnh gốc để giải thích giới hạn độ phân giải |

## Giải thích ma trận khi thầy đối chiếu slide

“Em viết điểm P thành một hàng và tính Q = P × M như trang 5. Với đầu bút, lấy P = (0, 0, 0, 1) trong hệ trục gắn với ngòi. Nhân với ma trận tổng hợp của đầu bút cho ra hàng cuối của ma trận. Ba số đầu là tọa độ ngòi so với chân đế. Khi xoay đế, các khớp gắn trên đế quay theo.”

## Giải thích đường cong và nội suy

“Bản này lấy nét trực tiếp từ ảnh raster. Khi robot chạy, chương trình chia chuyển động giữa hai điểm thành các bước nhỏ theo tọa độ Cartesian rồi tính góc khớp. Các đường cong trong ảnh được biểu diễn bằng nhiều đoạn ngắn. Đề tài áp dụng biến đổi, tô vùng, raster hóa, hệ quan sát và chiếu sáng của slide. Chương Bézier là hướng mở rộng nếu bổ sung chức năng dựng hoặc khớp đường cong.”

