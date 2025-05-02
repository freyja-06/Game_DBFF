# Về Source code của game
1. main.cpp - File này đóng vai trò là điểm khởi đầu của toàn bộ chương trình
   * Khởi tạo các thư viện cần thiết (SDL, SDL_image, SDL_ttf, SDL_mixer thông qua AudioManager).
   * Tạo đối tượng Game chính.
   * Gọi hàm init() của Game để thiết lập cửa sổ, renderer, tải tài nguyên ban đầu.
   * Chạy vòng lặp game chính : liên tục gọi handleEvents(), update(), render() của đối tượng Game.
   * Quản lý tốc độ khung hình (FPS) để game chạy ổn định.
   * Khi vòng lặp kết thúc, gọi hàm clean() của Game để giải phóng tài nguyên và dọn dẹp trước khi kết thúc chương trình.

2. Game.hpp & Game.cpp - Lớp trung tâm điều khiển toàn bộ luồng hoạt động và logic của game.
   - Game.hpp:
     * Khai báo lớp Game
     * Định nghĩa enum GameState (Trạng thái game)
     * Khai báo các biến thành viên (trạng thái hiện tại, con trỏ tới các entity quan trọng như player, UI, background, điểm số, high score, các timer, font chữ, đối tượng quản lý ECS manager, cửa sổ window, renderer renderer...), và các phương thức chính (init, handleEvents, update, render, clean, các hàm quản lý trạng thái, tạo/hủy UI, xử lý logic game như va chạm, sinh đối tượng).
   - Game.cpp: Triển khai (implement) các phương thức đã khai báo trong Game.hpp. Đây là nơi chứa hầu hết logic cốt lõi của game:
       * Quản lý các trạng thái game (MainMenu, Playing, Paused, GameOver) và chuyển đổi giữa chúng.
       * Xử lý sự kiện đầu vào cấp cao (nhấn phím để pause/jump, click chuột vào nút UI).
       * Điều phối việc cập nhật trạng thái của tất cả các entity thông qua manager.update().
       * Thực hiện kiểm tra va chạm giữa các đối tượng (player vs kẻ địch, player vs vật phẩm, player vs nền đất) và xử lý kết quả.
       * Quản lý việc sinh (spawn) các đối tượng như kẻ địch (fireball), vật phẩm (star).
       * Cập nhật điểm số, high score (bao gồm đọc/ghi file).
       * Điều phối việc vẽ (render) mọi thứ lên màn hình dựa trên trạng thái game hiện tại.
       * Tạo và hủy các thành phần giao diện người dùng (UI) tương ứng với từng trạng thái game.
       * Tải tài nguyên (âm thanh, font...).
    
3. ECS.h - Định nghĩa và triển khai kiến trúc Entity-Component-System (ECS).
   - ECS.h:
       * Định nghĩa lớp cơ sở Component.
       * Định nghĩa lớp Entity (đại diện cho một đối tượng trong game, chứa một tập hợp các Component).
       * Định nghĩa lớp Manager (quản lý vòng đời và tập hợp tất cả các Entity).
       * Cung cấp các cơ chế để thêm/lấy/kiểm tra component trong entity, cập nhật và vẽ các entity/component.
   - Mục đích: Tách biệt dữ liệu (Components) và hành vi (Systems - trong game này chủ yếu nằm ở Game::update và các hàm update của Component) giúp code linh hoạt, dễ mở rộng và quản lý.

4. Components.h:
  - Vai trò: Một file header tổng hợp, giúp gom nhóm việc include các file component khác nhau và định nghĩa một số component đơn giản.
  - Nhiệm vụ:
      * include tất cả các file header của các Component cụ thể (Transform, Sprite, Keyboard, Collider, Text).
      * Định nghĩa ButtonComponent: Chứa logic cơ bản cho một nút bấm UI, quan trọng nhất là std::function<void()> onClick để lưu trữ hành động sẽ thực hiện khi nút được nhấn.

5. TransformComponent.h:
  - Vai trò: Lưu trữ thông tin về vị trí, kích thước, tỉ lệ và trạng thái vật lý cơ bản của một Entity.
  - Nhiệm vụ: Chứa các biến như position (Vector2D), velocity (Vector2D), width, height, scale, và các thuộc tính vật lý như gravity, jumpForce, speed, isGrounded. Hàm update() của nó áp dụng vận tốc và trọng lực để thay đổi vị trí theo thời gian (dt).

