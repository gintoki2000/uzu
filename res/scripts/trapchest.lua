
function _on_chest_open(msg)
  if msg.chest:get_name() == 'trap' then
    print("trap!!")

    local x, y = msg.chest:get_position()
    local mon1_x = x - 32
    local mon1_y = y 
    scene.new_chort(mon1_x, mon1_y)
  end
end
