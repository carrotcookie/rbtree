#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;

  p->root = nil;
  p->nil = nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  node_t *now = t->root;

  if (now != t->nil)
    delete_node(t, now);
  
  free(t->nil);
  free(t);
}
void delete_node(rbtree *t, node_t *node) {
  if (node->left != t->nil)
    delete_node(t, node->left);
  if (node->right != t->nil)
    delete_node(t, node->right);

  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *p = t->nil;
  node_t *now = t->root;
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;

  while (now != t->nil){
    p = now;
    if (node->key < now->key)
      now = now->left;
    else
      now = now->right;
  }
  node->parent = p;

  if (p == t->nil)
    t->root = node;
  else if (node->key < p->key)
    p->left = node;
  else
    p->right = node;

  node->left = t->nil;
  node->right = t->nil;
  node->color = RBTREE_RED;

  rbtree_insert_fixup(t, node);

  return node;
}

void rbtree_insert_fixup(rbtree *t, node_t *node) {
  while (node->parent->color == RBTREE_RED) {
    if (node->parent == node->parent->parent->left) { //왼쪽 영역일 때
      node_t *p_bro = node->parent->parent->right;

      if (p_bro->color == RBTREE_RED) {
        node->parent->color = RBTREE_BLACK;
        p_bro->color = RBTREE_BLACK; 
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }
      else {
        if (node == node->parent->right) {
          node = node->parent;
          rbtree_left_rotate(t, node);
        }
        
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, node->parent->parent);
      }
    }
    else { // 오른쪽 영역일 때
      node_t *p_bro = node->parent->parent->left;

      if (p_bro->color == RBTREE_RED) {
        node->parent->color = RBTREE_BLACK;
        p_bro->color = RBTREE_BLACK; 
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }
      else {
        if (node == node->parent->left) {
          node = node->parent;
          rbtree_right_rotate(t, node);
        }

        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

void rbtree_left_rotate(rbtree *t, node_t *node) {
  node_t *tmp = node->right;

  node->right = tmp->left;
  if (tmp->left != t->nil)
    tmp->left->parent = node;

  tmp->parent = node->parent;
  if (node->parent == t->nil)
    t->root = tmp;
  else if (node == node->parent->left)
    node->parent->left = tmp;
  else
    node->parent->right = tmp;

  tmp->left = node;
  node->parent = tmp;
}

void rbtree_right_rotate(rbtree *t, node_t *node) {
  node_t *tmp = node->left;

  node->left = tmp->right;
  if (tmp->right != t->nil)
    tmp->right->parent = node;

  tmp->parent = node->parent;
  if (node->parent == t->nil)
    t->root = tmp;
  else if (node == node->parent->left)
    node->parent->left = tmp;
  else
    node->parent->right = tmp;

  tmp->right = node;
  node->parent = tmp;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *now = t->root;

  while (now != t->nil) {
    if (key == now->key)
      return now;
    else if (key < now->key) 
      now = now->left;
    else
      now = now->right;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *now = t->root;

  while (now->left != t->nil)
    now = now->left;
  
  return now;
}

node_t *rbtree_minimum(const rbtree *t, node_t *pivot) {
  while (pivot->left != t->nil)
    pivot = pivot->left;
  
  return pivot;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *now = t->root;

  while (now->right != t->nil)
    now = now->right;
  
  return now;
}

int rbtree_erase(rbtree *t, node_t *origin) {
  node_t *target = origin;
  color_t erased_color = target->color;

  // 실제 삭제될 노드를 대체할 노드를 가르킬 변수 선언
  // 해당 노드에 extra black을 부여할거임
  node_t *erased_sub_node = t->nil;

  // 자식이 없을 때 or 1개만 있을 때
  if (origin->left == t->nil) {
    erased_sub_node = origin->right;
    rbtree_transplant(t, origin, origin->right);
  }
  else if (origin->right == t->nil) {
    erased_sub_node = origin->left;
    rbtree_transplant(t, origin, origin->left);
  }
  // 자식이 2개일 때
  else {
    target = rbtree_minimum(t, origin->right);
    erased_color = target->color;
    erased_sub_node = target->right;

    rbtree_transplant(t, target, target->right);
    rbtree_transplant(t, origin, target);
    target->right = origin->right;
    target->right->parent = target;
    target->left = origin->left;
    target->left->parent = target;
    target->color = origin->color;
  }

  if (erased_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, erased_sub_node);
  
  free(origin);

  return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *node) {
  while (node != t->root && node->color == RBTREE_BLACK) {
    // 왼쪽 영역일 때
    if (node == node->parent->left) {
      node_t *bro = node->parent->right;

      // 형제의 색깔이 RED일 때 -> case.1
      // 형태 변환 후 다시 검사
      if (bro->color == RBTREE_RED) {
        bro->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, node->parent);
        bro = node->parent->right;
      }

      // 형제의 색깔이 BLACK이고 양쪽 자식 모두 BLACK일 때
      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
        bro->color = RBTREE_RED;
        // 재검사를 위한 절차
        node = node->parent;
      }
      // 형제의 색깔이 모두 BLACK이 아닐 때
      else {
        // 형제의 오른쪽만 BLACK일 때 -> case.3
        // 형태를 case.4로 바꿈
        if (bro->right->color == RBTREE_BLACK) {
          bro->left->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          rbtree_right_rotate(t, bro);
          bro = node->parent->right;
        }

        bro->color = node->parent->color;
        node->parent->color = RBTREE_BLACK;
        bro->right->color = RBTREE_BLACK;
        rbtree_left_rotate(t, node->parent);
        // case.4를 해결하면 탈출하기 위해서 root로 초기화
        node = t->root;
      }
    }
    // 오른쪽 영역일 때
    else {
      node_t *bro = node->parent->left;

      if (bro->color == RBTREE_RED) {
        bro->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, node->parent);
        bro = node->parent->left;
      }

      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
        bro->color = RBTREE_RED;
        node = node->parent;
      }
      else {
        if (bro->left->color == RBTREE_BLACK) {
          bro->right->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          rbtree_left_rotate(t, bro);
          bro = node->parent->left;
        }

        bro->color = node->parent->color;
        node->parent->color = RBTREE_BLACK;
        bro->left->color = RBTREE_BLACK;
        rbtree_right_rotate(t, node->parent);
        node = t->root;
      }
    }
  }
  node->color = RBTREE_BLACK;
}

void rbtree_transplant(rbtree *t, node_t *empty, node_t *replace) {
  if (empty->parent == t->nil)
    t->root = replace;
  else if (empty == empty->parent->left)
    empty->parent->left = replace;
  else
    empty->parent->right = replace;

  replace->parent = empty->parent;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  
  return 0;
}