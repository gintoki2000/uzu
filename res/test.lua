count_monster_died = 0; 
num_monter_to_kill = 3

brian = nil

function _on_level_load(msg)
  print("level = ", msg.level)
  brian = scene.new_brian(450,260, "recuse_brian_3")
end

function _on_entity_died(msg)
  if msg.entity:is_monster() then
    local x,y =  msg.entity:get_position()
    scene.new_chort(x,y)
    scene.new_chort(x,y)
    count_monster_died = count_monster_died + 1
  end
  if count_monster_died == num_monter_to_kill then 
    print( msg.entity:get_name() )
    brian:set_dialogue("bloodbath_of_monster")
  end
end


