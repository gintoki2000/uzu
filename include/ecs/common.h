#ifndef ECS_COMMON_H
#define ECS_COMMON_H
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t ecs_entity_t;
typedef uint16_t ecs_size_t;

typedef void (*ecs_comp_init_fn_t)(void*);
typedef void (*ecs_comp_fini_fn_t)(void*);
typedef void (*ecs_comp_cpy_fn_t)(void*, const void*);

typedef struct EcsType
{
  ecs_comp_init_fn_t init_fn;
  ecs_comp_init_fn_t fini_fn;
  ecs_comp_cpy_fn_t  cpy_fn;
  ecs_size_t         size;
} EcsType;

#define ECS_ENT_VER_SHIFT 16
#define ECS_ENT_IDX_SHIFT 0
#define ECS_SIZE_MAX 0xffff
#define ECS_ENT_MAX 0xffffffff

#define ECS_NULL_ENT ECS_ENT_MAX
#define ECS_NULL_IDX ECS_SIZE_MAX

#define ECS_ENT_IDX(e) (((e) >> ECS_ENT_IDX_SHIFT) & ECS_SIZE_MAX)
#define ECS_ENT_VER(e) (((e) >> ECS_ENT_VER_SHIFT) & ECS_SIZE_MAX)
#define ECS_ENT(idx, ver) (((idx) << ECS_ENT_IDX_SHIFT) | ((ver) << ECS_ENT_VER_SHIFT))

#define ECS_ENT_FMT_PATTERN "[%4u | %4u]"
#define ECS_ENT_FMT_VARS(_ent) ECS_ENT_IDX(_ent), ECS_ENT_VER(_ent)

#endif // ECS_COMMON_H
