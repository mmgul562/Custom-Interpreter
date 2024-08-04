#include "lexer.h"


std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::NUMBER : return "NUMBER";
        case TokenType::IDENTIFIER : return "IDENTIFIER";
        case TokenType::STRING : return "STRING";
        case TokenType::TRUE : return "TRUE";
        case TokenType::FALSE : return "FALSE";
        case TokenType::EQUAL : return "EQUAL";
        case TokenType::NOTEQ : return "NOT EQUAL";
        case TokenType::GT : return "GREATER THAN";
        case TokenType::LT : return "LESS THAN";
        case TokenType::GTEQ : return "GREATER THAN OR EQUAL";
        case TokenType::LTEQ : return "LESS THAN OR EQUAL";
        case TokenType::NOT : return "NOT";
        case TokenType::AND : return "AND";
        case TokenType::OR : return "OR";
        case TokenType::UNDERSCORE : return "UNDERSCORE";
        case TokenType::PLUS : return "PLUS";
        case TokenType::MINUS : return "MINUS";
        case TokenType::MOD : return "MOD";
        case TokenType::ASTER : return "ASTERISK";
        case TokenType::DBL_ASTER : return "DOUBLE ASTERISK";
        case TokenType::SLASH : return "SLASH";
        case TokenType::DBL_SLASH : return "DOUBLE SLASH";
        case TokenType::ASSIGN : return "ASSIGN";
        case TokenType::IF : return "IF";
        case TokenType::ELSE : return "ELSE";
        case TokenType::THEN : return "THEN";
        case TokenType::STOP : return "STOP";
        case TokenType::SEMICOLON : return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::LBRACKET: return "L BRACKET";
        case TokenType::RBRACKET: return "R BRACKET";
        case TokenType::LPAREN : return "L PARENTHESES";
        case TokenType::RPAREN : return "R PARENTHESES";
        case TokenType::EOL : return "END OF LINE";
        case TokenType::END : return "END";
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
            if (input.substr(pos, 4) == "true" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::TRUE, Value(true));
            } else if (input.substr(pos, 5) == "false" && !std::isalnum(input[pos + 5])) {
                pos += 5;
                return Token(TokenType::FALSE, Value(false));
            } else if (input.substr(pos, 2) == "if" && !std::isalnum(input[pos + 2])) {
                pos += 2;
                return Token(TokenType::IF);
            } else if (input.substr(pos, 4) == "else" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::ELSE);
            } else if (input.substr(pos, 4) == "then" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::THEN);
            } else if (input.substr(pos, 4) == "stop" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::STOP);
            }
            return extractIdentifier();
        }
        switch (input[pos]) {
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
            case '"':
                return extractString();
            case '_':
                ++pos;
                return Token(TokenType::UNDERSCORE);
            case '+':
                ++pos;
                return Token(TokenType::PLUS);
            case '-': {
                if (isdigit(input[++pos])) return extractNumber(true);
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
            case ';': {
                ++pos;
                return Token(TokenType::SEMICOLON);
            }
            case '/': {
                if (input[++pos] == '/') {
                    ++pos;
                    return Token(TokenType::DBL_SLASH);
                }
                return Token(TokenType::SLASH);
            }
            case ',': {
                ++pos;
                return Token(TokenType::COMMA);
            }
            case '.': {
                ++pos;
                return Token(TokenType::DOT);
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
                throw LexerError(std::string("Unexpected character: '") + input[pos] + "'");
        }
    }
    return Token(TokenType::END);
}

TokenType Lexer::peekNextTokenType() {
    size_t tempPos = pos;
    Token token = getNextToken();
    pos = tempPos;
    return token.type;
}

Token Lexer::extractNumber(bool negative) {
    size_t start = pos;
    while (pos < length && (isdigit(input[pos]) || input[pos] == '.')) {
        ++pos;
    }
    double number = std::stod(input.substr(start, pos - start));
    return Token(TokenType::NUMBER, Value(negative ? -number : number));
}

Token Lexer::extractIdentifier() {
    size_t start = pos;
    while (pos < length && (isalnum(input[pos]) || input[pos] == '_')) {
        ++pos;
    }
    return Token(TokenType::IDENTIFIER, Value(input.substr(start, pos - start)));
}

Token Lexer::extractString() {
    std::string str;
    ++pos;
    while (pos < length && input[pos] != '"') {
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
        throw LexerError("Unterminated string literal");
    }
    ++pos;
    return Token(TokenType::STRING, Value(str));
}

void Lexer::reset(const std::string &newInput) {
    input = newInput;
    pos = 0;
    length = input.length();
}
