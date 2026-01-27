# 🍽️ Restaurant Order Management System

![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B)
![MySQL](https://img.shields.io/badge/Database-MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white)
![Docker](https://img.shields.io/badge/Deployment-Docker-2496ED?style=for-the-badge&logo=docker&logoColor=white)

> A comprehensive Terminal-based Console Application designed to streamline restaurant operations. Developed as a final project for the **Software Engineering** course at UTH (Ho Chi Minh City University of Transport).

## 📋 Table of Contents
- [About the Project](#-about-the-project)
- [Team Members](#-team-members)
- [Key Features](#-key-features)
- [Tech Stack](#-tech-stack)
- [System Architecture](#-system-architecture)
- [Installation & Usage (Docker)](#-installation--usage-docker)
- [Screenshots](#-screenshots)

## 📖 About the Project
This project aims to digitize the manual ordering process in restaurants. It features a robust **Terminal User Interface (TUI)** with a modern **Dracula theme**, ensuring a pleasant user experience for staff working in low-light environments. The system supports Role-Based Access Control (RBAC) to separate duties between Admins, Chefs, and Waiters.

## 👥 Team Members
We are a team of 5 students from UTH passionate about Software Engineering and DevOps.

| No. | Student Name | GitHub |
| :-: | :--- | :--- |
| 1 | **Hồ Văn Phát** | [@vanphat111](https://github.com/vanphat111) |
| 2 | **Đoàn Phú Trọng** |  [@trongDP-lead-git](https://github.com/trongDP-lead-git) |
| 3 | **Lê Thanh Nhị** | [@thanhnhi-gif](https://github.com/thanhnhi-gif) |
| 4 | **Thông Hoàn Thái** | [@hoanthai06](https://github.com/hoanthai06) |
| 5 | **Phan Trần Quang Lâm** | [@quanglam061206-glitch](https://github.com/quanglam061206-glitch) |

## ✨ Key Features

### 🛡️ Admin
- **User Management**: Add, remove, and update staff roles (Chef, Waiter).
- **Menu Management**: CRUD operations for menu items (adjust prices, inventory).
- **Reporting**: View daily revenue, best-selling items, and generate financial reports.

### 🍳 Chef
- **Kitchen Queue**: Real-time view of pending orders sorted by time.
- **Workflow Control**: Update order status from `PENDING` to `READY`.
- **Cooking Dashboard**: Highlighted interface for urgent orders.

### 💁 Waiter
- **Table Management**: Visual map of Available/Occupied tables.
- **Order Processing**: Create orders, add/remove items, and modify quantities.
- **Billing**: Calculate totals and process payments (releases table automatically).

## 🛠 Tech Stack
- **Language**: C++ (Standard 17)
- **Database**: MySQL 8.0
- **Library**: `mysql-connector-c++`
- **Containerization**: Docker & Docker Compose
- **Design Pattern**: Singleton (DB Connection), MVC Architecture.


## 🚀 Installation & Usage (Docker)

This project is fully containerized. You can run both the Database and the Client Application with a single command, ensuring a consistent environment without installing C++ compilers or MySQL manually.

### Prerequisites
- [Docker Desktop](https://www.docker.com/products/docker-desktop/) or Docker Engine.
- Git.

### Step-by-Step Guide

1.  **Clone the Repository**
    ```bash
    git clone [https://github.com/vanphat111/Restaurant-Order-Management-System.git](https://github.com/vanphat111/Restaurant-Order-Management-System.git)
    cd Restaurant-Order-Management-System
    ```

2.  **Run with Docker Compose**
    This command will download MySQL, build the C++ application, and link them together in a private network.
    ```bash
    docker-compose up -d --build
    ```
    *(Wait a few seconds for the Database to initialize)*.

3.  **Launch the Application**
    Since this is an interactive Console Application (TUI), you need to attach to the running container:
    ```bash
    docker attach restaurant_client
    ```

4.  **Stop the Application**
    To exit the app, select `0. Exit` in the menu. To stop and remove the containers:
    ```bash
    docker-compose down
    ```

### ⚠️ Troubleshooting
- **Database Connection Error**: If you see a connection error immediately after attaching, the MySQL container might still be booting up. Wait 10 seconds and try running the app again inside the container:
  ```bash
  # Inside the container shell if the app crashed
  ./restaurant_app