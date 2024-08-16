#include "../../util/errors.h"
#include "lexer.h"


std::string getTypeName(TokenType type) {
    switch (type) {
        case TokenType::FLOAT : return "ZMIENNOPRZECINKOWA";
        case TokenType::INT : return "CAŁKOWITA";
        case TokenType::IDENTIFIER : return "IDENTYFIKATOR";
        case TokenType::STRING : return "ŁAŃCUCH";
        case TokenType::TRUE : return "PRAWDA";
        case TokenType::FALSE : return "FAŁSZ";
        case TokenType::INT_T : return "TYP CAŁKOWITY";
        case TokenType::FLOAT_T : return "TYP ZMIENNOPRZECINKOWY";
        case TokenType::STR_T : return "TYP ŁAŃCUCHOWY";
        case TokenType::BOOL_T : return "TYP LOGICZNY";
        case TokenType::QMARK : return "ZNAK ZAPYTANIA";
        case TokenType::EQUAL : return "RÓWNY";
        case TokenType::NOTEQ : return "NIE RÓWNY";
        case TokenType::GT : return "WIĘKSZY OD";
        case TokenType::LT : return "MNIEJSZY OD";
        case TokenType::GTEQ : return "WIĘKSZY LUB RÓWNY OD";
        case TokenType::LTEQ : return "MNIEJSZY LUB RÓWNY OD";
        case TokenType::NOT : return "NIE";
        case TokenType::AND : return "I";
        case TokenType::OR : return "LUB";
        case TokenType::UNDERSCORE : return "PODKREŚLNIK";
        case TokenType::PLUS : return "PLUS";
        case TokenType::MINUS : return "MINUS";
        case TokenType::MOD : return "MOD";
        case TokenType::ASTER : return "GWIAZDKA";
        case TokenType::DBL_ASTER : return "PODWÓJNA GWIAZDKA";
        case TokenType::SLASH : return "UKOŚNIK";
        case TokenType::DBL_SLASH : return "PODWÓJNY UKOŚNIK";
        case TokenType::ASSIGN : return "PRZYDZIELACZ";
        case TokenType::ASSIGN_NEW : return "NOWY PRZYDZIELACZ";
        case TokenType::IF : return "JEŻELI";
        case TokenType::ELSE : return "INACZEJ";
        case TokenType::THEN : return "WTEDY";
        case TokenType::FOR : return "DLA";
        case TokenType::IN : return "W";
        case TokenType::WHILE : return "PODCZAS GDY";
        case TokenType::DO : return "WYKONUJ";
        case TokenType::DEF : return "ZDEF";
        case TokenType::AS : return "JAKO";
        case TokenType::BREAK : return "PRZEŁAM";
        case TokenType::CONTINUE : return "KONTYNUUJ";
        case TokenType::RETURN : return "ZWRÓC";
        case TokenType::STOP : return "STOP";
        case TokenType::SEMICOLON : return "ŚREDNIK";
        case TokenType::COLON : return "DWUKROPEK";
        case TokenType::COMMA: return "PRZECINEK";
        case TokenType::DOT: return "KROPKA";
        case TokenType::DBL_DOT: return "PODWÓJNA KROPKA";
        case TokenType::LBRACE: return "L KLAMRA";
        case TokenType::RBRACE: return "P KLAMRA";
        case TokenType::LBRACKET: return "L NAWIAS KWADRATOWY";
        case TokenType::RBRACKET: return "P NAWIAS KWADRATOWY";
        case TokenType::LPAREN : return "L NAWIAS";
        case TokenType::RPAREN : return "P NAWIAS";
        case TokenType::EOL : return "KONIEC LINII";
        case TokenType::END : return "KONIEC";
        default: return "";
    }
}


