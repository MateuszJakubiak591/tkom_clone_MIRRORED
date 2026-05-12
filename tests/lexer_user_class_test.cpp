#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

TEST(UserClass, UserClassTokenization) {
   const auto tokens = tokenizeFile("tests/lexer_user_class_test.djm");

   assertToken(tokens[0], TokenType::KwClass, "class", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "User", 1, 7);
   assertToken(tokens[2], TokenType::LBrace, "{", 1, 12);
   assertToken(tokens[3], TokenType::Newline, "\\n", 1, 13);

   assertToken(tokens[4], TokenType::KwPrivate, "private", 2, 4);
   assertToken(tokens[5], TokenType::Identifier, "name", 2, 12);
   assertToken(tokens[6], TokenType::Colon, ":", 2, 16);
   assertToken(tokens[7], TokenType::KwString, "string", 2, 18);
   assertToken(tokens[8], TokenType::Newline, "\\n", 2, 24);

   assertToken(tokens[9], TokenType::KwPrivate, "private", 3, 4);
   assertToken(tokens[10], TokenType::Identifier, "age", 3, 12);
   assertToken(tokens[11], TokenType::Colon, ":", 3, 15);
   assertToken(tokens[12], TokenType::KwInt, "int", 3, 17);
   assertToken(tokens[13], TokenType::Newline, "\\n", 3, 20);

   assertToken(tokens[14], TokenType::KwPrivate, "private", 4, 4);
   assertToken(tokens[15], TokenType::Identifier, "isActive", 4, 12);
   assertToken(tokens[16], TokenType::Colon, ":", 4, 20);
   assertToken(tokens[17], TokenType::KwBool, "bool", 4, 22);
   assertToken(tokens[18], TokenType::Newline, "\\n", 4, 26);

   assertToken(tokens[19], TokenType::KwPrivate, "private", 5, 4);
   assertToken(tokens[20], TokenType::Identifier, "country", 5, 12);
   assertToken(tokens[21], TokenType::Colon, ":", 5, 19);
   assertToken(tokens[22], TokenType::KwString, "string", 5, 21);
   assertToken(tokens[23], TokenType::Newline, "\\n", 5, 27);

   assertToken(tokens[24], TokenType::Newline, "\\n", 6, 1);

   assertToken(tokens[25], TokenType::KwStatic, "static", 7, 4);
   assertToken(tokens[26], TokenType::Identifier, "userCount", 7, 11);
   assertToken(tokens[27], TokenType::Colon, ":", 7, 20);
   assertToken(tokens[28], TokenType::KwInt, "int", 7, 22);
   assertToken(tokens[29], TokenType::Assign, "=", 7, 26);
   assertToken(tokens[30], TokenType::IntLiteral, "0", 7, 28);
   assertToken(tokens[31], TokenType::Newline, "\\n", 7, 29);

   assertToken(tokens[32], TokenType::Newline, "\\n", 8, 4);

   assertToken(tokens[33], TokenType::Identifier, "User", 9, 4);
   assertToken(tokens[34], TokenType::LParen, "(", 9, 8);
   assertToken(tokens[35], TokenType::Identifier, "name", 9, 9);
   assertToken(tokens[36], TokenType::Colon, ":", 9, 13);
   assertToken(tokens[37], TokenType::KwString, "string", 9, 15);
   assertToken(tokens[38], TokenType::Comma, ",", 9, 21);
   assertToken(tokens[39], TokenType::Identifier, "age", 9, 23);
   assertToken(tokens[40], TokenType::Colon, ":", 9, 26);
   assertToken(tokens[41], TokenType::KwInt, "int", 9, 28);
   assertToken(tokens[42], TokenType::Comma, ",", 9, 31);
   assertToken(tokens[43], TokenType::Identifier, "country", 9, 33);
   assertToken(tokens[44], TokenType::Colon, ":", 9, 40);
   assertToken(tokens[45], TokenType::KwString, "string", 9, 42);
   assertToken(tokens[46], TokenType::RParen, ")", 9, 48);
   assertToken(tokens[47], TokenType::LBrace, "{", 9, 50);
   assertToken(tokens[48], TokenType::Newline, "\\n", 9, 51);

   assertToken(tokens[49], TokenType::KwThis, "this", 10, 7);
   assertToken(tokens[50], TokenType::Dot, ".", 10, 11);
   assertToken(tokens[51], TokenType::Identifier, "name", 10, 12);
   assertToken(tokens[52], TokenType::Assign, "=", 10, 17);
   assertToken(tokens[53], TokenType::Identifier, "name", 10, 19);
   assertToken(tokens[54], TokenType::Newline, "\\n", 10, 23);

   assertToken(tokens[55], TokenType::KwThis, "this", 11, 7);
   assertToken(tokens[56], TokenType::Dot, ".", 11, 11);
   assertToken(tokens[57], TokenType::Identifier, "age", 11, 12);
   assertToken(tokens[58], TokenType::Assign, "=", 11, 16);
   assertToken(tokens[59], TokenType::Identifier, "age", 11, 18);
   assertToken(tokens[60], TokenType::Newline, "\\n", 11, 21);

   assertToken(tokens[61], TokenType::KwThis, "this", 12, 7);
   assertToken(tokens[62], TokenType::Dot, ".", 12, 11);
   assertToken(tokens[63], TokenType::Identifier, "country", 12, 12);
   assertToken(tokens[64], TokenType::Assign, "=", 12, 20);
   assertToken(tokens[65], TokenType::Identifier, "country", 12, 22);
   assertToken(tokens[66], TokenType::Newline, "\\n", 12, 29);

   assertToken(tokens[67], TokenType::KwThis, "this", 13, 7);
   assertToken(tokens[68], TokenType::Dot, ".", 13, 11);
   assertToken(tokens[69], TokenType::Identifier, "isActive", 13, 12);
   assertToken(tokens[70], TokenType::Assign, "=", 13, 21);
   assertToken(tokens[71], TokenType::BoolLiteral, "true", 13, 23);
   assertToken(tokens[72], TokenType::Newline, "\\n", 13, 27);

   assertToken(tokens[73], TokenType::Identifier, "User", 14, 7);
   assertToken(tokens[74], TokenType::Dot, ".", 14, 11);
   assertToken(tokens[75], TokenType::Identifier, "userCount", 14, 12);
   assertToken(tokens[76], TokenType::Assign, "=", 14, 22);
   assertToken(tokens[77], TokenType::Identifier, "User", 14, 24);
   assertToken(tokens[78], TokenType::Dot, ".", 14, 28);
   assertToken(tokens[79], TokenType::Identifier, "userCount", 14, 29);
   assertToken(tokens[80], TokenType::Plus, "+", 14, 39);
   assertToken(tokens[81], TokenType::IntLiteral, "1", 14, 41);
   assertToken(tokens[82], TokenType::Newline, "\\n", 14, 42);

   assertToken(tokens[83], TokenType::RBrace, "}", 15, 4);
   assertToken(tokens[84], TokenType::Newline, "\\n", 15, 5);

   assertToken(tokens[85], TokenType::KwFun, "fun", 16, 4);
   assertToken(tokens[86], TokenType::Identifier, "getName", 16, 8);
   assertToken(tokens[87], TokenType::LParen, "(", 16, 15);
   assertToken(tokens[88], TokenType::RParen, ")", 16, 16);
   assertToken(tokens[89], TokenType::Arrow, "->", 16, 18);
   assertToken(tokens[90], TokenType::KwString, "string", 16, 21);
   assertToken(tokens[91], TokenType::LBrace, "{", 16, 28);
   assertToken(tokens[92], TokenType::Newline, "\\n", 16, 29);

   assertToken(tokens[93], TokenType::KwReturn, "return", 17, 7);
   assertToken(tokens[94], TokenType::KwThis, "this", 17, 14);
   assertToken(tokens[95], TokenType::Dot, ".", 17, 18);
   assertToken(tokens[96], TokenType::Identifier, "name", 17, 19);
   assertToken(tokens[97], TokenType::Newline, "\\n", 17, 23);

   assertToken(tokens[98], TokenType::RBrace, "}", 18, 4);
   assertToken(tokens[99], TokenType::Newline, "\\n", 18, 5);

   assertToken(tokens[100], TokenType::KwMut, "mut", 19, 4);
   assertToken(tokens[101], TokenType::KwFun, "fun", 19, 8);
   assertToken(tokens[102], TokenType::Identifier, "setName", 19, 12);
   assertToken(tokens[103], TokenType::LParen, "(", 19, 19);
   assertToken(tokens[104], TokenType::Identifier, "name", 19, 20);
   assertToken(tokens[105], TokenType::Colon, ":", 19, 24);
   assertToken(tokens[106], TokenType::KwString, "string", 19, 26);
   assertToken(tokens[107], TokenType::RParen, ")", 19, 32);
   assertToken(tokens[108], TokenType::Arrow, "->", 19, 34);
   assertToken(tokens[109], TokenType::KwVoid, "void", 19, 37);
   assertToken(tokens[110], TokenType::LBrace, "{", 19, 42);
   assertToken(tokens[111], TokenType::Newline, "\\n", 19, 43);

   assertToken(tokens[112], TokenType::KwThis, "this", 20, 7);
   assertToken(tokens[113], TokenType::Dot, ".", 20, 11);
   assertToken(tokens[114], TokenType::Identifier, "name", 20, 12);
   assertToken(tokens[115], TokenType::Assign, "=", 20, 17);
   assertToken(tokens[116], TokenType::Identifier, "name", 20, 19);
   assertToken(tokens[117], TokenType::Newline, "\\n", 20, 23);

   assertToken(tokens[118], TokenType::RBrace, "}", 21, 4);
   assertToken(tokens[119], TokenType::Newline, "\\n", 21, 5);

   assertToken(tokens[120], TokenType::KwFun, "fun", 22, 4);
   assertToken(tokens[121], TokenType::Identifier, "getAge", 22, 8);
   assertToken(tokens[122], TokenType::LParen, "(", 22, 14);
   assertToken(tokens[123], TokenType::RParen, ")", 22, 15);
   assertToken(tokens[124], TokenType::Arrow, "->", 22, 17);
   assertToken(tokens[125], TokenType::KwInt, "int", 22, 20);
   assertToken(tokens[126], TokenType::LBrace, "{", 22, 24);
   assertToken(tokens[127], TokenType::Newline, "\\n", 22, 25);

   assertToken(tokens[128], TokenType::KwReturn, "return", 23, 7);
   assertToken(tokens[129], TokenType::KwThis, "this", 23, 14);
   assertToken(tokens[130], TokenType::Dot, ".", 23, 18);
   assertToken(tokens[131], TokenType::Identifier, "age", 23, 19);
   assertToken(tokens[132], TokenType::Newline, "\\n", 23, 22);

   assertToken(tokens[133], TokenType::RBrace, "}", 24, 4);
   assertToken(tokens[134], TokenType::Newline, "\\n", 24, 5);

   assertToken(tokens[135], TokenType::KwMut, "mut", 25, 4);
   assertToken(tokens[136], TokenType::KwFun, "fun", 25, 8);
   assertToken(tokens[137], TokenType::Identifier, "setAge", 25, 12);
   assertToken(tokens[138], TokenType::LParen, "(", 25, 18);
   assertToken(tokens[139], TokenType::Identifier, "age", 25, 19);
   assertToken(tokens[140], TokenType::Colon, ":", 25, 22);
   assertToken(tokens[141], TokenType::KwInt, "int", 25, 24);
   assertToken(tokens[142], TokenType::RParen, ")", 25, 27);
   assertToken(tokens[143], TokenType::Arrow, "->", 25, 29);
   assertToken(tokens[144], TokenType::KwVoid, "void", 25, 32);
   assertToken(tokens[145], TokenType::LBrace, "{", 25, 37);
   assertToken(tokens[146], TokenType::Newline, "\\n", 25, 38);

   assertToken(tokens[147], TokenType::KwThis, "this", 26, 7);
   assertToken(tokens[148], TokenType::Dot, ".", 26, 11);
   assertToken(tokens[149], TokenType::Identifier, "age", 26, 12);
   assertToken(tokens[150], TokenType::Assign, "=", 26, 16);
   assertToken(tokens[151], TokenType::Identifier, "age", 26, 18);
   assertToken(tokens[152], TokenType::Newline, "\\n", 26, 21);

   assertToken(tokens[153], TokenType::RBrace, "}", 27, 4);
   assertToken(tokens[154], TokenType::Newline, "\\n", 27, 5);

   assertToken(tokens[155], TokenType::KwFun, "fun", 28, 4);
   assertToken(tokens[156], TokenType::Identifier, "isAdult", 28, 8);
   assertToken(tokens[157], TokenType::LParen, "(", 28, 15);
   assertToken(tokens[158], TokenType::RParen, ")", 28, 16);
   assertToken(tokens[159], TokenType::Arrow, "->", 28, 18);
   assertToken(tokens[160], TokenType::KwBool, "bool", 28, 21);
   assertToken(tokens[161], TokenType::LBrace, "{", 28, 26);
   assertToken(tokens[162], TokenType::Newline, "\\n", 28, 27);

   assertToken(tokens[163], TokenType::KwReturn, "return", 29, 7);
   assertToken(tokens[164], TokenType::KwThis, "this", 29, 14);
   assertToken(tokens[165], TokenType::Dot, ".", 29, 18);
   assertToken(tokens[166], TokenType::Identifier, "age", 29, 19);
   assertToken(tokens[167], TokenType::GreaterEqual, ">=", 29, 23);
   assertToken(tokens[168], TokenType::IntLiteral, "18", 29, 26);
   assertToken(tokens[169], TokenType::Newline, "\\n", 29, 28);

   assertToken(tokens[170], TokenType::RBrace, "}", 30, 4);
   assertToken(tokens[171], TokenType::Newline, "\\n", 30, 5);

   assertToken(tokens[172], TokenType::KwFun, "fun", 31, 4);
   assertToken(tokens[173], TokenType::Identifier, "getCountry", 31, 8);
   assertToken(tokens[174], TokenType::LParen, "(", 31, 18);
   assertToken(tokens[175], TokenType::RParen, ")", 31, 19);
   assertToken(tokens[176], TokenType::Arrow, "->", 31, 21);
   assertToken(tokens[177], TokenType::KwString, "string", 31, 24);
   assertToken(tokens[178], TokenType::LBrace, "{", 31, 31);
   assertToken(tokens[179], TokenType::Newline, "\\n", 31, 32);

   assertToken(tokens[180], TokenType::KwReturn, "return", 32, 7);
   assertToken(tokens[181], TokenType::KwThis, "this", 32, 14);
   assertToken(tokens[182], TokenType::Dot, ".", 32, 18);
   assertToken(tokens[183], TokenType::Identifier, "country", 32, 19);
   assertToken(tokens[184], TokenType::Newline, "\\n", 32, 26);

   assertToken(tokens[185], TokenType::RBrace, "}", 33, 4);
   assertToken(tokens[186], TokenType::Newline, "\\n", 33, 5);

   assertToken(tokens[187], TokenType::KwMut, "mut", 34, 4);
   assertToken(tokens[188], TokenType::KwFun, "fun", 34, 8);
   assertToken(tokens[189], TokenType::Identifier, "deactivate", 34, 12);
   assertToken(tokens[190], TokenType::LParen, "(", 34, 22);
   assertToken(tokens[191], TokenType::RParen, ")", 34, 23);
   assertToken(tokens[192], TokenType::Arrow, "->", 34, 25);
   assertToken(tokens[193], TokenType::KwVoid, "void", 34, 28);
   assertToken(tokens[194], TokenType::LBrace, "{", 34, 33);
   assertToken(tokens[195], TokenType::Newline, "\\n", 34, 34);

   assertToken(tokens[196], TokenType::KwThis, "this", 35, 7);
   assertToken(tokens[197], TokenType::Dot, ".", 35, 11);
   assertToken(tokens[198], TokenType::Identifier, "isActive", 35, 12);
   assertToken(tokens[199], TokenType::Assign, "=", 35, 21);
   assertToken(tokens[200], TokenType::BoolLiteral, "false", 35, 23);
   assertToken(tokens[201], TokenType::Newline, "\\n", 35, 28);

   assertToken(tokens[202], TokenType::RBrace, "}", 36, 4);
   assertToken(tokens[203], TokenType::Newline, "\\n", 36, 5);

   assertToken(tokens[204], TokenType::KwStatic, "static", 37, 4);
   assertToken(tokens[205], TokenType::KwFun, "fun", 37, 11);
   assertToken(tokens[206], TokenType::Identifier, "getUserCount", 37, 15);
   assertToken(tokens[207], TokenType::LParen, "(", 37, 27);
   assertToken(tokens[208], TokenType::RParen, ")", 37, 28);
   assertToken(tokens[209], TokenType::Arrow, "->", 37, 30);
   assertToken(tokens[210], TokenType::KwInt, "int", 37, 33);
   assertToken(tokens[211], TokenType::LBrace, "{", 37, 37);
   assertToken(tokens[212], TokenType::Newline, "\\n", 37, 38);

   assertToken(tokens[213], TokenType::KwReturn, "return", 38, 7);
   assertToken(tokens[214], TokenType::Identifier, "User", 38, 14);
   assertToken(tokens[215], TokenType::Dot, ".", 38, 18);
   assertToken(tokens[216], TokenType::Identifier, "userCount", 38, 19);
   assertToken(tokens[217], TokenType::Newline, "\\n", 38, 28);

   assertToken(tokens[218], TokenType::RBrace, "}", 39, 4);
   assertToken(tokens[219], TokenType::Newline, "\\n", 39, 5);

   assertToken(tokens[220], TokenType::KwFun, "fun", 40, 4);
   assertToken(tokens[221], TokenType::Identifier, "equals", 40, 8);
   assertToken(tokens[222], TokenType::LParen, "(", 40, 14);
   assertToken(tokens[223], TokenType::Identifier, "other", 40, 15);
   assertToken(tokens[224], TokenType::Colon, ":", 40, 20);
   assertToken(tokens[225], TokenType::Identifier, "User", 40, 22);
   assertToken(tokens[226], TokenType::RParen, ")", 40, 26);
   assertToken(tokens[227], TokenType::Arrow, "->", 40, 28);
   assertToken(tokens[228], TokenType::KwBool, "bool", 40, 31);
   assertToken(tokens[229], TokenType::LBrace, "{", 40, 36);
   assertToken(tokens[230], TokenType::Newline, "\\n", 40, 37);

   assertToken(tokens[231], TokenType::KwReturn, "return", 41, 7);
   assertToken(tokens[232], TokenType::KwThis, "this", 41, 14);
   assertToken(tokens[233], TokenType::Dot, ".", 41, 18);
   assertToken(tokens[234], TokenType::Identifier, "name", 41, 19);
   assertToken(tokens[235], TokenType::Equal, "==", 41, 24);
   assertToken(tokens[236], TokenType::Identifier, "other", 41, 27);
   assertToken(tokens[237], TokenType::Dot, ".", 41, 32);
   assertToken(tokens[238], TokenType::Identifier, "name", 41, 33);
   assertToken(tokens[239], TokenType::LogicAnd, "&&", 41, 38);
   assertToken(tokens[240], TokenType::KwThis, "this", 41, 41);
   assertToken(tokens[241], TokenType::Dot, ".", 41, 45);
   assertToken(tokens[242], TokenType::Identifier, "age", 41, 46);
   assertToken(tokens[243], TokenType::Equal, "==", 41, 50);
   assertToken(tokens[244], TokenType::Identifier, "other", 41, 53);
   assertToken(tokens[245], TokenType::Dot, ".", 41, 58);
   assertToken(tokens[246], TokenType::Identifier, "age", 41, 59);
   assertToken(tokens[247], TokenType::LogicAnd, "&&", 41, 63);
   assertToken(tokens[248], TokenType::KwThis, "this", 41, 66);
   assertToken(tokens[249], TokenType::Dot, ".", 41, 70);
   assertToken(tokens[250], TokenType::Identifier, "country", 41, 71);
   assertToken(tokens[251], TokenType::Equal, "==", 41, 79);
   assertToken(tokens[252], TokenType::Newline, "\\n", 41, 81);

   assertToken(tokens[253], TokenType::Identifier, "other", 42, 7);
   assertToken(tokens[254], TokenType::Dot, ".", 42, 12);
   assertToken(tokens[255], TokenType::Identifier, "country", 42, 13);
   assertToken(tokens[256], TokenType::Newline, "\\n", 42, 20);

   assertToken(tokens[257], TokenType::RBrace, "}", 43, 4);
   assertToken(tokens[258], TokenType::Newline, "\\n", 43, 5);

   assertToken(tokens[259], TokenType::KwFun, "fun", 44, 4);
   assertToken(tokens[260], TokenType::Identifier, "compare", 44, 8);
   assertToken(tokens[261], TokenType::LParen, "(", 44, 15);
   assertToken(tokens[262], TokenType::Identifier, "other", 44, 16);
   assertToken(tokens[263], TokenType::Colon, ":", 44, 21);
   assertToken(tokens[264], TokenType::Identifier, "User", 44, 23);
   assertToken(tokens[265], TokenType::RParen, ")", 44, 27);
   assertToken(tokens[266], TokenType::Arrow, "->", 44, 29);
   assertToken(tokens[267], TokenType::KwInt, "int", 44, 32);
   assertToken(tokens[268], TokenType::LBrace, "{", 44, 36);
   assertToken(tokens[269], TokenType::Newline, "\\n", 44, 37);

   assertToken(tokens[270], TokenType::KwReturn, "return", 45, 7);
   assertToken(tokens[271], TokenType::KwThis, "this", 45, 14);
   assertToken(tokens[272], TokenType::Dot, ".", 45, 18);
   assertToken(tokens[273], TokenType::Identifier, "age", 45, 19);
   assertToken(tokens[274], TokenType::Minus, "-", 45, 23);
   assertToken(tokens[275], TokenType::Identifier, "other", 45, 25);
   assertToken(tokens[276], TokenType::Dot, ".", 45, 30);
   assertToken(tokens[277], TokenType::Identifier, "age", 45, 31);
   assertToken(tokens[278], TokenType::Newline, "\\n", 45, 34);

   assertToken(tokens[279], TokenType::RBrace, "}", 46, 4);
   assertToken(tokens[280], TokenType::Newline, "\\n", 46, 5);

   assertToken(tokens[281], TokenType::RBrace, "}", 47, 1);
   assertToken(tokens[282], TokenType::Newline, "\\n", 47, 2);

   assertToken(tokens[283], TokenType::Newline, "\\n", 48, 1);

   assertToken(tokens[284], TokenType::KwMut, "mut", 49, 1);
   assertToken(tokens[285], TokenType::Identifier, "User", 49, 5);
   assertToken(tokens[286], TokenType::Identifier, "user", 49, 10);
   assertToken(tokens[287], TokenType::Assign, "=", 49, 15);
   assertToken(tokens[288], TokenType::Identifier, "User", 49, 17);
   assertToken(tokens[289], TokenType::LParen, "(", 49, 21);
   assertToken(tokens[290], TokenType::StringLiteral, "\"Ania\"", 49, 22);
   assertToken(tokens[291], TokenType::Comma, ",", 49, 28);
   assertToken(tokens[292], TokenType::IntLiteral, "25", 49, 30);
   assertToken(tokens[293], TokenType::Comma, ",", 49, 32);
   assertToken(tokens[294], TokenType::StringLiteral, "\"Polska\"", 49, 34);
   assertToken(tokens[295], TokenType::RParen, ")", 49, 42);
   assertToken(tokens[296], TokenType::EndOfFile, "", 49, 43);
}