6. SpriteComponent.h:
  - Vai trò: Chịu trách nhiệm hiển thị hình ảnh (sprite) cho một Entity.
  - Nhiệm vụ:
    * Giữ một SDL_Texture là hình ảnh của entity.
    * Quản lý srcRect (phần hình ảnh nguồn sẽ vẽ) và destRect (vị trí và kích thước vẽ lên màn hình).
    * Xử lý hoạt ảnh (animation) bằng cách thay đổi srcRect theo thời gian dựa trên dữ liệu từ các Animation struct (lưu trong map animation) và trạng thái của nhân vật (UpdateAnimationState).
    * Tương tác với TransformComponent để lấy vị trí/kích thước vẽ (destRect).
    * Sử dụng TextureManager để vẽ texture lên renderer.

7. KeyboardController.h:
  - Vai trò: Xử lý đầu vào từ bàn phím để điều khiển một Entity (thường là người chơi).
  - Nhiệm vụ: Đọc trạng thái các phím (trái, phải, nhảy), cập nhật velocity của TransformComponent và spriteFlip của SpriteComponent tương ứng. Cung cấp hàm TryJump() để thực hiện hành động nhảy.

8. ColliderComponent.h:
  - Vai trò: Định nghĩa vùng va chạm vật lý cho một Entity.
  - Nhiệm vụ: Chứa một SDL_Rect (collider) đại diện cho hình dạng va chạm và một tag (chuỗi ký tự) để xác định loại đối tượng va chạm (vd: "player", "ground", "fireball"). Hàm update() của nó đồng bộ vị trí/kích thước của collider với TransformComponent.
    
9. TextComponent.h:
  - Vai trò: Hiển thị văn bản trên màn hình.
  - Nhiệm vụ: Sử dụng thư viện SDL_ttf để render một chuỗi ký tự (text) với font và màu sắc (font, color) đã cho thành một SDL_Texture. Sau đó vẽ texture này lên màn hình tại vị trí xác định bởi TransformComponent. Có cơ chế needsUpdate để chỉ render lại text thành texture khi nội dung text thay đổi.
    
10. AudioManager.h & AudioManager.cpp:
  - Vai trò: Lớp quản lý tập trung cho việc tải và phát âm thanh (nhạc nền và hiệu ứng).
  - Nhiệm vụ: Cung cấp giao diện tĩnh (static) để dễ dàng LoadMusic, LoadChunk, PlayMusic, PlayChunk, điều chỉnh âm lượng... mà không cần trực tiếp tương tác với các hàm SDL_mixer ở nhiều nơi trong code.
    
11. TextureManager.h & TextureManager.cpp:
  - Vai trò: Lớp tiện ích tĩnh (static) để đơn giản hóa việc tải và vẽ SDL_Texture.
  - Nhiệm vụ: Cung cấp hàm LoadTexture để tải ảnh từ file và Draw để vẽ texture lên renderer, ẩn đi chi tiết triển khai của SDL_image và SDL_RenderCopyEx.
    
12. Collision.h & Collision.cpp:
  - Vai trò: Lớp tiện ích tĩnh (static) chứa các thuật toán kiểm tra va chạm.
  - Nhiệm vụ: Cung cấp hàm AABB để kiểm tra va chạm giữa hai hình chữ nhật (phương pháp kiểm tra va chạm phổ biến và đơn giản) và PointInRect để kiểm tra xem một điểm (như vị trí chuột) có nằm trong hình chữ nhật (như nút bấm) hay không.
    
13. Vector2D.h & Vector2D.cpp:
Vai trò: Định nghĩa một cấu trúc dữ liệu cơ bản cho vector 2 chiều.
Nhiệm vụ: Cung cấp một lớp đơn giản để lưu trữ và thao tác với các giá trị tọa độ x, y kiểu float, thường được dùng cho vị trí và vận tốc.

14. Animation.h:
  - Vai trò: Định nghĩa cấu trúc dữ liệu để lưu thông tin về một hoạt ảnh cụ thể.
  - Nhiệm vụ: Chứa các thông số cần thiết cho một chuỗi hoạt ảnh như chỉ số hàng trên spritesheet (index), số lượng khung hình (frames), tốc độ phát (speed), và cờ cho biết có lặp lại (loop) hay không. Được SpriteComponent sử dụng.

