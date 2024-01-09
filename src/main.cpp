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



std::vector<Token> tokenize(const std::string& str)
{
    std::vector<Token> tokens;
    std::string buf;
    for (int i = 0; i < str.length(); i++)
    {
       char c = str[i];
       if (std::isalpha(c))
       {
            buf.push_back(c);
            i++;
            while (std::isalnum(str[i]))
            {
                buf.push_back(str[i]);
                i++;
            }
            i--;
            if(buf == "return")
            {
                tokens.push_back({.type = TokenType::_return});
                buf.clear();
                continue;
            }
            else
            {
                std::cerr << "Unknown token: " << buf << std::endl;
                exit(EXIT_FAILURE);
            }
              
       }
       else if (std::isdigit(c))
       {
           buf.push_back(c);
           i++;
           while (std::isdigit(str[i]))
           {
               buf.push_back(str[i]);
               i++;
           }
           i--;
           tokens.push_back({.type = TokenType::int_literal, .value = buf});
           buf.clear();
       }
       else if (c == ';')
       {
           tokens.push_back({.type = TokenType::semi});
       }
       
       else if (std::isspace(c))
       {
           continue;
       }
       else
       {
           std::cerr << "Unknown token: " << c << std::endl;
           exit(EXIT_FAILURE);
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
    std::vector<Token> tokens = tokenize(contents);
    std::cout << token_to_asm(tokens) << std::endl;
    {
        std::fstream output("out.asm", std::ios::out);
        output << token_to_asm(tokens);
    }
    system("nasm -fmacho64 out.asm");
    system("ld -static -o out test.o");

    return EXIT_SUCCESS;
}