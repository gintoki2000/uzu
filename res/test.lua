-- global variables
pos_x = 440
pos_y = 260
con = "recuse_brian_3" 

function printf(s, ...)
  return io.write(s:format(...))
end

function _on_level_load(msg)
  if msg.level == 'demon_ruin' then
    scene.new_npc('brian', pos_x, pos_y, con)
  else if msg.level == 'corridor' then
    scene.new_monster('chort', 100, 100)
  end
end

remaining_monsters = 4

function _on_entity_died(msg)
  if msg.entity:is_monster() then
    remaining_monsters = remaining_monsters - 1
  end

  if remaining_monsters == 0 then

  end
end

function _on_item_picked_up(msg)
  printf("you just pick %s up\n", msg.item_name)
  item = inventory.get_item(Item.RED_FLASK)
  if item ~= nil then 
    printf("you have %d red flask(s)", item:get_quality())
  else
    print("you do not have any red flask")
  end
end

