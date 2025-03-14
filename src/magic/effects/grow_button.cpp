#include "magic/effects/grow_button.hpp"
#include "magic/effects/common.hpp"
#include "managers/GrowthTremorManager.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/runtime.hpp"
#include "util.hpp"

namespace Gts {
	std::string GrowButton::GetName() {
		return "GrowButton";
	}

	bool GrowButton::StartEffect(EffectSetting* effect) { // NOLINT
		auto& runtime = Runtime::GetSingleton();

		return effect == runtime.GrowPcButton;
	}

	void GrowButton::OnStart() {
		Actor* caster = GetCaster();
		if (!caster) {
			return;
		}
		auto& runtime = Runtime::GetSingleton();
		auto GrowthSound = runtime.growthSound;
		float Volume = clamp(0.50, 2.0, get_visual_scale(caster));
		PlaySound(GrowthSound, caster, Volume, 0.0);
		
		//log::info("Grow Button actor: {}", caster->GetDisplayFullName());

	}

	void GrowButton::OnUpdate() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto& runtime = Runtime::GetSingleton();

		float caster_scale = get_visual_scale(caster);
		float stamina = clamp(0.05, 1.0, GetStaminaPercentage(caster));
		DamageAV(caster, ActorValue::kStamina, 0.45 * (caster_scale * 0.5 + 0.5) * stamina * TimeScale());
		Grow(caster, 0.0030 * stamina, 0.0);
		GrowthTremorManager::GetSingleton().CallRumble(caster, caster, 1.0);
	}
}
