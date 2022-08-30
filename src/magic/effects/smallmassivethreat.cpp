#include "magic/effects/smallmassivethreat.hpp"
#include "magic/effects/common.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/runtime.hpp"

namespace Gts {
	std::string SmallMassiveThreat::GetName() {
		return "SmallMassiveThreat";
	}

    bool SmallMassiveThreat::StartEffect(EffectSetting* effect) { // NOLINT
		auto& runtime = Runtime::GetSingleton();
		return effect == runtime.SmallMassiveThreat;
		
	}

	void SmallMassiveThreat::OnUpdate() {
		const float BASE_POWER = 0.00035;
		const float DUAL_CAST_BONUS = 2.0;
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		float CasterScale = get_visual_scale(caster);
        if (CasterScale >= 2.0)
        {Dispel();} // <- Disallow having it when scale is > 2.0
	}

	inline float SmallMassiveThreat::Augmentation() {
		auto Player = PlayerCharacter::GetSingleton();
		auto& runtime = Runtime::GetSingleton();
		auto AugmentationPerk = runtime.NoSpeedLoss;
		float MSBonus = this->MovementSpeedBonus;
		if (Player->IsSprinting() && Player->HasMagicEffect(runtime.SmallMassiveThreat))
		{
			this->MovementSpeedBonus += 0.00005;
		}
		else if (Player->IsSprinting() && Player->HasPerk(AugmentationPerk) && Player->HasMagicEffect(runtime.SmallMassiveThreat))
		{
			this->MovementSpeedBonus += 0.000075;
		}
		else
		{
			this->MovementSpeedBonus = 0.0;
		}
		if (MSBonus >= 1.0)
		{Debug.Notification("Ready to Insta-Crush someone");}
		float MSBonus = clamp(0.0, 1.0, this->MovementSpeedBonus); 
		return MSBonus;
	}

	void SmallMassiveThreat::OverrideBonus(float Value) {
		this->MovementSpeedBonus = Value;
	}
}
