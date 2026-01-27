// colors.h
// From Gemini with love

#pragma once
#include <string>

// Mã màu ANSI (Dracula Theme)
const std::string RESET       = "\033[0m";
const std::string BOLD        = "\033[1m";

// Màu cơ bản
const std::string RED         = "\033[38;5;203m"; // Error
const std::string GREEN       = "\033[38;5;120m"; // Success
const std::string YELLOW      = "\033[38;5;229m"; // Warning/Highlight
const std::string PURPLE      = "\033[38;5;141m"; // Title
const std::string CYAN        = "\033[38;5;117m"; // Subtitle
const std::string GRAY        = "\033[38;5;245m"; // Text phụ

// Mapping vào biến mày đang dùng trong code
const std::string COLOR_TITLE     = BOLD + PURPLE;    // Tiêu đề chính (Admin/Chef Dashboard)
const std::string COLOR_SUBTITLE  = BOLD + CYAN;      // Tiêu đề phụ (Kitchen Queue...)
const std::string COLOR_ERROR     = BOLD + RED;       // Báo lỗi/Logout
const std::string COLOR_SUCCESS   = BOLD + GREEN;     // Báo thành công
const std::string COLOR_HIGHLIGHT = BOLD + YELLOW;    // Dòng đang chọn/quan trọng
const std::string CLEAR_SCREEN    = "\033[2J\033[1;1H"; // Xóa màn hình sạch sẽ