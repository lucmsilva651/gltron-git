-- loop until RETURN_QUIT code is sent

RETURN_QUIT = 0
RETURN_GAME_END = 1
RETURN_GAME_PAUSE = 2
RETURN_GAME_UNPAUSE = 3
RETURN_GAME_CREDITS = 4
RETURN_GAME_ESCAPE = 5
RETURN_GUI_PROMPT = 6
RETURN_TIMEDEMO = 7
RETURN_TIMEDEMO_ABORT = 8
RETURN_CREDITS = 9
RETURN_GAME_LAUNCH = 10
RETURN_GUI_ESCAPE = 11
RETURN_PROMPT_ESCAPE = 12
RETURN_PAUSE_ESCAPE = 13

-- enable below to run in timedemo-only mode
if(nil) then
	SetCallback("timedemo")
	c_mainLoop()
	os.exit()
end

callback = "gui"

next_callback = {}
next_callback[ RETURN_GAME_LAUNCH ] = "pause"
next_callback[ RETURN_GAME_END ] = "pause"
next_callback[ RETURN_GAME_PAUSE ] = "pause"
next_callback[ RETURN_GAME_UNPAUSE ] = "game"
next_callback[ RETURN_CREDITS ] = "credits"
next_callback[ RETURN_GAME_CREDITS ] = "credits"

next_callback[ RETURN_GAME_ESCAPE ] = "gui"
next_callback[ RETURN_GUI_ESCAPE ] = "pause"
next_callback[ RETURN_PROMPT_ESCAPE ] = "gui"
next_callback[ RETURN_PAUSE_ESCAPE ] = "gui"

next_callback[ RETURN_GUI_PROMPT ] = "configure"
next_callback[ RETURN_TIMEDEMO ] = nil
next_callback[ RETURN_TIMEDEMO_ABORT ] = nil
next_callback[ RETURN_QUIT ] = nil


while 1 do
	SetCallback(callback)
	-- io.write(string.format("[lua] setting callback '%s'\n", callback))
	 
	status = c_mainLoop()
	-- io.write(string.format("[lua] system returned (%d)\n", status))
	 
	if(next_callback[ status ]) then
		 callback = next_callback[ status ]
	else
		if(status == RETURN_QUIT) then
			io.write(string.format("[lua] clean exit\n"))
			os.exit()
                else
			io.write(string.format("[lua] unhandled callback (%d)\n", status))
			os.exit()
		end
	end
end
			
