#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <vector>
#include <sstream>
#include <windows.h>

std::map<std::string, std::vector<std::string>> sortTokensByClass(const std::vector<std::pair<std::string, std::string>>& token_pairs) {
    std::map<std::string, std::vector<std::string>> sortedTokens;

    for (const auto& pair : token_pairs) {
        sortedTokens[pair.second].push_back(pair.first);
    }

    return sortedTokens;
}
int main() {
    std::map<std::string, std::regex> patterns = {
            {"comment", std::regex(R"(#.*)")},
            {"preprocessor", std::regex(R"(^(import|from).+)")},
            {"reserved_word", std::regex(R"(\b(if|else|while|for|def|return|class|try|except|with|break|continue|and|or|not|pass|is|in|lambda|yield)\b)")},
            {"operator", std::regex(R"(\+|-|\*|/|//|%|&|\||^|<<|>>|<|>|<=|>=|==|!=|~)")},
            {"punctuation", std::regex(R"([(),:\[\]{}])")},
            {"identifier", std::regex(R"(\b([a-zA-Z_]\w*)\b)")},
            {"number_decimal", std::regex(R"(\b\d+\b)")},
            {"number_float", std::regex(R"(\b\d+\.\d+\b)")},
            {"number_hex", std::regex(R"(\b0x[a-fA-F0-9]+\b)")},
            {"string", std::regex(R"(["'].*?["'])")}
    };
    std::map<std::string, WORD> token_colors = {
            {"comment", FOREGROUND_GREEN},
            {"preprocessor", FOREGROUND_RED},
            {"reserved_word", FOREGROUND_BLUE},
            {"operator", FOREGROUND_BLUE | FOREGROUND_GREEN},
            {"punctuation", FOREGROUND_GREEN | FOREGROUND_RED},
            {"identifier", FOREGROUND_GREEN | FOREGROUND_BLUE},
            {"number_decimal", FOREGROUND_RED | FOREGROUND_BLUE},
            {"number_float", FOREGROUND_RED | FOREGROUND_GREEN},
            {"number_hex", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN},
            {"string", FOREGROUND_INTENSITY}
    };
    std::ifstream infile("python.txt");
    std::string content((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));

    std::vector<std::pair<std::string, std::string>> token_pairs;

    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        for (auto& pattern : patterns) {
            std::smatch match;
            if (std::regex_search(line, match, pattern.second)) {
                token_pairs.push_back({match[0], pattern.first});
            }
        }
    }

    auto sortedTokens = sortTokensByClass(token_pairs);

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    std::ofstream outfile("out.txt");

    for (const auto& [tokenClass, tokens] : sortedTokens) {
        for (const auto& token : tokens) {
            SetConsoleTextAttribute(handle, token_colors[tokenClass]);
            outfile << token << " ";
        }
    }
    SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    outfile << "\n\n";

    for (const auto& [tokenClass, tokens] : sortedTokens) {
        for (const auto& token : tokens) {
            outfile << "< " << token << " , " << tokenClass << " >\n";
        }
    }

    outfile.close();

    return 0;
}
