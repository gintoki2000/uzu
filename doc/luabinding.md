# SCRIPTING API 
## 1.Entity
### 1.1 Fucntions
**Entity.get_name(entity)**
  >get name of given entity
  ***@param entity[Entity]*** an entity
  ***@return*** name of given entity or nil
  
  example: 
  ```lua
  if entity:get_name() == 'brian' then
    print('you are Brian')
  end
  ```

**Entity.get_dialogue(entity)**
  >get current dialogue id of given entity
  ***@param entity[Entity]*** an entity
  ***@return[string]*** dialogue id of given entity or nil
  
  example:
  ```lua
  --- print out dialogue id if exist
  local dialogue_id = entity:get_dialogue()
  if dialogue_id ~= nil then
    print('current dialogue is', dialogue_id)
  end
  ```

**Entity.set_dialogue(entity, dialogue)**
  >set new dialogue for given entity by id
  ***@param entity[Entity]*** an entity
  ***@param dialogue[string]*** dialogue id
  
   example:
  ```lua
entity:set_dialogue('brian_dialogue_2')
  ```

## 2 Item
Item is an object manageed by inventory 
### 2.1 Constances
**Item**.*RED_FLASK*
  >id of "RedFlask" item type

**Item**.*BLUE_FLASK*
  >id of "BlueFlask" item type

**Item**.BIG_RED_FLASK
  >id of "BigRedFlask" item type
### 2.2 Functions
**Item.get_name(item)**
  >get name of given item
  ***@param item[Item]*** an item
  ***@return[string]*** item name
     example:
  ```lua
local name = item:get_name()
  ```

**Item.get_quality(item)**
  >get quality of given item
  ***@param item[Item]*** an item
  ***@return[number]*** item quality

**Item.set_quality(item, quality)**
  >set quality of given item
  ***@param item[Item]*** an item
  ***@param quality[number]*** new item quality

**Item.use(item)**
  >use given item and decrease it quality by one
  ***@param item[Item]*** an item

**Item.drop(item, n)**
>drop item by n
***@param item[Item]*** an item
***@param n[number]*** how many you want to drop


## 3 Inventory
**inventory.add_item(type_id, quality)**
  >add new item to player inventory
**inventory.get_item(type_id)**
## 4 Scene





