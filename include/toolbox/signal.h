#ifndef SIGNAL_H
#define SIGNAL_H
#include "common.h"
#include <stdint.h>
/* Hiện tại hàm disconnect đang có vấn đề
 * nếu trong quá trình emit signal cái slot nhận sginal tự
 * disconnect chính nó hoặc một slot khác cùng nhận loại signal tương tự.
 * TODO: tạo một buffer các slot/object cần disconnect. disconnect
 *  	chỉ thực hiện một khi hàm update được gọi
 * */

typedef struct Signal     Signal;
typedef struct Dispatcher Dispatcher;

Signal* signal_new();
void    signal_destroy(Signal* sig);
void    signal_connect(Signal* sig, pointer_t user_data, funcptr_t func);
void    signal_disconnect(Signal* sig, pointer_t func_or_instance);
void    signal_emit(Signal* sig, const void* evt);

Dispatcher* dispatcher_new(u32 num_singals);
void        dispatcher_destroy(Dispatcher* dispatcher);
void        dispatcher_connect(Dispatcher* self, int sig, pointer_t user_data, funcptr_t func);
void        dispatcher_disconnect(Dispatcher* self, int sig, pointer_t func_or_instance);
void        dispatcher_emit(Dispatcher* self, int sig, const void* evt);

#endif // SIGNAL_H
