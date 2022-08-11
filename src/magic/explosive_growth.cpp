#include "magic/explosive_growth.h"
#include "magic/magic.h"
#include "scale/scale.h"

namespace Gts {
	void explosive_growth(Actor* target) {
		float one = 2.0;
		float two = 4.0;
		float three = 6.0;

		BGSPerk* extra_growth = find_form<BGSPerk>("GTS.esp|332563");
		TESGlobal* progression_multiplier_global = find_form<TESGlobal>("GTS.esp|37E46E");
		TESGlobal* SizeLimit = find_form<TESGlobal>("GTS.esp|0x2028B4");
		float progression_multiplier = progression_multiplier_global->value;
		float size_limit = SizeLimit->value;

		if (target->HasPerk(extra_growth))
			
		{
		 one = 4;
		 two = 8;
		 three = 12;
		}
		
		float scale = get_visual_scale(target);
		if (scale <= size_limit) 
		{
			EffectSetting* growth_three = find_form<EffectSetting>("GTS.esp|007928"); // 3
			EffectSetting* growth_two = find_form<EffectSetting>("GTS.esp|1E42A5"); // 2
			EffectSetting* growth_one = find_form<EffectSetting>("GTS.esp|1E42A6"); // 1
			if (target->HasMagicEffect(growth_three) && scale <= three) // Explosive Growth Part 3 (Strongest)
			{
				mod_target_scale(target, (0.00480 * progression_multiplier));
			}
			if (target->HasMagicEffect(growth_two) && scale <= two) // Explosive Growth Part 2 (Medium)
			{
				mod_target_scale(target, (0.00300 * progression_multiplier));
			}
			if (target->HasMagicEffect(growth_one && scale <= one) // Explosive Growth Part 1 (Small)
			{
				mod_target_scale(target, (0.00175 * progression_multiplier));
			}
		}
	}
}
