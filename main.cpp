#include <iostream>
#include <regex>
#include <fstream>
#include <cstring>

void lexRegexAnalyze(const std::string &py, std::ofstream &outfile) {
    std::regex whitespaceRegex("(\\s+)");
    std::regex keywordRegex("\\b(function|var|return|if|else|while|for)\\b");
    std::regex identifierRegex("\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b");
    std::regex dotRegex("\\.");
    std::regex numericLiteralRegex("\\b\\d+\\.?\\d*([eE][-+]?\\d+)?\\b");
    std::regex hexLiteralRegex("\\b0x[0-9a-fA-F]+\\b");
    std::regex stringLiteralRegex("(\"(.*?)\")");
    std::regex charLiteralRegex("'.'");
    std::regex preprocessorDirectiveRegex("#\\w+");
    std::regex commentRegex("//.*|/\\*.*?\\*/|#.*");
    std::regex operatorRegex("(==|!=|<=|>=|&&|\\|\\||\\+\\+|--|\\+|-|\\*|/|%|&|\\||\\^|~|<<|>>|!|=)");
    std::regex punctuationRegex("(\\{|\\}|\\(|\\)|\\[|\\]|;|,|:)");
    std::regex errorRegex("[^a-zA-Z0-9_+=\\-*/%&|!^<>=~#\"'().,{};\\s`\\[\\]\\$\\:\\?\\\\]");

    std::string remainingCode = py;

    while (!remainingCode.empty()) {
        std::smatch match;
        bool found = false;

        std::vector<std::pair<std::regex, std::string>> patterns = {
                {errorRegex, "Unrecognized token"},
                {whitespaceRegex, "Whitespace"},
                {keywordRegex, "Keyword"},
                {identifierRegex, "Identifier"},
                {numericLiteralRegex, "Numeric Literal"},
                {hexLiteralRegex, "Hexadecimal Literal"},
                {stringLiteralRegex, "String Literal"},
                {charLiteralRegex, "Char Literal"},
                {preprocessorDirectiveRegex, "Preprocessor directive"},
                {commentRegex, "Comment"},
                {operatorRegex, "Operator"},
                {punctuationRegex, "Punctuation"},
                {dotRegex, "Dot Operator"}
        };

        for (const auto& [regex, type] : patterns) {
            if (std::regex_search(remainingCode, match, regex)) {
                if (match.position() == 0) {
                    if (type != "Whitespace") {
                        outfile << match[0] << " - " << type << "\n";
                    }
                    remainingCode = match.suffix().str();
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            std::cerr << "Unrecognized sequence: " << remainingCode << "\n";
            break;
        }
    }
}

bool tryOpenFile(std::ifstream& file, const std::string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file '" << filename << "'.\n";
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool getUserConsent() {
    std::string choice;
    std::cout << "Would you like to retry? (yes/no): ";
    std::cin >> choice;
    return choice == "yes";
}

int main() {
    const int retryLimit = 3;
    std::string filename;
    std::ifstream file;

    for (int attempts = 0; attempts < retryLimit; ++attempts) {
        if (attempts > 0 && !getUserConsent()) {
            std::cerr << "Exiting program.\n";
            return 1;
        }

        std::cout << "Enter a filename: ";
        std::cin >> filename;

        if (tryOpenFile(file, filename)) {
            break;
        }
    }
    if (!file.is_open()) {
        std::cerr << "Exceeded maximum retries. Exiting program.\n";
        return 1;
    }

    std::string py((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::ofstream outfile("out.txt");

    lexRegexAnalyze(py, outfile);

    outfile.close();

    return 0;
}