# Đồ họa
1. Background, chướng ngại vật, nút bấm: Lấy nguồn từ game Ngọc Rồng Online: https://ngocrongonline.com/gioi-thieu
    ![image](https://github.com/user-attachments/assets/c052a12c-c582-4c64-bb68-ea4c5c986345)

    Background: Sử dụng SnapEdit(https://snapedit.app/vi) : Xóa đi chữ và các nút ở giữa, đồng thời thêm một số cây để background game đẹp hơn
    Chướng ngại vật: Là nền đất nhưng được chỉnh sửa kích thước kéo dài hơn bằng paint.
    Nút bấm: Cũng được cắt từ ảnh trên và xóa + chỉnh lại chữ bằng SnapEdit 

2. Font chữ: Sử dụng font chung là vni souvenir: https://www.wfonts.com/font/vni-souvir

2. Animation
  Nguồn ảnh đểtạo SpriteSheet: https://www.deviantart.com/blacksanyt/art/Goku-SaiyanSaga-SwlPlus-Sprite-Sheet-900031739 

  Animation được tạo ra bằng cách chạy 1 file SpriteSheet với mỗi khung hình phải có kích thước bằng nhau như sau
  ![image](https://github.com/user-attachments/assets/74b64db6-22f5-4666-8625-7e44f171ce5a)

  Cách thức để tạo SpriteSheet để chạy được: Sử dụng phần mềm GIMP 3: https://www.gimp.org/downloads/

  Bước 1: Cắt ảnh, xóa phông đỏ trên web https://www.remove.bg/vi 
![image](https://github.com/user-attachments/assets/812fec51-3cde-4044-99de-2e7f5378f579)
![image](https://github.com/user-attachments/assets/5ff51342-5788-45dc-8641-079cf2aa5a47)


  Bước 2: Vào gimp, tạo ảnh mới với kích thước tùy ý, miễn đủ to để chứa đầy đủ các animation
  
  ![image](https://github.com/user-attachments/assets/cbbe9344-58c3-4330-943b-1483370ea800)

  Bước 3: Tạo grid để thêm ani Vào image -> configure grid -> chỉnh sửa theo kích thước nhân vật(100X113 trong game)->nhấn ok
![image](https://github.com/user-attachments/assets/2a7fd78c-0a6f-4a0e-8252-eb724061822f)

  Bước 4: View -> tích ShowGrid.

![image](https://github.com/user-attachments/assets/37f469d7-9a46-4d07-9294-1115c35470f0)

![image](https://github.com/user-attachments/assets/c09ff5bf-aef8-4ed8-99c6-2cb518bc1916)

  Bước 5: Xóa background hiện có: Chuột trái trên chữ background, chọn Delete layer
![image](https://github.com/user-attachments/assets/d3984085-0711-4f82-92fa-3ec0c9a13060)

![image](https://github.com/user-attachments/assets/fde5c00f-6b37-4fcf-a7f8-3ff6aff769af)

  Bước 6: Mở file vừa thu được tại bước 1 bằng gimp, file sẽ được mở cùng 1 cửa sổ với file vừa tạo
![image](https://github.com/user-attachments/assets/f95b9bfc-120a-4440-bd61-174f125a9c3a)

  Bước 7: Copy từng Sheet từ file mẫu sang file cần tạo, sau đó lưu file.
![image](https://github.com/user-attachments/assets/1fad28ab-4e4f-40eb-9df3-58a31dc3ceca)

Trong quá trình này, ta tự tạo các sheet sao cho giống như nhân vật đang chuyển động 

Như vậy, ta đã thu được file SpriteSheet, sử dụng để thực hiện animation cho game

3. Âm thanh
   Nhạc nền menu: https://www.youtube.com/watch?v=xkcUFEfbrQc (được tua 1.5)
   
   Nhạc hiệu ứng bị đánh trúng: https://mixkit.co/free-sound-effects/explosion/
   
   Nhạc hiệu ứng nhảy: https://uppbeat.io/sfx/category/gaming/video-game/arcade-jump
   
   Nhạc hiệu ứng ăn sao: https://www.youtube.com/watch?v=mQSmVZU5EL4
































