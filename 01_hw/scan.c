/**
 * @file scan.c
 * @brief 字句解析器の実装
 * @version 0.1
 * @date 2025-02-26
 */

#include "scan.h"

/* グローバル変数 */
static FILE *fp;                      // 入力ファイルポインタ
static int cbuf;                      // 1文字先読みバッファ
static int linenum;                   // 現在の行番号
int num_attr;                         // 数値トークンの属性値
char string_attr[MAXSTRSIZE];         // 文字列・名前トークンの属性値

/* 内部ヘルパー関数のプロトタイプ宣言 */
static int is_letter(int c);
static int is_digit(int c);
static void next_char(void);
static void process_newline(void);
static int scan_name(void);
static int scan_number(void);
static int scan_string(void);
static int skip_comment_brace(void);
static int skip_comment_slash(void);
static int lookup_keyword(void);

/**
 * @brief アルファベット判定
 * @param c 判定する文字
 * @return アルファベットなら1、そうでなければ0
 */
static int is_letter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief 数字判定
 * @param c 判定する文字
 * @return 数字なら1、そうでなければ0
 */
static int is_digit(int c) {
    return (c >= '0' && c <= '9');
}

/**
 * @brief 次の文字をcbufに読み込む
 */
static void next_char(void) {
    cbuf = fgetc(fp);
}

/**
 * @brief 改行を処理する（4パターンに対応）
 */
static void process_newline(void) {
    if (cbuf == '\n') {
        next_char();
        /* \n\r の処理 */
        if (cbuf == '\r') {
            next_char();
        }
        linenum++;
    } else if (cbuf == '\r') {
        next_char();
        /* \r\n の処理 */
        if (cbuf == '\n') {
            next_char();
        }
        linenum++;
    }
}

/**
 * @brief キーワード検索
 * @return キーワードなら対応するトークン番号、そうでなければTNAME
 */
static int lookup_keyword(void) {
    int i;
    for (i = 0; i < KEYWORDSIZE; i++) {
        if (strcmp(string_attr, key[i].keyword) == 0) {
            return key[i].keytoken;
        }
    }
    return TNAME;
}

/**
 * @brief 名前の読み取り
 * @return トークン番号（TNAME またはキーワード）、エラー時はS_ERROR
 */
static int scan_name(void) {
    int i = 0;

    /* 最初の文字（アルファベット）を格納 */
    string_attr[i++] = cbuf;
    next_char();

    /* 英数字が続く限り読み取る */
    while (is_letter(cbuf) || is_digit(cbuf)) {
        if (i >= MAXSTRSIZE - 1) {
            /* バッファオーバーフロー：残りの文字を読み飛ばす */
            while (is_letter(cbuf) || is_digit(cbuf)) {
                next_char();
            }
            fprintf(stderr, "ERROR: Line %d: Name too long (exceeds %d characters)\n",
                    linenum, MAXSTRSIZE - 1);
            return S_ERROR;
        }
        string_attr[i++] = cbuf;
        next_char();
    }

    string_attr[i] = '\0';

    /* キーワードかどうか判定 */
    return lookup_keyword();
}

/**
 * @brief 数値の読み取り
 * @return TNUMBER または S_ERROR（範囲外の場合）
 */
static int scan_number(void) {
    int num = 0;

    /* 数字が続く限り読み取る */
    while (is_digit(cbuf)) {
        num = num * 10 + (cbuf - '0');
        next_char();

        /* 範囲チェック（0-32768） */
        if (num > 32768) {
            /* 残りの数字を読み飛ばす */
            while (is_digit(cbuf)) {
                next_char();
            }
            fprintf(stderr, "ERROR: Line %d: Number out of range (exceeds 32768)\n", linenum);
            return S_ERROR;
        }
    }

    num_attr = num;
    return TNUMBER;
}

/**
 * @brief 文字列の読み取り
 * @return TSTRING または S_ERROR（閉じられていない場合）
 */
static int scan_string(void) {
    int i = 0;
    int overflow = 0;

    /* 開始の ' をスキップ */
    next_char();

    while (cbuf != EOF) {
        if (cbuf == '\'') {
            /* 次の文字を先読み */
            next_char();

            /* '' の場合は ' として扱う */
            if (cbuf == '\'') {
                if (i < MAXSTRSIZE - 1) {
                    string_attr[i++] = '\'';
                } else {
                    overflow = 1;
                }
                next_char();
            } else {
                /* 文字列の終了 */
                string_attr[i] = '\0';
                if (overflow) {
                    fprintf(stderr, "ERROR: Line %d: String too long (exceeds %d characters)\n",
                            linenum, MAXSTRSIZE - 1);
                    return S_ERROR;
                }
                return TSTRING;
            }
        } else if (cbuf == '\n' || cbuf == '\r') {
            /* 文字列が閉じられていない */
            fprintf(stderr, "ERROR: Line %d: String not closed\n", linenum);
            return S_ERROR;
        } else {
            if (i < MAXSTRSIZE - 1) {
                string_attr[i++] = cbuf;
            } else {
                overflow = 1;
            }
            next_char();
        }
    }

    /* EOF に達したが文字列が閉じられていない */
    fprintf(stderr, "ERROR: Line %d: String not closed (EOF)\n", linenum);
    return S_ERROR;
}

