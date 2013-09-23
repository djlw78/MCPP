all: mods


.PHONY: mods
mods:


MOD_OBJ:=$(OBJ)
MOD_LIB:=$(LIB) bin/mcpp.dll


include chat.mk
include command.mk
include info.mk
include player.mk
include world.mk


mods: \
bin/mods/mcpp_auth.dll \
bin/mods/mcpp_ban.dll \
bin/mods/mcpp_brand.dll \
bin/mods/mcpp_disconnect.dll \
bin/mods/mcpp_entity_id.dll \
bin/mods/mcpp_keep_alive.dll \
bin/mods/mcpp_op.dll \
bin/mods/mcpp_ping.dll \
bin/mods/mcpp_player_list.dll \
bin/mods/mcpp_plugin_message.dll \
bin/mods/mcpp_time.dll


#	MINECRAFT.NET AUTHENTICATION


bin/mods/mcpp_auth.dll: \
$(MOD_OBJ) \
obj/auth/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	BANS


bin/mods/mcpp_ban.dll: \
$(MOD_OBJ) \
obj/ban/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	BRAND IDENTIFICATION


bin/mods/mcpp_brand.dll: \
$(MOD_OBJ) \
obj/brand/main.o | \
$(MOD_LIB) \
bin/mods/mcpp_plugin_message.dll
	$(GPP) -shared -o $@ $^ $(MOD_LIB) bin/mods/mcpp_plugin_message.dll
	
	
#	DISCONNECT PACKET HANDLING


bin/mods/mcpp_disconnect.dll: \
$(MOD_OBJ) \
obj/disconnect/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	ENTITY ID GENERATION


bin/mods/mcpp_entity_id.dll: \
$(MOD_OBJ) \
obj/entity_id/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	KEEP ALIVE


bin/mods/mcpp_keep_alive.dll: \
$(MOD_OBJ) \
obj/keep_alive/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)


#	SERVER OPERATORS


bin/mods/mcpp_op.dll: \
$(MOD_OBJ) \
obj/op/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	PING


bin/mods/mcpp_ping.dll: \
$(MOD_OBJ) \
obj/ping/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	PLAYER LIST SUPPORT


bin/mods/mcpp_player_list.dll: \
$(MOD_OBJ) \
obj/player_list/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	PLUGIN MESSAGE SUPPORT


bin/mods/mcpp_plugin_message.dll: \
$(MOD_OBJ) \
obj/plugin_message/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)
	
	
#	TIME


bin/mods/mcpp_time.dll: \
$(MOD_OBJ) \
obj/time/main.o | \
$(MOD_LIB)
	$(GPP) -shared -o $@ $^ $(MOD_LIB)