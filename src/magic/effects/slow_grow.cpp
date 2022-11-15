#include "magic/effects/slow_grow.hpp"
#include "managers/GrowthTremorManager.hpp"
#include "magic/effects/common.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/runtime.hpp"
#include "timer.hpp"

namespace Gts {
	std::string SlowGrow::GetName() {
		return "SlowGrow";
	}


	bool SlowGrow::StartEffect(EffectSetting* effect) { // NOLINT
		auto& runtime = Runtime::GetSingleton();
		return effect == runtime.SlowGrowth;

	}

	void SlowGrow::OnUpdate() {
		const float BASE_POWER = 0.000020;
		const float DUAL_CAST_BONUS = 2.0;
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto& runtime = Runtime::GetSingleton();
		float AlterBonus = caster->GetActorValue(ActorValue::kAlteration) * 0.000025;
		float power = BASE_POWER + AlterBonus;
	
		if (this->timer.ShouldRun()) {
			auto GrowthSound = runtime.growthSound;
			float Volume = clamp(0.15, 1.0, get_visual_scale(caster)/8);
			PlaySound_Frequency(GrowthSound, caster, Volume, 1.0);
		}
		float bonus = 1.0;
		if (PlayerCharacter::GetSingleton()->HasMagicEffect(runtime.EffectSizeAmplifyPotion))
		{
			bonus = get_target_scale(caster) * 0.25 + 0.75;
		}

		Grow(caster, 0.0, power * bonus);
		GrowthTremorManager::GetSingleton().CallRumble(caster, caster, 0.30);
		//log::info("Slowly Growing, actor: {}", caster->GetDisplayFullName());
	}

	void SlowGrow::OnStart() {
		if (IsDualCasting()) {
			this->IsDual = true;
		} else {
			this->IsDual = false;
		}
	}

	void SlowGrow::OnFinish() {
	}
}
