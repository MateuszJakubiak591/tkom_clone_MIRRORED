#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"

TEST(FunctionsProgram, FunctionsProgramTokenization) {
   const auto tokens = tokenizeFile("tests/lexer_functions_test.djm");

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "DEFAULT_A", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 15);
   assertToken(tokens[3], TokenType::IntLiteral, "3", 1, 17);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 18);

   assertToken(tokens[5], TokenType::KwInt, "int", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "DEFAULT_B", 2, 5);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 15);
   assertToken(tokens[8], TokenType::IntLiteral, "7", 2, 17);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 18);

   assertToken(tokens[10], TokenType::Newline, "\\n", 3, 1);

   assertToken(tokens[11], TokenType::KwFun, "fun", 4, 1);
   assertToken(tokens[12], TokenType::Identifier, "fib", 4, 5);
   assertToken(tokens[13], TokenType::LParen, "(", 4, 8);
   assertToken(tokens[14], TokenType::Identifier, "n", 4, 9);
   assertToken(tokens[15], TokenType::Colon, ":", 4, 10);
   assertToken(tokens[16], TokenType::KwInt, "int", 4, 12);
   assertToken(tokens[17], TokenType::RParen, ")", 4, 15);
   assertToken(tokens[18], TokenType::Arrow, "->", 4, 17);
   assertToken(tokens[19], TokenType::KwInt, "int", 4, 20);
   assertToken(tokens[20], TokenType::LBrace, "{", 4, 24);
   assertToken(tokens[21], TokenType::Newline, "\\n", 4, 25);

   assertToken(tokens[22], TokenType::KwIf, "if", 5, 4);
   assertToken(tokens[23], TokenType::Identifier, "n", 5, 7);
   assertToken(tokens[24], TokenType::LessEqual, "<=", 5, 9);
   assertToken(tokens[25], TokenType::IntLiteral, "1", 5, 12);
   assertToken(tokens[26], TokenType::LBrace, "{", 5, 14);
   assertToken(tokens[27], TokenType::Newline, "\\n", 5, 15);

   assertToken(tokens[28], TokenType::KwReturn, "return", 6, 7);
   assertToken(tokens[29], TokenType::Identifier, "n", 6, 14);
   assertToken(tokens[30], TokenType::Newline, "\\n", 6, 15);

   assertToken(tokens[31], TokenType::RBrace, "}", 7, 4);
   assertToken(tokens[32], TokenType::Newline, "\\n", 7, 5);

   assertToken(tokens[33], TokenType::KwReturn, "return", 8, 4);
   assertToken(tokens[34], TokenType::Identifier, "fib", 8, 11);
   assertToken(tokens[35], TokenType::LParen, "(", 8, 14);
   assertToken(tokens[36], TokenType::Identifier, "n", 8, 15);
   assertToken(tokens[37], TokenType::Minus, "-", 8, 17);
   assertToken(tokens[38], TokenType::IntLiteral, "1", 8, 19);
   assertToken(tokens[39], TokenType::RParen, ")", 8, 20);
   assertToken(tokens[40], TokenType::Plus, "+", 8, 22);
   assertToken(tokens[41], TokenType::Identifier, "fib", 8, 24);
   assertToken(tokens[42], TokenType::LParen, "(", 8, 27);
   assertToken(tokens[43], TokenType::Identifier, "n", 8, 28);
   assertToken(tokens[44], TokenType::Minus, "-", 8, 30);
   assertToken(tokens[45], TokenType::IntLiteral, "2", 8, 32);
   assertToken(tokens[46], TokenType::RParen, ")", 8, 33);
   assertToken(tokens[47], TokenType::Newline, "\\n", 8, 34);

   assertToken(tokens[48], TokenType::RBrace, "}", 9, 1);
   assertToken(tokens[49], TokenType::Newline, "\\n", 9, 2);
   assertToken(tokens[50], TokenType::Newline, "\\n", 10, 1);

   assertToken(tokens[51], TokenType::KwFun, "fun", 11, 1);
   assertToken(tokens[52], TokenType::Identifier, "average", 11, 5);
   assertToken(tokens[53], TokenType::LParen, "(", 11, 12);
   assertToken(tokens[54], TokenType::Identifier, "a", 11, 13);
   assertToken(tokens[55], TokenType::Colon, ":", 11, 14);
   assertToken(tokens[56], TokenType::KwInt, "int", 11, 16);
   assertToken(tokens[57], TokenType::Comma, ",", 11, 19);
   assertToken(tokens[58], TokenType::Identifier, "b", 11, 21);
   assertToken(tokens[59], TokenType::Colon, ":", 11, 22);
   assertToken(tokens[60], TokenType::KwInt, "int", 11, 24);
   assertToken(tokens[61], TokenType::RParen, ")", 11, 27);
   assertToken(tokens[62], TokenType::Arrow, "->", 11, 29);
   assertToken(tokens[63], TokenType::KwFloat, "float", 11, 32);
   assertToken(tokens[64], TokenType::LBrace, "{", 11, 38);
   assertToken(tokens[65], TokenType::Newline, "\\n", 11, 39);

   assertToken(tokens[66], TokenType::KwReturn, "return", 12, 4);
   assertToken(tokens[67], TokenType::LParen, "(", 12, 11);
   assertToken(tokens[68], TokenType::Identifier, "a", 12, 12);
   assertToken(tokens[69], TokenType::Plus, "+", 12, 14);
   assertToken(tokens[70], TokenType::Identifier, "b", 12, 16);
   assertToken(tokens[71], TokenType::RParen, ")", 12, 17);
   assertToken(tokens[72], TokenType::OpAs, "as", 12, 19);
   assertToken(tokens[73], TokenType::KwFloat, "float", 12, 22);
   assertToken(tokens[74], TokenType::Divide, "/", 12, 28);
   assertToken(tokens[75], TokenType::FloatLiteral, "2.0", 12, 30);
   assertToken(tokens[76], TokenType::Newline, "\\n", 12, 33);

   assertToken(tokens[77], TokenType::RBrace, "}", 13, 1);
   assertToken(tokens[78], TokenType::Newline, "\\n", 13, 2);
   assertToken(tokens[79], TokenType::Newline, "\\n", 14, 1);

   assertToken(tokens[80], TokenType::KwFun, "fun", 15, 1);
   assertToken(tokens[81], TokenType::Identifier, "median3", 15, 5);
   assertToken(tokens[82], TokenType::LParen, "(", 15, 12);
   assertToken(tokens[83], TokenType::Identifier, "a", 15, 13);
   assertToken(tokens[84], TokenType::Colon, ":", 15, 14);
   assertToken(tokens[85], TokenType::KwInt, "int", 15, 16);
   assertToken(tokens[86], TokenType::Comma, ",", 15, 19);
   assertToken(tokens[87], TokenType::Identifier, "b", 15, 21);
   assertToken(tokens[88], TokenType::Colon, ":", 15, 22);
   assertToken(tokens[89], TokenType::KwInt, "int", 15, 24);
   assertToken(tokens[90], TokenType::Comma, ",", 15, 27);
   assertToken(tokens[91], TokenType::Identifier, "c", 15, 29);
   assertToken(tokens[92], TokenType::Colon, ":", 15, 30);
   assertToken(tokens[93], TokenType::KwInt, "int", 15, 32);
   assertToken(tokens[94], TokenType::RParen, ")", 15, 35);
   assertToken(tokens[95], TokenType::Arrow, "->", 15, 37);
   assertToken(tokens[96], TokenType::KwInt, "int", 15, 40);
   assertToken(tokens[97], TokenType::LBrace, "{", 15, 44);
   assertToken(tokens[98], TokenType::Newline, "\\n", 15, 45);

   assertToken(tokens[99], TokenType::KwIf, "if", 16, 4);
   assertToken(tokens[100], TokenType::Identifier, "a", 16, 7);
   assertToken(tokens[101], TokenType::Greater, ">", 16, 9);
   assertToken(tokens[102], TokenType::Identifier, "b", 16, 11);
   assertToken(tokens[103], TokenType::LBrace, "{", 16, 13);
   assertToken(tokens[104], TokenType::Newline, "\\n", 16, 14);

   assertToken(tokens[105], TokenType::KwIf, "if", 17, 7);
   assertToken(tokens[106], TokenType::Identifier, "b", 17, 10);
   assertToken(tokens[107], TokenType::Greater, ">", 17, 12);
   assertToken(tokens[108], TokenType::Identifier, "c", 17, 14);
   assertToken(tokens[109], TokenType::LBrace, "{", 17, 16);
   assertToken(tokens[110], TokenType::Newline, "\\n", 17, 17);

   assertToken(tokens[111], TokenType::KwReturn, "return", 18, 10);
   assertToken(tokens[112], TokenType::Identifier, "b", 18, 17);
   assertToken(tokens[113], TokenType::Newline, "\\n", 18, 18);

   assertToken(tokens[114], TokenType::RBrace, "}", 19, 7);
   assertToken(tokens[115], TokenType::Newline, "\\n", 19, 8);

   assertToken(tokens[116], TokenType::KwIf, "if", 20, 7);
   assertToken(tokens[117], TokenType::Identifier, "a", 20, 10);
   assertToken(tokens[118], TokenType::Greater, ">", 20, 12);
   assertToken(tokens[119], TokenType::Identifier, "c", 20, 14);
   assertToken(tokens[120], TokenType::LBrace, "{", 20, 16);
   assertToken(tokens[121], TokenType::Newline, "\\n", 20, 17);

   assertToken(tokens[122], TokenType::KwReturn, "return", 21, 10);
   assertToken(tokens[123], TokenType::Identifier, "c", 21, 17);
   assertToken(tokens[124], TokenType::Newline, "\\n", 21, 18);

   assertToken(tokens[125], TokenType::RBrace, "}", 22, 7);
   assertToken(tokens[126], TokenType::Newline, "\\n", 22, 8);

   assertToken(tokens[127], TokenType::KwReturn, "return", 23, 7);
   assertToken(tokens[128], TokenType::Identifier, "a", 23, 14);
   assertToken(tokens[129], TokenType::Newline, "\\n", 23, 15);

   assertToken(tokens[130], TokenType::RBrace, "}", 24, 4);
   assertToken(tokens[131], TokenType::Newline, "\\n", 24, 5);

   assertToken(tokens[132], TokenType::KwIf, "if", 25, 4);
   assertToken(tokens[133], TokenType::Identifier, "a", 25, 7);
   assertToken(tokens[134], TokenType::Greater, ">", 25, 9);
   assertToken(tokens[135], TokenType::Identifier, "c", 25, 11);
   assertToken(tokens[136], TokenType::LBrace, "{", 25, 13);
   assertToken(tokens[137], TokenType::Newline, "\\n", 25, 14);

   assertToken(tokens[138], TokenType::KwReturn, "return", 26, 7);
   assertToken(tokens[139], TokenType::Identifier, "a", 26, 14);
   assertToken(tokens[140], TokenType::Newline, "\\n", 26, 15);

   assertToken(tokens[141], TokenType::RBrace, "}", 27, 4);
   assertToken(tokens[142], TokenType::Newline, "\\n", 27, 5);

   assertToken(tokens[143], TokenType::KwIf, "if", 28, 4);
   assertToken(tokens[144], TokenType::Identifier, "b", 28, 7);
   assertToken(tokens[145], TokenType::Greater, ">", 28, 9);
   assertToken(tokens[146], TokenType::Identifier, "c", 28, 11);
   assertToken(tokens[147], TokenType::LBrace, "{", 28, 13);
   assertToken(tokens[148], TokenType::Newline, "\\n", 28, 14);

   assertToken(tokens[149], TokenType::KwReturn, "return", 29, 7);
   assertToken(tokens[150], TokenType::Identifier, "c", 29, 14);
   assertToken(tokens[151], TokenType::Newline, "\\n", 29, 15);

   assertToken(tokens[152], TokenType::RBrace, "}", 30, 4);
   assertToken(tokens[153], TokenType::Newline, "\\n", 30, 5);

   assertToken(tokens[154], TokenType::KwReturn, "return", 31, 4);
   assertToken(tokens[155], TokenType::Identifier, "b", 31, 11);
   assertToken(tokens[156], TokenType::Newline, "\\n", 31, 12);

   assertToken(tokens[157], TokenType::RBrace, "}", 32, 1);
   assertToken(tokens[158], TokenType::Newline, "\\n", 32, 2);
   assertToken(tokens[159], TokenType::Newline, "\\n", 33, 1);

   assertToken(tokens[160], TokenType::KwFun, "fun", 34, 1);
   assertToken(tokens[161], TokenType::Identifier, "main", 34, 5);
   assertToken(tokens[162], TokenType::LParen, "(", 34, 9);
   assertToken(tokens[163], TokenType::RParen, ")", 34, 10);
   assertToken(tokens[164], TokenType::Arrow, "->", 34, 12);
   assertToken(tokens[165], TokenType::KwInt, "int", 34, 15);
   assertToken(tokens[166], TokenType::LBrace, "{", 34, 19);
   assertToken(tokens[167], TokenType::Newline, "\\n", 34, 20);

   assertToken(tokens[168], TokenType::KwInt, "int", 35, 4);
   assertToken(tokens[169], TokenType::Identifier, "m", 35, 8);
   assertToken(tokens[170], TokenType::Assign, "=", 35, 10);
   assertToken(tokens[171], TokenType::Identifier, "median3", 35, 12);
   assertToken(tokens[172], TokenType::LParen, "(", 35, 19);
   assertToken(tokens[173], TokenType::Identifier, "DEFAULT_A", 35, 20);
   assertToken(tokens[174], TokenType::Comma, ",", 35, 29);
   assertToken(tokens[175], TokenType::Identifier, "DEFAULT_B", 35, 31);
   assertToken(tokens[176], TokenType::Comma, ",", 35, 40);
   assertToken(tokens[177], TokenType::Identifier, "fib", 35, 42);
   assertToken(tokens[178], TokenType::LParen, "(", 35, 45);
   assertToken(tokens[179], TokenType::IntLiteral, "5", 35, 46);
   assertToken(tokens[180], TokenType::RParen, ")", 35, 47);
   assertToken(tokens[181], TokenType::RParen, ")", 35, 48);
   assertToken(tokens[182], TokenType::Newline, "\\n", 35, 49);

   assertToken(tokens[183], TokenType::KwReturn, "return", 36, 4);
   assertToken(tokens[184], TokenType::Identifier, "m", 36, 11);
   assertToken(tokens[185], TokenType::Newline, "\\n", 36, 12);

   assertToken(tokens[186], TokenType::RBrace, "}", 37, 1);
   assertToken(tokens[187], TokenType::EndOfFile, "", 37, 2);
}