/**
 * @brief {...} 形式の注釈をスキップ
 * @return 正常終了なら0、エラーならS_ERROR
 */
static int skip_comment_brace(void) {
    /* { をスキップ */
    next_char();

    while (cbuf != EOF) {
        if (cbuf == '}') {
            next_char();
            return 0;
        } else if (cbuf == '\n' || cbuf == '\r') {
            process_newline();
        } else {
            next_char();
        }
    }

    /* EOF に達したが注釈が閉じられていない */
    fprintf(stderr, "ERROR: Line %d: Comment not closed (EOF)\n", linenum);
    return S_ERROR;
}

/**
 * @brief slash-asterisk ... asterisk-slash 形式の注釈をスキップ
 * @return 正常終了なら0、エラーならS_ERROR
 */
static int skip_comment_slash(void) {
    /* / をスキップ */
    next_char();
    /* * をスキップ */
    next_char();

    while (cbuf != EOF) {
        if (cbuf == '*') {
            next_char();
            if (cbuf == '/') {
                next_char();
                return 0;
            }
            /* * の後が / でない場合、その文字をループの先頭で処理 */
            continue;
        } else if (cbuf == '\n' || cbuf == '\r') {
            process_newline();
        } else {
            next_char();
        }
    }

    /* EOF に達したが注釈が閉じられていない */
    fprintf(stderr, "ERROR: Line %d: Comment not closed (EOF)\n", linenum);
    return S_ERROR;
}

/**
 * @brief 字句解析の初期化
 * @param filename 入力ファイル名
 * @return 正常終了なら0、エラーならS_ERROR
 */
int init_scan(char *filename) {
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return S_ERROR;
    }

    linenum = 1;
    next_char();  /* 最初の1文字を先読み */

    return 0;
}

/**
 * @brief 次のトークンを返す
 * @return トークン番号、エラーならS_ERROR
 */
int scan(void) {
    /* 分離子（空白、タブ、改行、注釈）をスキップ */
    while (1) {
        /* 空白とタブをスキップ */
        while (cbuf == ' ' || cbuf == '\t') {
            next_char();
        }

        /* 改行処理（4パターン） */
        if (cbuf == '\n' || cbuf == '\r') {
            process_newline();
            continue;
        }

        /* 注釈処理 */
        if (cbuf == '{') {
            if (skip_comment_brace() == S_ERROR) {
                return S_ERROR;
            }
            continue;
        } else if (cbuf == '/') {
            next_char();
            if (cbuf == '*') {
                if (skip_comment_slash() == S_ERROR) {
                    return S_ERROR;
                }
                continue;
            } else {
                /* / の後に * がない場合は不正な文字 */
                fprintf(stderr, "ERROR: Line %d: Invalid character '/' (ASCII 47)\n", linenum);
                return S_ERROR;
            }
        }

        break;
    }

    /* EOF */
    if (cbuf == EOF) {
        return EOF;
    }

    /* 名前またはキーワード */
    if (is_letter(cbuf)) {
        return scan_name();
    }

    /* 数値 */
    if (is_digit(cbuf)) {
        return scan_number();
    }

    /* 文字列 */
    if (cbuf == '\'') {
        return scan_string();
    }

    /* 記号（最長一致） */
    switch (cbuf) {
        case '+':
            next_char();
            return TPLUS;
        case '-':
            next_char();
            return TMINUS;
        case '*':
            next_char();
            return TSTAR;
        case '=':
            next_char();
            return TEQUAL;
        case '<':
            next_char();
            if (cbuf == '=') {
                next_char();
                return TLEEQ;
            } else if (cbuf == '>') {
                next_char();
                return TNOTEQ;
            }
            return TLE;
        case '>':
            next_char();
            if (cbuf == '=') {
                next_char();
                return TGREQ;
            }
            return TGR;
        case '(':
            next_char();
            return TLPAREN;
        case ')':
            next_char();
            return TRPAREN;
        case '[':
            next_char();
            return TLSQPAREN;
        case ']':
            next_char();
            return TRSQPAREN;
        case ':':
            next_char();
            if (cbuf == '=') {
                next_char();
                return TASSIGN;
            }
            return TCOLON;
        case '.':
            next_char();
            return TDOT;
        case ',':
            next_char();
            return TCOMMA;
        case ';':
            next_char();
            return TSEMI;
        default:
            /* 不正な文字 */
            fprintf(stderr, "ERROR: Line %d: Invalid character '%c' (ASCII %d)\n",
                    linenum, cbuf, cbuf);
            next_char();
            return S_ERROR;
    }
}

/**
 * @brief 現在の行番号を返す
 * @return 現在の行番号
 */
int get_linenum(void) {
    return linenum;
}

/**
 * @brief ファイルクローズ
 */
void end_scan(void) {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}
