#pragma once

namespace About
{
	static constexpr char name[] = "IOECU";
	static constexpr char desc[] = "IO board control board for Pixel project";
	static constexpr char board_type = 0x10;		// 5 bits
	static constexpr char board_ver = 3;		// 3 bits
	static constexpr char soft_ver = 3;			// 6 bits
	static constexpr char can_ver = 1;			// 2 bits
	static constexpr char git[] = "https://github.com/starfactorypixel/SlaveECU_IO";
	
	inline void Setup()
	{
		Logger.PrintNewLine();
		Logger.PrintTopic("INFO").Printf("%s, board:%d, soft:%d, can:%d", name, board_ver, soft_ver, can_ver).PrintNewLine();
		Logger.PrintTopic("INFO").Printf("Desc: %s", desc).PrintNewLine();
		Logger.PrintTopic("INFO").Printf("Build: %s %s", __DATE__, __TIME__).PrintNewLine();
		Logger.PrintTopic("INFO").Printf("GitHub: %s", git).PrintNewLine();
		Logger.PrintTopic("READY").PrintNewLine();
		
		return;
	}
	
	inline void Loop(uint32_t &current_time)
	{
		return;
	}
}
