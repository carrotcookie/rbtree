#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
	// rbtree를 위한 메모리 할당
  // rbtree의 root와 nil 초기화
    
	rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
	
  p->root = nil;
  p->nil = nil;

  return p;
}

void delete_rbtree(rbtree *t) {
	// root에서부터 삭제
  node_t *now = t->root;

  if (now != t->nil)
    delete_node(t, now);
  	
  // root는 delete_node 호출 시 메모리 해제 됨
  // rbtree의 nil은 따로 해제
  free(t->nil);
  free(t);
}

void delete_node(rbtree *t, node_t *node) {
  if (node->left != t->nil)
    delete_node(t, node->left);
  if (node->right != t->nil)
    delete_node(t, node->right);
	
  // 전부 찾아서 메모리 해제
  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 현재 위치 노드 초기화
  node_t *now = t->root;
  // 현재노드의 부모노드로 사용할 변수 초기화
  node_t *p = t->nil;
	  
  // 받은 키값을 가지는 노드 생성
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;
  
  // 삽입될 적절한 위치 찾기
  while (now != t->nil){
    p = now;
        
    if (node->key < now->key)
      	now = now->left;
    else
      	now = now->right;
  }
  // node의 부모를 p로 설정
  node->parent = p;

  if (p == t->nil)
    t->root = node;
  // p의 자식을 node로 설정
  else if (node->key < p->key)
    p->left = node;
  else
    p->right = node;

  node->left = t->nil;
  node->right = t->nil;
  node->color = RBTREE_RED;
	
  // 룰 위반여부 검사
  rbtree_insert_fixup(t, node);

  return node;
}

