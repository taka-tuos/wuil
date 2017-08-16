function wuil_main()
	obj = wuil_createwindow(640,480,"wuilcfg");
	local c = 0
	stc = wuil_getticks()
	--bt = wuil_createbutton(obj,16,16,512,256,"ファイル")
	while true do
		wuil_pollevent()
		--wuilgdi_clear(obj)
		--wuilgdi_drawstring(obj,math.floor(math.random(640+64)-64),math.floor(math.random(480+64)-64),math.floor(math.random(16777216)),"Somebody screeeeeem!!")
		x0 = math.floor(math.random(639))
		x1 = math.floor(math.random(639))
		y0 = math.floor(math.random(479))
		y1 = math.floor(math.random(479))
		--wuilgdi_drawbox(obj,x0,y0,x1,y1,math.floor(math.random(16777216)))
		--wuilgdi_drawline(obj,x0,y0,x1,y1,math.floor(math.random(16777216)))
		wuilgdi_bitblt(obj,x0,y0,"usb_android_connected.png")
		--[[if wuil_checkbutton(bt) ~= 0 then
			--obj2 = wuil_createwindow(640,480,"wuilcfg");
		end]]--
		wuil_reflesh(obj)
		c = c + 1
		if wuil_getticks() - stc >= 1000 then
			wuil_setwindowtitile(obj,string.format("wuilcfg %d gdiscore",c))
			c = 0
			stc = wuil_getticks()
		end
	end
end
