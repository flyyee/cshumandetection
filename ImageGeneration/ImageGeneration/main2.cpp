//#include "functions.hpp"
//#include "engine.hpp"
//
//#include <chrono>
//#include <thread>
//
//c_engine g_engine;
//
//int main()
//{
//	//main startup function
//	if (!startup()) {
//		system("pause");
//		return 0;
//	}
//
//	const char* cmd = "setpos_player 2 1323.607178 498.532104 189.273682;";
//	cmd = "echo hi\necho hey";
//	g_engine.Console(cmd);
//
//	c_base_entity local = g_engine.get_localplayer();
//	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
//	{
//		c_base_entity entity = g_engine.get_entity_from_index(x);
//		if (entity.address)
//		{
//			if (entity.get_team() != local.get_team()) {
//				//std::cout << x << std::endl;
//				vector vHead0 = entity.get_bone_pos(6);
//				Vector3 vHead(vHead0.x, vHead0.y, vHead0.z);
//				vHead += Vector3(0, 0, 9);
//				Vector3 vFeet;
//
//				if (entity.get_flags() & 1 << 1) { // ducking
//					vFeet = vHead - Vector3(0, 0, 50);
//				}
//				else {
//					vFeet = vHead - Vector3(0, 0, 75);
//				}
//				Vector3 vTop, vBot;
//
//				if (g_engine.WorldToScreen(vHead, vTop) && g_engine.WorldToScreen(vFeet, vBot))
//				{
//					float h = vBot.y - vTop.y;
//					float w = h / 5.0f;
//					std::cout << vTop.x << " " << vTop.y << std::endl;
//				}
//			}
//		}
//	}
//
//	return 0;
//}