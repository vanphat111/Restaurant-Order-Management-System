
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

INSERT INTO User VALUES
('U01','admin1','123456','Admin'),
('U02','waiter1','123456','Waiter'),
('U03','chef1','123456','Chef');

INSERT INTO Admin VALUES ('U01');
INSERT INTO Waiter VALUES ('U02');
INSERT INTO Chef VALUES ('U03');

INSERT INTO Tables VALUES
('T01','A','Available'),
('T02','B','Occupied');

INSERT INTO MenuItem VALUES
('M01','Fried Rice',50000,'Vietnamese fried rice',50,'Main'),
('M02','Coffee',30000,'Black coffee',100,'Drink');
