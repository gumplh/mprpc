#pragma once
#include <string>

void eraseFrontAndBackSpace(std::string &str);

int parseConfig(const std::string &src_buf, std::string &key, std::string &value, const char &splitChar);

void eraseLineFeed(std::string &str);