Token Lexer::getNextToken() {
    while (pos < length) {
        if (isspace(input[pos])) {
            if (input[pos++] == '\n') {
                return Token(TokenType::EOL);
            }
            continue;
        }
        if (isdigit(input[pos])) {
            return extractNumber();
        }
        if (isalpha(input[pos])) {
            if (input.substr(pos, 4) == "calk" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::INT_T);
            } else if (input.substr(pos, 7) == "zmienno" && !std::isalnum(input[pos + 7])) {
                pos += 7;
                return Token(TokenType::FLOAT_T);
            } else if (input.substr(pos, 7) == "lancuch" && !std::isalnum(input[pos + 7])) {
                pos += 7;
                return Token(TokenType::STR_T);
            } else if (input.substr(pos, 8) == "logiczna" && !std::isalnum(input[pos + 8])) {
                pos += 8;
                return Token(TokenType::BOOL_T);
            } else if (input.substr(pos, 6) == "prawda" && !std::isalnum(input[pos + 6])) {
                pos += 6;
                return Token(TokenType::TRUE, Value(true));
            } else if (input.substr(pos, 5) == "falsz" && !std::isalnum(input[pos + 5])) {
                pos += 5;
                return Token(TokenType::FALSE, Value(false));
            } else if (input.substr(pos, 6) == "jezeli" && !std::isalnum(input[pos + 6])) {
                pos += 6;
                return Token(TokenType::IF);
            } else if (input.substr(pos, 7) == "inaczej" && !std::isalnum(input[pos + 7])) {
                pos += 7;
                return Token(TokenType::ELSE);
            } else if (input.substr(pos, 5) == "wtedy" && !std::isalnum(input[pos + 5])) {
                pos += 5;
                return Token(TokenType::THEN);
            } else if (input.substr(pos, 3) == "dla" && !std::isalnum(input[pos + 3])) {
                pos += 3;
                return Token(TokenType::FOR);
            } else if (input.substr(pos, 1) == "w" && !std::isalnum(input[pos + 1])) {
                pos += 1;
                return Token(TokenType::IN);
            } else if (input.substr(pos, 11) == "podczas gdy" && !std::isalnum(input[pos + 11])) {
                pos += 11;
                return Token(TokenType::WHILE);
            } else if (input.substr(pos, 7) == "wykonuj" && !std::isalnum(input[pos + 7])) {
                pos += 7;
                return Token(TokenType::DO);
            } else if (input.substr(pos, 4) == "zdef" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::DEF);
            } else if (input.substr(pos, 4) == "jako" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::AS);
            } else if (input.substr(pos, 7) == "przelam" && !std::isalnum(input[pos + 7])) {
                pos += 7;
                return Token(TokenType::BREAK);
            } else if (input.substr(pos, 9) == "kontynuuj" && !std::isalnum(input[pos + 9])) {
                pos += 9;
                return Token(TokenType::CONTINUE);
            } else if (input.substr(pos, 5) == "zwroc" && !std::isalnum(input[pos + 5])) {
                pos += 5;
                return Token(TokenType::RETURN);
            }
            else if (input.substr(pos, 4) == "stop" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::STOP);
            }
            return extractIdentifier();
        }
        switch (input[pos]) {
            case '?':
                ++pos;
                return Token(TokenType::QMARK);
            case '=':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::EQUAL);
                }
                return Token(TokenType::ASSIGN);
            case '&':
                ++pos;
                return Token(TokenType::AND);
            case '|':
                ++pos;
                return Token(TokenType::OR);
            case '!':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::NOTEQ);
                }
                return Token(TokenType::NOT);
            case '>':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::GTEQ);
                }
                return Token(TokenType::GT);
            case '<':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::LTEQ);
                }
                return Token(TokenType::LT);
            case '"': case '\'':
                return extractString();
            case '_':
                ++pos;
                return Token(TokenType::UNDERSCORE);
            case '+':
                ++pos;
                return Token(TokenType::PLUS);
            case '-': {
                ++pos;
                return Token(TokenType::MINUS);
            }
            case '%':
                ++pos;
                return Token(TokenType::MOD);
            case '*': {
                if (input[++pos] == '*') {
                    ++pos;
                    return Token(TokenType::DBL_ASTER);
                }
                return Token(TokenType::ASTER);
            }
            case '/': {
                if (input[++pos] == '/') {
                    ++pos;
                    return Token(TokenType::DBL_SLASH);
                }
                return Token(TokenType::SLASH);
            }
            case ';': {
                ++pos;
                return Token(TokenType::SEMICOLON);
            }
            case ':': {
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::ASSIGN_NEW);
                }
                return Token(TokenType::COLON);
            }
            case ',': {
                ++pos;
                return Token(TokenType::COMMA);
            }
            case '.': {
                if (input[++pos] == '.') {
                    ++pos;
                    return Token(TokenType::DBL_DOT);
                }
                return Token(TokenType::DOT);
            }
            case '{': {
                ++pos;
                return Token(TokenType::LBRACE);
            }
            case '}': {
                ++pos;
                return Token(TokenType::RBRACE);
            }
            case '[': {
                ++pos;
                return Token(TokenType::LBRACKET);
            }
            case ']': {
                ++pos;
                return Token(TokenType::RBRACKET);
            }
            case '(':
                ++pos;
                return Token(TokenType::LPAREN);
            case ')':
                ++pos;
                return Token(TokenType::RPAREN);
            default:
                throw LexerError(std::string("Nieoczekiwany znak: '") + input[pos] + "'");
        }
    }
    return Token(TokenType::END);
}


TokenType Lexer::peekNextTokenType() {
    size_t tempPos = pos;
    Token token = getNextToken();
    pos = tempPos;
    return token.getType();
}


Token Lexer::extractNumber() {
    size_t start = pos;
    bool isFloat = false;

    while (pos < length && (isdigit(input[pos]) || (input[pos] == '.' && input[pos + 1] != '.'))) {
        if (input[pos++] == '.') {
            isFloat = true;
        }
    }
    std::string numberStr = input.substr(start, pos - start);
    if (isFloat) {
        double number = std::stod(numberStr);
        return Token(TokenType::FLOAT, Value(number));
    } else {
        long number = std::stol(numberStr);
        return Token(TokenType::INT, Value(number));
    }
}


Token Lexer::extractIdentifier() {
    size_t start = pos;
    while (pos < length && (isalnum(input[pos]) || input[pos] == '_')) {
        ++pos;
    }
    return Token(TokenType::IDENTIFIER, Value(input.substr(start, pos - start)));
}


Token Lexer::extractString() {
    char quote = input[pos];
    std::string str;
    ++pos;
    while (pos < length && input[pos] != quote) {
        if (input[pos] == '\\' && pos + 1 < length) {
            switch (input[pos + 1]) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case '"': str += '"'; break;
                case '\\': str += '\\'; break;
                default: str += input[pos + 1];
            }
            pos += 2;
        } else {
            str += input[pos++];
        }
    }
    if (pos == length) {
        throw LexerError("Niezakonczony łańcuch znaków");
    }
    ++pos;
    return Token(TokenType::STRING, Value(str));
}


void Lexer::reset(const std::string &newInput) {
    input = newInput;
    pos = 0;
    length = input.length();
}
