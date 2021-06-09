// ngotrung Sun 30 May 2021 08:20:52 AM +07
#ifndef BT_ROOT_H
#define BT_ROOT_H

#define BT_ROOT(ptr) ((bt_Root*)ptr)

typedef struct bt_Root bt_Root;
struct bt_Node;
struct bt_Root* bt_root_create();
void            bt_root_attach(struct bt_Root* self, struct bt_Node* child);
struct bt_Node* bt_root_child(struct bt_Root* self);
#endif // BT_ROOT_H
