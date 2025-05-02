# Tác giả 
Họ tên sinh viên: Nguyễn Quốc Hoàn
MSSV: 24020135

# Dragon ball: Fireball Fury

Hòa mình vào thế giới pixel đầy màu sắc! Điều khiển Songoku nhanh nhẹn né tránh những quả cầu lửa đang lao tới từ bầu trời. Thu thập thật nhiều ngôi sao lấp lánh để tăng điểm. Phản xạ của bạn nhanh đến đâu? Hãy thử sức và lập kỷ lục điểm số cao nhất!

Trò chơi là một game né tránh (dodging) và thu thập (collecting) theo phong cách platformer 2D với đồ họa pixel art, lấy cảm hứng từ thế giới Dragon Ball. Người chơi sẽ vào vai Songoku, cố gắng sống sót càng lâu càng tốt và đạt điểm cao bằng cách né chướng ngại vật và thu thập vật phẩm.

# Mục tiêu:
Sống sót: Né tránh các quả cầu lửa (Ki Blast) liên tục rơi từ trên trời xuống.
Ghi điểm: Thu thập các ngôi sao rơi xuống để tăng điểm số hiện tại.
Đạt High Score: Vượt qua kỷ lục điểm số cao nhất đã được lưu trước đó.

# Điều khiển:
Di chuyển: Sử dụng phím A/D hoặc Mũi tên Trái/Phải để di chuyển Songoku sang trái hoặc phải trên mặt đất.
Nhảy: Nhấn phím Spacebar để Songoku nhảy lên, giúp né chướng ngại vật hoặc lấy các ngôi sao ở trên cao.
# Luồng Chơi Chính:
Bắt đầu: Người chơi chọn "New Game" từ Main Menu. Nhạc nền game bắt đầu.
![image](https://github.com/user-attachments/assets/404fa1ee-6ba8-4b66-8fc9-f948ea425660)


Gameplay: Songoku xuất hiện trên màn chơi.
![image](https://github.com/user-attachments/assets/cbb26bba-9cd9-42e4-aac0-f8ed6c0c6428)


Quả cầu lửa (Fireball): Các quả cầu lửa màu xanh bắt đầu rơi ngẫu nhiên từ phía trên màn hình với tốc độ khác nhau. 

![image](https://github.com/user-attachments/assets/11edbef1-535d-4e5f-abc6-b6083db5141c)

Người chơi phải liên tục di chuyển và nhảy để né chúng. Va chạm với quả cầu lửa sẽ dẫn đến Game Over.



Ngôi sao (Star): Các ngôi sao cũng rơi từ trên xuống, thường chậm hơn và có thể có quỹ đạo hơi khác so với quả cầu lửa. Người chơi cần di chuyển hoặc nhảy để chạm vào ngôi sao. Mỗi ngôi sao ăn được sẽ cộng thêm điểm (10 điểm) và phát ra âm thanh thu thập.

Điểm số: Điểm hiện tại và điểm cao nhất (High Score) được hiển thị ở góc trên bên trái màn hình.
Điểm hiện tại tăng lên khi ăn sao.

![image](https://github.com/user-attachments/assets/9957944c-2748-4aa4-8ae1-fe33362834cf)



Tạm dừng (Pause): Người chơi có thể nhấn phím ESC hoặc P (hoặc click nút Pause) để tạm dừng trò chơi. Gameplay sẽ đóng băng và một menu hiện ra cho phép tiếp tục ("Continue"), chơi lại ("New Game"), hoặc thoát về menu chính ("Exit").

![image](https://github.com/user-attachments/assets/6a8028de-c06f-4b1a-9a74-fcef4b451e2f)


Kết thúc (Game Over):
Khi Songoku va chạm với một quả cầu lửa.

Nhân vật bị hạ, phát animation bị đánh một lần và đứng yên trên mặt đất.

Nhạc nền game dừng.

Màn hình Game Over hiện ra với điểm số cuối cùng, high score, và các tùy chọn "Restart" hoặc "Main Menu".

Nếu điểm hiện tại vượt qua high score, high score mới sẽ được lưu lại.


![image](https://github.com/user-attachments/assets/e1f689de-0544-487c-826b-84790a9844e1)


# Yếu tố Thử thách:

Tốc độ và Tần suất: Tốc độ rơi và tần suất xuất hiện của quả cầu lửa có thể tăng dần theo thời gian (mặc dù hiện tại có vẻ là ngẫu nhiên cố định) để tăng độ khó.

Phản xạ: Người chơi cần phản xạ nhanh để né các quả cầu lửa xuất hiện bất ngờ.

Quản lý vị trí: Vừa phải né chướng ngại vật, vừa phải di chuyển để ăn sao đòi hỏi sự tính toán vị trí hợp lý.

Game mang lại trải nghiệm arcade cổ điển, đòi hỏi sự tập trung và kỹ năng né tránh nhanh nhẹn, kết hợp với yếu tố thu thập điểm để tạo động lực chơi lại và phá kỷ lục.

# Quá trình thiết kế game



# Ngưỡng điểm xứng đáng và các lý do bảo vệ ngưỡng điểm đó
Em đánh giá rằng game của em phù hợp với ngưỡng điểm 7.5 đến 8.5 vì em đạt 2/3 tiêu chí của barem điểm là:
  Tiêu chí (2) - Game tự viết có đồ họa đơn giản: Game có đồ họa (sprite, animation).
  Tiêu chí (3) - Game phát triển từ code mẫu nhưng có thay đổi đáng kể về nội dung và bổ sung đáng kể các tính năng mới: Em đã bổ sung nhiều tính năng như 
      1. Kiến trúc ECS: Tự xây dựng và áp dụng hoàn chỉnh.
      2. State Machine: Quản lý các trạng thái game khác nhau.
      3. UI đầy đủ: Menu chính, menu pause, game over screen, điểm số, high score (bao gồm cả lưu file).
      4. Hệ thống Animation chi tiết: Animation thay đổi theo hành động và trạng thái vật lý của nhân vật.
      5. Gameplay: Spawning kẻ địch/vật phẩm, cơ chế tính điểm, điều kiện thua.
      6. Âm thanh: Tích hợp cả nhạc nền và hiệu ứng âm thanh.
      7. Vật lý & Va chạm: Xử lý va chạm với nền đất, tường, vật phẩm, kẻ địch.
      8. Đánh giá theo số lượng tính năng tự làm/học: Game này có đủ các yếu tố được liệt kê: âm thanh, nhạc nền, texture, hoạt hình, chuyển động (vật lý), thuật toán (va chạm, spawning).


