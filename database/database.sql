
-- =========================
-- FILE: restaurant_db.sql
-- =========================

CREATE DATABASE IF NOT EXISTS restaurant_db;
USE restaurant_db;

CREATE TABLE User (
    UserID CHAR(10) PRIMARY KEY,
    UserName VARCHAR(15),
    Password VARCHAR(64),
    Role VARCHAR(20)
);

CREATE TABLE Admin (
    UserID CHAR(10) PRIMARY KEY,
    FOREIGN KEY (UserID) REFERENCES User(UserID)
);

CREATE TABLE Waiter (
    UserID CHAR(10) PRIMARY KEY,
    FOREIGN KEY (UserID) REFERENCES User(UserID)
);

CREATE TABLE Chef (
    UserID CHAR(10) PRIMARY KEY,
    FOREIGN KEY (UserID) REFERENCES User(UserID)
);

CREATE TABLE Tables (
    TableNumber VARCHAR(10) PRIMARY KEY,
    Section VARCHAR(20),
    Status VARCHAR(20)
);

CREATE TABLE MenuItem (
    ItemCode VARCHAR(10) PRIMARY KEY,
    ItemName VARCHAR(20),
    Price DECIMAL(10,2),
    Description TEXT,
    InventoryQty INT,
    Category VARCHAR(20)
);

CREATE TABLE Orders (
    OrderID INT AUTO_INCREMENT PRIMARY KEY,
    TableNumber VARCHAR(10),
    CreationTime DATETIME,
    TotalAmount DECIMAL(10,2),
    CookingStatus VARCHAR(20),
    FOREIGN KEY (TableNumber) REFERENCES Tables(TableNumber)
);

CREATE TABLE OrderDetail (
    OrderID INT,
    ItemCode VARCHAR(10),
    Quantity INT,
    SubTotal DECIMAL(10,2),
    PRIMARY KEY (OrderID, ItemCode),
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
    FOREIGN KEY (ItemCode) REFERENCES MenuItem(ItemCode)
);

CREATE TABLE Report (
    ReportDate DATE PRIMARY KEY,
    DailyRevenue DECIMAL(10,2),
    OrderCount INT
);

