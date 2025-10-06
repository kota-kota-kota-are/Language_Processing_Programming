/**
 * @file id-list.c
 * @author omzn (@omzn.aquatan.net)
 * @brief  識別子リスト操作の関数群
 * @version 0.1
 * @date 2025-02-26
 * 
 */
#include "scan.h"
#include "id-list.h"

//! 識別子リストの先頭ポインタ
ID *idroot;

/**
 * @brief 識別子リストの初期化
 */
void init_idtab() { /* Initialise the table */
  idroot = NULL;
}

/**
 * @brief 識別子リストから指定する識別子を検索する
 * 
 * @param[in] np 指定する識別子
 * @return ID* 発見した識別子へのポインタ
 */
ID *search_idtab(char *np) { /* search the name pointed by np */
  ID *p;

  for (p = idroot; p != NULL; p = p->nextp) {
    if (!strcmp(np, p->name))
      return (p);
  }
  return (NULL);
}

/**
 * @brief 識別子リストに指定する識別子を登録し，その識別子の出現回数を記録する．
 * 
 * @param[in] np 指定する識別子
 */
void id_countup(char *np) { /* Register and count up the name pointed by np */
  ID *p;
  char *cp;

  if ((p = search_idtab(np)) != NULL)
    p->count++;
  else {
    if ((p = (ID *)malloc(sizeof(ID))) == NULL) {
      error("Cannot malloc for p in id_countup");
      return;
    }
    if ((cp = (char *)malloc(strlen(np) + 1)) == NULL) {
      error("Cannot malloc for cp in id_countup");
      return;
    }
    strcpy(cp, np);
    p->name = cp;
    p->count = 1;
    p->nextp = idroot;
    idroot = p;
  }
}

/**
 * @brief 識別子リストの内容を出力する
 * 
 */
void print_idtab() { /* Output the registered data */
  ID *p;

  for (p = idroot; p != NULL; p = p->nextp) {
    if (p->count != 0)
      printf("\t\"Identifier\" \"%s\"\t%d\n", p->name, p->count);
  }
}

/**
 * @brief 識別子リストのメモリを解放する
 * 
 */
void release_idtab() { /* Release tha data structure */
  ID *p, *q = NULL;

  for (p = idroot; p != NULL; p = q) {
    free(p->name);
    q = p->nextp;
    free(p);
  }
  init_idtab();
}
