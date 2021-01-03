#include "functions.hpp"
#include "engine.hpp"

#include <chrono>
#include <thread>

c_engine g_engine;

//choke packets thread function
//void chokepackets()
//{
//	while (true)
//	{
//		if (g_engine.is_ingame() && choke_packets)
//		{
//			g_engine.send_packets(false);
//			std::this_thread::sleep_for(std::chrono::milliseconds(150));
//			g_engine.send_packets(true);
//			std::this_thread::sleep_for(std::chrono::milliseconds(150));
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}

int main()
{
	//main startup function
	if (!startup()) {
		system("pause");
		return 0;
	}

	const char* cmd = "setpos_player 2 1323.607178 498.532104 189.273682;";
	cmd = "echo hi\necho hey";
	g_engine.Console(cmd);

	c_base_entity local = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				//std::cout << x << std::endl;
				vector vHead0 = entity.get_bone_pos(6);
				Vector3 vHead(vHead0.x, vHead0.y, vHead0.z);
				vHead += Vector3(0, 0, 9);
				Vector3 vFeet;

				if (entity.get_flags() & 1 << 1) { // ducking
					vFeet = vHead - Vector3(0, 0, 50);
				}
				else {
					vFeet = vHead - Vector3(0, 0, 75);
				}
				Vector3 vTop, vBot;

				if (g_engine.WorldToScreen(vHead, vTop) && g_engine.WorldToScreen(vFeet, vBot))
				{
					float h = vBot.y - vTop.y;
					float w = h / 5.0f;
					std::cout << vTop.x << " " << vTop.y << std::endl;
				}
			}
		}
	}



	//return 0;

	//glow team, enemy;

	//team.clr = color{ 0,1,0 };
	//enemy.clr = color{ 1,0,0 };

	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)chokepackets, 0, 0, 0);

	//while (true)
	//{
	//	c_base_entity local = g_engine.get_localplayer();
	//	glow_manager = rpm<DWORD>(client + signatures::dwGlowObjectManager);


	//	//choke packets toggle key
	//	//if (GetAsyncKeyState(VK_NUMPAD5) & 1)
	//	//{
	//	//	choke_packets = !choke_packets;
	//	//}


	//	//getting class id of an entity (you can use this to differentiate entity from entity)
	//	//if (GetAsyncKeyState(VK_NUMPAD0) & 1)
	//	//{
	//	//	system("cls");
	//	//	for (int i = 0; i <= 500; i++)
	//	//	{
	//	//		c_base_entity entity = g_engine.get_entity_from_index(i);
	//	//		if (entity.address)
	//	//		{
	//	//			printf("%i. class id: %i\n", i, entity.get_classid());
	//	//		}
	//	//	}
	//	//}


	//	//bhop
	//	//if (GetAsyncKeyState(VK_SPACE))
	//	//{
	//	//	if (local.get_flags() & FL_ONGROUND) {
	//	//		g_engine.force_jump();
	//	//	}
	//	//}

		//glow
		//for (size_t x = 0; x <= g_engine.get_max_players(); x++)
		//{
		//	c_base_entity entity = g_engine.get_entity_from_index(x);
		//	if (entity.address)
		//	{
		//		if (entity.get_team() == local.get_team())
		//			g_engine.glow_entity(entity, team);
		//		else {
		//			//std::cout << x << std::endl;
		//			g_engine.glow_entity(entity, enemy);
		//			vector vHead0 = entity.get_bone_pos(6);
		//			//std::cout << vHead0.x << " " << vHead0.y << " " << vHead0.z << std::endl;
		//			Vector3 vHead(vHead0.x, vHead0.y, vHead0.z);
		//			vHead += Vector3(0, 0, 9);

		//			Vector3 vFeet;

		//			if (entity.get_flags() & 1 << 1) {
		//				vFeet = vHead - Vector3(0, 0, 50);
		//			}
		//			else {
		//				vFeet = vHead - Vector3(0, 0, 75);
		//			}
		//			Vector3 vTop, vBot;
		//			
		//			if (g_engine.WorldToScreen(vHead, vTop) && g_engine.WorldToScreen(vFeet, vBot))
		//			{
		//				float h = vBot.y - vTop.y;
		//				float w = h / 5.0f;
		//				std::cout << vTop.x << " " << vTop.y << std::endl;
		//			}
		//		}
		//	}
		//}

	//	Sleep(1);
	//}
}