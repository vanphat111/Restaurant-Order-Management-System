# Restaurant Order Management System (C++ OOP - Console)

Bài tập nhóm: Restaurant Order Management System (console/TUI), viết bằng C++ theo OOP.

## Folder
- src/      : chứa toàn bộ file .cpp và .h/.hpp 
- docs/     : tài liệu báo cáo (requirements/design/testing)
- diagrams/ : file .drawio (DFD, Use Case, Class, DB)
- assets/   : hình chụp màn hình / ảnh export từ diagrams
- build/    : file biên dịch (không commit)

## Build & Run (Linux/macOS)
    mkdir -p build
    g++ src/*.cpp -o build/app
    ./build/app

## Build & Run (Windows - MinGW g++)
    mkdir build
    g++ src\*.cpp -o build\app.exe
    build\app.exe

## Git rules
- Mỗi người làm 1 branch riêng.
- Làm xong tạo Pull Request vào main.
- Không commit build/ và file chạy (*.o, *.exe).
- Lần đầu (mỗi người làm 1 lần)

#### Clone repo
    git clone <link_repo>
    cd restaurant-order-management-system

    # Tạo branch của mình (không làm trên main)
    git checkout -b ten_cua_ban

#### Mỗi lần bắt đầu code (trước khi sửa gì)

    # Cập nhật main mới nhất
    git checkout main
    git pull origin main

    # Quay lại branch mình và nhập main vào
    git checkout ten_cua_ban
    git merge main

#### Đẩy code lên (sau khi làm xong 1 phần)

    # Kiểm tra file đã sửa
    git status

    # Add + commit
    git add .
    git commit -m "mota ngan gon"

    # Push lên GitHub
    git push -u origin ten_cua_ban # (lần đầu)
    git push # (những lần sau)

    # git push là lệnh đẩy commit từ máy lên repo trên GitHub.​
#### Nguyên tắc

- Không ai push trực tiếp lên main, chỉ merge bằng Pull Request.​

-  Làm xong 1 phần thì tạo Pull Request từ branch mình vào main để nhóm review/merge.​

## Thành viên

| No. | Name     | Student ID | Role              |Tasks                                              |
| --- | -------- | ---------- | ----------------- | ------------------------------------------------- |
| 1   | <Name 1> | <ID 1>     | Lead / Docs       | Requirements, final report, interface description |
| 2   | <Name 2> | <ID 2>     | Analyst / Design  | Use case, DFD, class diagram, update diagrams     |
| 3   | <Name 3> | <ID 3>     | Data model / Docs | Data model, DB schema, TUI screens description    |
| 4   | <Name 4> | <ID 4>     | Developer         | Implement waiter/order + unit tests               |
| 5   | <Name 5> | <ID 5>     | Git/Dev + QA      | Git workflow, merge PR, debug support, test cases |

## License

This project is licensed under the **GNU General Public License v3.0**.
See [LICENSE](https://www.gnu.org/licenses/gpl-3.0.html) for full details.
