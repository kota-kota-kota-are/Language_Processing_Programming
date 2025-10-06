/**
 * @file id-list.h
 * @brief 識別子リストのヘッダファイル
 * @version 0.1
 * @date 2025-02-26
 */

 /**
  * @struct ID
  * @brief 識別子を管理する線形リスト
  */
typedef struct _id {
  //! 識別子の名前
  char *name;
  //! 識別子の出現回数
  int count;
  //! 次の識別子へのポインタ
  struct _id *nextp;
} ID;

