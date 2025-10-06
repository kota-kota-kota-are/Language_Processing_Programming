/**
 * @file main.c
 * @brief 字句解析器メインファイル
 * @author omzn (@omzn.aquatan.net)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan.h"

//! キーワードリスト配列
struct KEY key[KEYWORDSIZE] = {
  {"and", TAND},         {"array", TARRAY},     {"begin", TBEGIN},
  {"boolean", TBOOLEAN}, {"break", TBREAK},     {"call", TCALL},
  {"char", TCHAR},       {"div", TDIV},         {"do", TDO},
  {"else", TELSE},       {"end", TEND},         {"false", TFALSE},
  {"if", TIF},           {"integer", TINTEGER}, {"not", TNOT},
  {"of", TOF},           {"or", TOR},           {"procedure", TPROCEDURE},
  {"program", TPROGRAM}, {"read", TREAD},       {"readln", TREADLN},
  {"return", TRETURN},   {"then", TTHEN},       {"true", TTRUE},
  {"var", TVAR},         {"while", TWHILE},     {"write", TWRITE},
  {"writeln", TWRITELN}};

//! トークンカウンター
int numtoken[NUMOFTOKEN + 1];

//! 各トークンに対応する文字列
char *tokenstr[NUMOFTOKEN + 1] = {
  "",        "NAME",    "program", "var",     "array",     "of",     
  "begin",   "end",     "if",      "then",    "else",      "procedure",
  "return",  "call",    "while",   "do",      "not",       "or",
  "div",     "and",     "char",    "integer", "boolean",   "readln",
  "writeln", "true",    "false",   "NUMBER",  "STRING",    "+",
  "-",       "*",       "=",       "<>",      "<",         "<=",
  ">",       ">=",      "(",       ")",       "[",         "]",
  ":=",      ".",       ",",       ":",       ";",         "read",   
  "write",   "break"};

/**
 * @brief メイン関数
 * 
 * @param argc コマンドライン引数の数
 * @param argv 引数が格納された配列
 * @return int 正常:0 異常:非0
 */
int main(int argc, char *argv[]) {
  //! トークン, i
  int token, i;

  if (argc < 2) {
    error("File name is not given.");
    return 0;
  }
  if (init_scan(argv[1]) < 0) {
    error("Cannot open input file.");
	  end_scan();
    return 0;
  }
  /* 作成する部分：トークンカウント用の配列？を初期化する */
  while ((token = scan()) >= 0) {
    /* 作成する部分：トークンをカウントする */
  }
  end_scan();
  /* 作成する部分:カウントした結果を出力する */
  return 0;
}

/**
 * @brief エラーメッセージを表示する関数
 * 
 * @param mes メッセージ
 * @return int S_ERROR (固定)
 */
int error(char *mes) { 
	fprintf(stderr, "\nERROR: %s\n", mes); 
  return S_ERROR;
}
