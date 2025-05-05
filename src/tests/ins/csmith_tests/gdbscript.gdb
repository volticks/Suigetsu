# https://stackoverflow.com/questions/14261404/how-to-prevent-gdb-to-stop-after-next-or-finish-command
define myfinish
	finish
end

define hookpost-myfinish
	set $var1 = (uint32_t)returned
	#set $name = 
	set logging on
	printf "returned: %08x\n", $var1
	set logging off
end

break *func_1 
commands 
	myfinish
	continue
end

set logging file gdb.output
run
