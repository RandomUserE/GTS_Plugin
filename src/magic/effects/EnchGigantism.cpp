#include "magic/effects/EnchGigantism.hpp"
#include "magic/effects/common.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "util.hpp"
#include "managers/GtsSizeManager.hpp"

namespace Gts {
	std::string Gigantism::GetName() {
		return "Gigantism";
	}

	bool Gigantism::StartEffect(EffectSetting* effect) { // NOLINT
		auto& runtime = Runtime::GetSingleton();
		log::info("Starting Gigantism Effect");
		return effect == runtime.EnchGigantism;
	}

	void Gigantism::OnStart() {
		auto& runtime = Runtime::GetSingleton();
		auto caster = GetCaster();

		if (!caster) {
			return;
		}
		float GigantismPower = GetActiveEffect()->magnitude;
		SizeManager::GetSingleton().ModExtraMaxSize(caster, GigantismPower);
		log::info("Increasing GigantismPower: {}, Actor: {}", SizeManager::GetSingleton().GetExtraMaxSize(caster), caster->GetDisplayFullName());
	}

	void Gigantism::OnUpdate() {

	}


	void Gigantism::OnFinish() {
		auto& runtime = Runtime::GetSingleton();
		auto caster = GetCaster();

		if (!caster) {
			return;
		}
		float GigantismPower = GetActiveEffect()->magnitude;
		SizeManager::GetSingleton().ModExtraMaxSize(caster, -GigantismPower);
		log::info("Decreasing GigantismPower: {}, Actor: {}", SizeManager::GetSingleton().GetExtraMaxSize(caster), caster->GetDisplayFullName());
	}
}