INSERT INTO User (UserID, UserName, Password, Role) VALUES
('AD01', 'admin',    '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Admin'),
('AD02', 'manager',  '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Admin'),
('WT01', 'waiter1',  '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Waiter'),
('WT02', 'waiter2',  '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Waiter'),
('WT03', 'waiter3',  '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Waiter'),
('CH01', 'chef1',    '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Chef'),
('CH02', 'chef_master', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Chef');

-- Phân quyền cụ thể
INSERT INTO Admin (UserID) VALUES ('AD01'), ('AD02');
INSERT INTO Waiter (UserID) VALUES ('WT01'), ('WT02'), ('WT03');
INSERT INTO Chef (UserID) VALUES ('CH01'), ('CH02');

-- 3. THÊM BÀN (TABLES)
-- Khu A (VIP - View đẹp), Khu B (Thường)
INSERT INTO Tables (TableNumber, Section, Status) VALUES
('T01', 'VIP_A', 'Available'),
('T02', 'VIP_A', 'Available'),
('T03', 'VIP_A', 'Occupied'), -- Đang có khách ngồi
('T04', 'VIP_A', 'Available'),
('T05', 'Main_B', 'Available'),
('T06', 'Main_B', 'Occupied'), -- Đang có khách ngồi
('T07', 'Main_B', 'Available'),
('T08', 'Main_B', 'Available'),
('T09', 'Garden_C', 'Available'),
('T10', 'Garden_C', 'Available');

-- 4. THÊM MENU (MENU ITEM) - Món ăn Việt Nam & Âu Á kết hợp
INSERT INTO MenuItem (ItemCode, ItemName, Price, Description, InventoryQty, Category) VALUES
-- Main Course (Món chính)
('F01', 'Pho Bo Dac Biet', 65000, 'Special Beef Noodle Soup', 50, 'Main'),
('F02', 'Com Tam Suon Bi', 55000, 'Broken Rice with Pork Chop', 60, 'Main'),
('F03', 'Bun Cha Ha Noi', 60000, 'Grilled Pork with Noodles', 45, 'Main'),
('F04', 'Bo Bit Tet', 120000, 'Beef Steak with Fries', 20, 'Main'),
('F05', 'My Y Sot Bo Bam', 75000, 'Spaghetti Bolognese', 30, 'Main'),

-- Side Dish (Ăn kèm)
('S01', 'Goi Cuon (3pc)', 30000, 'Fresh Spring Rolls', 100, 'Appetizer'),
('S02', 'Cha Gio (5pc)', 45000, 'Fried Spring Rolls', 80, 'Appetizer'),
('S03', 'Khoai Tay Chien', 35000, 'French Fries', 200, 'Snack'),

-- Drinks (Đồ uống)
('D01', 'Ca Phe Sua Da', 25000, 'Iced Milk Coffee', 500, 'Drink'),
('D02', 'Tra Dao Cam Sa', 35000, 'Peach Lemongrass Tea', 200, 'Drink'),
('D03', 'Sting Dau', 15000, 'Energy Drink', 1000, 'Drink'),
('D04', 'Bia Tiger', 25000, 'Tiger Beer', 500, 'Drink'),

-- Dessert (Tráng miệng)
('E01', 'Banh Flan', 15000, 'Caramel Pudding', 50, 'Dessert'),
('E02', 'Che Khuc Bach', 30000, 'Almond Panna Cotta', 40, 'Dessert');

-- 5. THÊM ĐƠN HÀNG MẪU (ORDERS)
-- Order 1: Đã hoàn thành (READY) - Bàn T03 (Hiện tại đang Occupied nhưng chưa thanh toán)
INSERT INTO Orders (TableNumber, CreationTime, TotalAmount, CookingStatus) VALUES 
('T03', NOW(), 0, 'READY'); 
-- Lưu ý: TotalAmount = 0 vì Waiter chưa bấm Request Payment để tính tổng.

-- Order 2: Đang chờ nấu (PENDING) - Bàn T06
INSERT INTO Orders (TableNumber, CreationTime, TotalAmount, CookingStatus) VALUES 
('T06', NOW(), 0, 'PENDING');

-- Order 3: Đơn cũ hôm qua (Để test báo cáo doanh thu) - Đã thanh toán xong
INSERT INTO Orders (TableNumber, CreationTime, TotalAmount, CookingStatus) VALUES 
('T01', DATE_SUB(NOW(), INTERVAL 1 DAY), 150000, 'READY');

-- 6. CHI TIẾT ĐƠN HÀNG (ORDER DETAILS)
-- Chi tiết cho Order 1 (T03): 2 Phở, 2 Cafe
INSERT INTO OrderDetail (OrderID, ItemCode, Quantity, SubTotal) VALUES
(1, 'F01', 2, 130000), -- 65k * 2
(1, 'D01', 2, 50000);  -- 25k * 2

-- Chi tiết cho Order 2 (T06): 1 Cơm tấm, 1 Sting (Đang chờ Chef nấu)
INSERT INTO OrderDetail (OrderID, ItemCode, Quantity, SubTotal) VALUES
(2, 'F02', 1, 55000),
(2, 'D03', 1, 15000);

-- Chi tiết cho Order 3 (Đơn hôm qua): 1 Bò bít tết, 1 Bia
INSERT INTO OrderDetail (OrderID, ItemCode, Quantity, SubTotal) VALUES
(3, 'F04', 1, 120000),
(3, 'D04', 1, 25000);

-- Cập nhật lại TotalAmount cho Order 3 (vì đơn này coi như đã chốt sổ)
UPDATE Orders SET TotalAmount = 145000 WHERE OrderID = 3;

-- 7. DỮ LIỆU BÁO CÁO GIẢ LẬP (REPORT)
-- Tạo 1 dòng báo cáo cho ngày hôm qua
INSERT INTO Report (ReportDate, DailyRevenue, OrderCount) VALUES
(DATE_SUB(CURDATE(), INTERVAL 1 DAY), 145000, 1);
