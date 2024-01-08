#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<optional>


enum class TokenType{
    _return,
    int_literal,
    semi
};

struct Token{
    TokenType type;
    std::optional<std::string> value;
};



std::vector<Token> lex(std::string contents)
{
    std::vector<Token> tokens;
    std::string current_token;
    for (char c : contents)
    {

        if (c == ' ')
        {
            if (current_token == "return")
            {
                tokens.push_back(Token{TokenType::_return});
            }
            else if (current_token == ";")
            {
                tokens.push_back(Token{TokenType::semi});
            }
            else
            {
                tokens.push_back(Token{TokenType::int_literal, current_token});
            }
            current_token = "";
        }
        else
        {
            current_token += c;
        }
    }
    return tokens;
}

std::string token_to_asm(const std::vector<Token>& tokens)
{
    std::stringstream output;
    output << "global start\nstart:\n";
    for (int i = 0; i < tokens.size(); i++)
    {
        const Token& token = tokens[i];
        if (token.type == TokenType::_return)
        {
           if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::int_literal)
           {
            if(i+2 < tokens.size() && tokens[i+2].type == TokenType::semi)
            {
               output << "  mov eax, 0x2000001\n"; // syscall number for exit
               output << "  mov edi, " + tokens[i+1].value.value() + "\n"; // exit code
               output << "  syscall";
            }
            else
            {
                std::cerr << "Expected semicolon after return statement" << std::endl;
                exit(EXIT_FAILURE);
            }
           }
           
        }
    }
    return output.str();
}



int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage incorrect" << std::endl;
        std::cerr << "olang <input.ol>" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string contents;
    { 
        std::stringstream content_stream;
        std::fstream input(argv[1],std::ios::in);
        content_stream << input.rdbuf();
        contents = content_stream.str();
    }
    std::vector<Token> tokens = lex(contents);
    std::cout << token_to_asm(tokens) << std::endl;

    return EXIT_SUCCESS;
}