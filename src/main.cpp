// main.cpp
#include <iostream>
#include <string>
#include <limits>
#include "database.h"
#include "user.h"
#include "chef.h"
#include "colors.h" // Sử dụng đúng schema mày gửi

// Hàm in Logo
void printBanner() {
    // Logo tím mộng mơ (Dracula Theme)
    std::cout << COLOR_TITLE << R"(
  ____            _                              _   
 |  _ \ ___  ___ | |_  __ _ _   _ _ __ __ _ _ __| |_ 
 | |_) / _ \/ __|| __|/ _` | | | | '__/ _` | '__| __|
 |  _ <  __/\__ \| |_| (_| | |_| | | | (_| | |  | |_ 
 |_| \_\___||___/ \__|\__,_|\__,_|_|  \__,_|_|   \__|
                                                     
    )" << RESET << "\n";
    std::cout << GRAY << "======================================================" << RESET << "\n";
    std::cout << "      " << BOLD << "RESTAURANT MANAGEMENT SYSTEM" << RESET << " | " << COLOR_HIGHLIGHT << "v2.0" << RESET << "\n";
    std::cout << GRAY << "======================================================" << RESET << "\n";
}

int main() {
    try {
        // 1. Khởi tạo kết nối Database
        std::cout << CLEAR_SCREEN;
        std::cout << COLOR_TITLE << "[SYSTEM] Connecting to Database..." << RESET << "\n";
        
        DatabaseManager::getInstance(); // Singleton Init
        
        std::cout << COLOR_SUCCESS << "[SYSTEM] Database connected successfully." << RESET << "\n";
        std::cout << "Press Enter to launch...";
        std::cin.get();

        while (true) {
            std::cout << CLEAR_SCREEN;
            printBanner();

            std::cout << "\n" << COLOR_SUBTITLE << "[ MAIN MENU ]" << RESET << "\n";
            std::cout << "  " << COLOR_HIGHLIGHT << "1." << RESET << " Login System\n";
            std::cout << "  " << COLOR_HIGHLIGHT << "2." << RESET << " Recover Password (Static Key)\n";
            std::cout << "  " << COLOR_ERROR     << "0. Exit Program" << RESET << "\n";
            
            std::cout << "\n" << GRAY << "------------------------------------------------------" << RESET;
            std::cout << "\n" << COLOR_SUBTITLE << "Select option: " << RESET;

            int mainChoice;
            if (!(std::cin >> mainChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (mainChoice == 0) {
                std::cout << "\n" << COLOR_TITLE << "Goodbye! See you again." << RESET << "\n";
                break;
            }

            if (mainChoice == 1) {
                std::cout << "\n" << COLOR_SUBTITLE << ">> AUTHENTICATION" << RESET << "\n";
                std::string u, p;
                
                std::cout << "Username: "; 
                std::getline(std::cin, u);
                
                std::cout << "Password: "; 
                std::getline(std::cin, p);

                User* userPtr = User::login(u, p);

                if (userPtr != nullptr) {
                    std::cout << "\n" << COLOR_SUCCESS << "[SUCCESS] Access Granted! Loading Dashboard..." << RESET << "\n";
                    // Dừng nhẹ 1 nhịp cho user sướng
                    // std::cin.get(); 

                    // Vào Dashboard (Đa hình: Admin/Chef/Waiter tự nhận diện)
                    userPtr->showMenu(); 

                    delete userPtr;
                    // Sau khi logout sẽ quay lại vòng lặp while để hiện lại Menu chính
                } else {
                    std::cout << "\n" << COLOR_ERROR << "[FAILED] Access Denied. Invalid username or password." << RESET << "\n";
                    std::cout << "Press Enter to try again...";
                    std::cin.get();
                }
            } 
            else if (mainChoice == 2) {
                std::cout << "\n" << COLOR_SUBTITLE << ">> PASSWORD RECOVERY" << RESET << "\n";
                std::string u, newP;
                std::cout << "Target Username: "; std::getline(std::cin, u);
                std::cout << "New Password: "; std::getline(std::cin, newP);

                if (User::recoverPassword(u, newP)) {
                    std::cout << "\n" << COLOR_SUCCESS << "[SUCCESS] Password reset successfully for user: " << COLOR_HIGHLIGHT << u << RESET << "\n";
                } else {
                    std::cout << "\n" << COLOR_ERROR << "[ERROR] User not found or Database error." << RESET << "\n";
                }
                std::cout << "Press Enter to return menu...";
                std::cin.get();
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "\n" << COLOR_ERROR << "[CRITICAL SYSTEM ERROR] " << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}