void rbtree_insert_fixup(rbtree *t, node_t *node) {
	// #4 위반 시 무한반복
	while (node->parent->color == RBTREE_RED) {
    // 할아버지 기준 좌측영역
    if (node->parent == node->parent->parent->left) {
      node_t *p_bro = node->parent->parent->right;
      // case.1 부모의 형제가 RED일 때
      if (p_bro->color == RBTREE_RED) {
        // 할아버지 BLACK과 부모라인 RED 교환
        node->parent->color = RBTREE_BLACK;
        p_bro->color = RBTREE_BLACK; 
        node->parent->parent->color = RBTREE_RED;
        // 할아버지 기준으로 #4 위반하는지 재검사
        node = node->parent->parent;
      }
      // case.2, case.3 부모의 형제가 BLACK일 때
      else {
        // case.2 꺾인 형태일 때
        if (node == node->parent->right) {
          // 회전 후 case.3으로 만듦
          node = node->parent;
          rbtree_left_rotate(t, node);
        }
				// case.3 뻗은 형태일 때
        // 할아버지 BLACK과 부모의 RED 교환 후 회전
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, node->parent->parent);
      }
    }
    // 할아버지 기준 우측영역. 좌측영역의 대칭과 동일
    else {
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
  // #2를 위반 시 BLACK으로 바꿔주면 해결
  t->root->color = RBTREE_BLACK;
}

void rbtree_left_rotate(rbtree *t, node_t *node) {
  node_t *tmp = node->right;

  // node와 tmp->left 상호연결
  node->right = tmp->left;
  if (tmp->left != t->nil)
    tmp->left->parent = node;

  // tmp와 node->parent 상호연결
  tmp->parent = node->parent;
  if (node->parent == t->nil)
    t->root = tmp;
  else if (node == node->parent->left)
    node->parent->left = tmp;
  else
    node->parent->right = tmp;

  // tmp와 node 상호연결
  tmp->left = node;
  node->parent = tmp;
}

void rbtree_right_rotate(rbtree *t, node_t *node) {
  node_t *tmp = node->left;

  // node와 tmp->right 상호연결
  node->left = tmp->right;
  if (tmp->right != t->nil)
    tmp->right->parent = node;

  // tmp와 node->parent 상호연결
  tmp->parent = node->parent;
  if (node->parent == t->nil)
    t->root = tmp;
  else if (node == node->parent->left)
    node->parent->left = tmp;
  else
    node->parent->right = tmp;

  // tmp와 node 상호연결
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

node_t *rbtree_successor(const rbtree *t, node_t *pivot) {
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
	// 현재 target은 원래 삭제하려던 노드 origin
  node_t *target = origin;
  // 실제 삭제될 색
  color_t erased_color = target->color;

  // 실제 삭제될 노드를 대체할 노드를 가르킬 변수
  // 해당 노드에 extra black을 부여할거임
  node_t *erased_sub_node = t->nil;

  // 자식이 0개 or 1개
  if (target->left == t->nil) {
    erased_sub_node = target->right;
    rbtree_transplant(t, target, erased_sub_node);
  }
  else if (target->right == t->nil) {
    erased_sub_node = target->left;
    rbtree_transplant(t, target, erased_sub_node);
  }
  // 자식이 2개
  else {
    // target이 successor로 바뀜
    target = rbtree_successor(t, target->right);
    erased_color = target->color;
    erased_sub_node = target->right;
  
    // origin, target, erased_sub_node 위치 변환
    rbtree_transplant(t, target, erased_sub_node);
    rbtree_transplant(t, origin, target);
    target->right = origin->right;
    target->right->parent = target;
    target->left = origin->left;
    target->left->parent = target;
    target->color = origin->color;
  }
  // 삭제되는 색이 BLACK이라면 extra black을 처리해줄 추가작업
  if (erased_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, erased_sub_node);
  
  // 삭제되는거는 target의 색인거지 target 노드가 아님
  // 삭제되는 노드는 origin임
  free(origin);

  return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *node) {
	// doubly black이면 무한반복
  // doubly black인데 root이면 탈출
  while (node != t->root && node->color == RBTREE_BLACK) {
    // 좌측영역
    if (node == node->parent->left) {
      node_t *bro = node->parent->right;

      // case.1 형제가 RED일 때
      if (bro->color == RBTREE_RED) {
        // 부모 BLACK과 형제 RED 교환 후 회전
        // case.2, case.3, case.4으로 변환
        bro->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, node->parent);
        bro = node->parent->right;
      }
    
      // case.2, case.3, case.4 형제가 BLACK일 때
          
      // case.2 형제의 자식 모두 BLACK일 때
      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
        // 공통속성 나의 extra black과 형제의 BLACK을 부모에게 옮김
        bro->color = RBTREE_RED;
        // 부모가 extra black을 받았으니 재검사
        node = node->parent;
      }
      else {
        // case.3 형제의 왼쪽만 RED일 때
        if (bro->left->color == RBTREE_RED) {
          // 형제의 BLACK과 형제자식의 RED 교환 후 회전
          //case.4로 변환
          bro->left->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          rbtree_right_rotate(t, bro);
          bro = node->parent->right;
        }
        // case. 4
        // 형제의 색을 부모의 색으로
        // 부모와 형제의 RED자식을 BLACK으로
        // 부모 기준 회전
        bro->color = node->parent->color;
        node->parent->color = RBTREE_BLACK;
        bro->right->color = RBTREE_BLACK;
        rbtree_left_rotate(t, node->parent);
            
        // case.4를 해결 시 탈출을 위한 root로 초기화
        node = t->root;
      }
    }
    // 우측영역. 좌측영역의 대칭과 동일
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
        if (bro->right->color == RBTREE_RED) {
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
	// replace와 empty->parent를 상호연결하는 함수
    
  if (empty->parent == t->nil)
    t->root = replace;
  else if (empty == empty->parent->left)
    empty->parent->left = replace;
  else
    empty->parent->right = replace;

  replace->parent = empty->parent;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // 오름차순 출력 -> 중위순회(inorder)
  if (t->root == t->nil)
    return -1;

  int idx = 0;
  rbtree_in_order(t, t->root, arr, &idx);

  return 0;
}

void rbtree_in_order(const rbtree *t, node_t *now, key_t *arr, int *idx) {
  if (now->left != t->nil) {
    rbtree_in_order(t, now->left, arr, idx);
  }

  arr[*idx] = now->key;
  *idx += 1;

  if (now->right != t->nil)
    rbtree_in_order(t, now->right, arr, idx);
}